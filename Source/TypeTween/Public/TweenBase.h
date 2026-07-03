// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "TweenEasing.h"
#include "TweenControl.h"
#include "TweenHandle.h"

namespace TypeTween::Detail {
	struct FTweenFrame {
		float Alpha;
		uint64 FrameCount;
	};

	enum class ECyclePhase : uint8 {
		Forward, /* 0 -> 1 */
		ReverseDelay, /* at 1 */
		Reverse, /* 1 -> 0 */
		RepeatDelay /* at 0 */
	};

	template<typename Derived>
	class TweenBase : public ITweenControl {
	public:
		// ------------------------------------------------------------------ config
		/* Delay before the tween starts after being interpolated */
		Derived& StartDelay(float Secs) { this->Settings.Delays.Start = Secs; return Self(); }
		/* Total time for one forward or reverse playthrough, not including delays. Required. */
		Derived& Duration(float Secs) { this->Settings.Duration = Secs; return Self(); }
		/* [Ping Pong Only] Delay between forward and reverse play */
		Derived& ReverseDelay(float Secs) { this->Settings.Delays.Reverse = Secs; return Self(); }
		/* Delay between the end of one cycle and the start of the next */
		Derived& RepeatDelay(float Secs) { this->Settings.Delays.Repeat = Secs; return Self(); }
		/* Delay after the tween finishes before firing OnComplete */
		Derived& EndDelay(float Secs) { this->Settings.Delays.End = Secs;  return Self(); }

		/* 0 = play once, -1 = infinite, N = play N+1 times total */
		Derived& Repeat(int32 Count) { this->Settings.RepeatCount = Count; return Self(); }
		/* Easing function for the tween, see https://easings.net/ for visualization */
		Derived& Ease(ETweenEase E) { this->Settings.Ease = E; return Self(); }
		/* Tween will reverse direction each cycle instead of jumping back to start */
		Derived& PingPong(bool Enable = true) {
			this->Settings.LoopMode = Enable ? ETweenLoopMode::PingPong : ETweenLoopMode::Restart;
			return Self();
		}

		/* Custom easing curve. Overrides ease value. */
		Derived& CustomEaseCurve(const FRuntimeFloatCurve& Curve) {
			this->Settings.Ease = ETweenEase::CustomCurve;
			this->Settings.CustomEaseCurve = Curve;
			return Self();
		}

		Derived& Preset(FTweenSettings Preset) { this->Settings = MoveTemp(Preset); return Self(); }

		// -------------------------------------------------- callbacks (C++ TFunction setters)
		/* Fired first frame when the tween is triggered, before StartDelay begins */
		Derived& OnPreStart(TFunction<void()> Fn) { Callbacks.OnPreStartFn = MoveTemp(Fn); return Self(); }
		/* (Start Delay...) */
		/* Fired once the tween starts playing (after StartDelay) */
		Derived& OnStart(TFunction<void()> Fn) { Callbacks.OnStartFn = MoveTemp(Fn); return Self(); }
		/* Fired at the start of each new cycle (after StartDelay or RepeatDelay) */
		Derived& OnCycleBegin(TFunction<void()> Fn) { Callbacks.OnCycleBeginFn = MoveTemp(Fn); return Self(); }
		/* (Duration...) */
		/* Fired at the end of the forward phase of each cycle (before ReverseDelay), when t == 1 */
		Derived& OnForwardEnd(TFunction<void()> Fn) { Callbacks.OnForwardEndFn = MoveTemp(Fn); return Self(); }
		/* (Reverse Delay...) */
		/* [Ping Pong Only] Fired at the start of the reverse phase of each cycle (after ReverseDelay) */
		Derived& OnReverseBegin(TFunction<void()> Fn) { Callbacks.OnReverseBeginFn = MoveTemp(Fn); return Self(); }
		/* (Duration...) */
		/* Fired at the end of each cycle (before EndDelay or RepeatDelay), when t == 0 */
		Derived& OnCycleEnd(TFunction<void()> Fn) { Callbacks.OnCycleEndFn = MoveTemp(Fn); return Self(); }
		/* (Repeat Delay...) */
		/* Fired at the end of each cycle except the last one */
		Derived& OnRepeat(TFunction<void()> Fn) { Callbacks.OnRepeatFn = MoveTemp(Fn); return Self(); }
		/* (End Delay...) */
		/* Fired once the tween finishes playing (after EndDelay + repeats) */
		Derived& OnComplete(TFunction<void()> Fn) { Callbacks.OnCompleteFn = MoveTemp(Fn); return Self(); }

		/* Fired every tick, including during delays. Unlike OnUpdate, always fires even when alpha is not changing */
		Derived& OnTick(TFunction<void()> Fn) { Callbacks.OnTickFn = MoveTemp(Fn); return Self(); }
		/* [Note] OnUpdate callback is handled by ITween<T> specialization and gets called during the interpolation */

		// --------------------------------------------------------------- control
		void Pause() override { bPaused = true; }
		void Resume() override { bPaused = false; }
		void Restart() override {
			Elapsed = 0.f;
			FrameCount = 0;
			bStartFired = false;
			bFinalized = false;
			bCompleteFired = false;
			LastCycleIndex = 0;
			LastPhase = ECyclePhase::Forward;
			bPaused = false;
		}
		void Finish() override { Elapsed = GetMaxElapsed(); }

		bool IsPaused() const override { return bPaused; }
		bool IsDone()   const override {
			if (bKilled) return true;
			if (this->Settings.RepeatCount < 0) return false;
			return Elapsed >= GetMaxElapsed();
		}

		/** Immediately stops the tween without firing any callbacks */
		void Kill() override {
			bKilled = true;
			bCompleteFired = true;
			bFinalized = true;
		}

		bool IsKilled() const override { return bKilled; }

		// -------------------------------------------------- type-erased access (ITweenControl)
		FTweenSettings& GetSettings() override { return this->Settings; }
		const FTweenSettings& GetSettings() const override { return this->Settings; }

		FTweenCallbacks& GetCallbacks() override { return Callbacks; }
		const FTweenCallbacks& GetCallbacks() const override { return Callbacks; }

		// ------------------------------------------------------------------ tick
		void Tick(float DeltaTime) {
			if (bPaused || IsDone()) return;

			if (Elapsed == 0.f) Callbacks.BroadcastOnPreStart();

			Elapsed += DeltaTime;
			if (this->Settings.RepeatCount >= 0)
				Elapsed = FMath::Min(Elapsed, GetMaxElapsed());

			/* Fires every frame, even during delays */
			Callbacks.BroadcastOnTick();

			if (Elapsed < this->Settings.Delays.Start) return;

			if (!bStartFired) {
				bStartFired = true;
				Callbacks.BroadcastOnStart();
				Callbacks.BroadcastOnCycleBegin();
			}

			const float AnimElapsed = Elapsed - this->Settings.Delays.Start;
			const float TotalAnimTime = GetTotalAnimTime();

			if (AnimElapsed + KINDA_SMALL_NUMBER >= TotalAnimTime) {
				if (!bFinalized) {
					bFinalized = true;

					const float FinalRaw = (this->Settings.LoopMode == ETweenLoopMode::PingPong) ? 0.f : 1.f;
					const float FinalAlpha = EvaluateEase(FinalRaw);
					Interpolate({ FinalAlpha, FrameCount });
					FrameCount++;
					if (LastPhase == ECyclePhase::Forward) Callbacks.BroadcastOnForwardEnd();
					Callbacks.BroadcastOnCycleEnd();
				}

				if (!bCompleteFired && Elapsed >= GetMaxElapsed()) {
					bCompleteFired = true;
					Callbacks.BroadcastOnComplete();
				}
				return;
			}

			bFinalized = false;

			bool bReversing = false;
			ECyclePhase Phase = ECyclePhase::Forward;
			const float RawAlpha = ComputeProgress(AnimElapsed, bReversing, Phase);
			const float Alpha = bReversing
				? 1.f - EvaluateEase(RawAlpha)
				: EvaluateEase(RawAlpha);

			const bool bActivePhase = (Phase == ECyclePhase::Forward || Phase == ECyclePhase::Reverse);
			if (bActivePhase) {
				Interpolate(FTweenFrame{ Alpha, FrameCount });
			}

			if (FrameCount > 0) {
				if (LastPhase == ECyclePhase::Forward && Phase != ECyclePhase::Forward) {
					Interpolate({ EvaluateEase(1.f), FrameCount });
					Callbacks.BroadcastOnForwardEnd();
				}

				if (Phase == ECyclePhase::Reverse && LastPhase != ECyclePhase::Reverse) {
					Callbacks.BroadcastOnReverseBegin();
				}

				if (Phase == ECyclePhase::RepeatDelay && LastPhase != ECyclePhase::RepeatDelay) {
					if (this->Settings.LoopMode == ETweenLoopMode::PingPong && LastPhase == ECyclePhase::Reverse) {
						Interpolate({ EvaluateEase(0.f), FrameCount });
					}
					Callbacks.BroadcastOnCycleEnd();
				}
			}

			// Cycle boundary
			const float CycleTime = GetCycleTime();
			const int32 CycleIdx = CycleTime > 0.f ? FMath::FloorToInt(AnimElapsed / CycleTime) : 0;
			const bool  bNewCycle = (CycleIdx > LastCycleIndex) && (FrameCount > 0);

			if (bNewCycle) {
				if (LastPhase != ECyclePhase::RepeatDelay) {
					const float BoundaryRaw = (this->Settings.LoopMode == ETweenLoopMode::PingPong) ? 0.f : 1.f;
					Interpolate({ EvaluateEase(BoundaryRaw), FrameCount });
					if (this->Settings.LoopMode == ETweenLoopMode::Restart) Callbacks.BroadcastOnForwardEnd();
					Callbacks.BroadcastOnCycleEnd();
				}
				Callbacks.BroadcastOnRepeat();
				Callbacks.BroadcastOnCycleBegin();
				LastCycleIndex = CycleIdx;
			}

			LastPhase = Phase;
			FrameCount++;
		}

		// -------------------------------------------------- self shared_ptr link
		// Set by the subsystem after heap-allocating the tween.
		// Enables conversion to TTweenHandle / FTweenHandle.
		void SetSelfWeak(TWeakPtr<Derived> Ptr) { SelfWeak = Ptr; }
		TSharedPtr<Derived> GetSelfShared() const { return SelfWeak.Pin(); }

		// -------------------------------------------------- handle conversion
		/** Convert to a storable typed handle. Keeps the tween alive. */
		Detail::TypedTweenHandle<Derived, TDefaultSharedPtr> ToHandle() {
			return Detail::TypedTweenHandle<Derived, TDefaultSharedPtr>(GetSelfShared());
		}

		Detail::TypedTweenHandle<Derived, TDefaultWeakPtr> ToWeakHandle() {
			return Detail::TypedTweenHandle<Derived, TDefaultWeakPtr>(TWeakPtr<Derived>(SelfWeak));
		}

		/** Implicit conversion to typed handle (enables assignment syntax). */
		operator Detail::TypedTweenHandle<Derived, TDefaultSharedPtr>() {
			return ToHandle();
		}

		operator Detail::TypedTweenHandle<Derived, TDefaultWeakPtr>() {
			return ToWeakHandle();
		}

		/** Implicit conversion to type-erased handle. */
		operator FTweenHandle() {
			return FTweenHandle(StaticCastSharedPtr<ITweenControl>(GetSelfShared()));
		}

	protected:
		FTweenSettings Settings;
		FTweenCallbacks Callbacks;

	private:
		float Elapsed = 0.f;
		uint64 FrameCount = 0;
		bool bPaused = false;
		bool bKilled = false;
		bool bStartFired = false;
		bool bFinalized = false;
		bool bCompleteFired = false;
		int32 LastCycleIndex = 0;
		ECyclePhase LastPhase = ECyclePhase::Forward;

		TWeakPtr<Derived> SelfWeak;

		void Interpolate(const TypeTween::Detail::FTweenFrame& Frame) {
			Self().Interpolate(Frame);
			Callbacks.BroadcastOnUpdate(Frame.Alpha);
		}

		Derived& Self() { return *static_cast<Derived*>(this); }

		float EvaluateEase(float Alpha) const {
			const ETweenEase Ease = this->Settings.Ease;
			if (Ease == ETweenEase::CustomCurve) {
				const FRuntimeFloatCurve& CustomEase = this->Settings.CustomEaseCurve;
				const FRichCurve* Curve = CustomEase.GetRichCurveConst();
				if (Curve && Curve->GetNumKeys() >= 2) {
					/* Evaluate custom curve */
					return Curve->Eval(Alpha, 0.f);
				}
				/* Fallback to linear if no curve is set */
				return Alpha;
			}
			/* Evaluate built-in easing function */
			return Detail::ApplyEase(Alpha, this->Settings.Ease);
		}

		float GetCycleTime() const {
			const float Base = (this->Settings.LoopMode == ETweenLoopMode::PingPong)
				? 2.f * this->Settings.Duration + this->Settings.Delays.Reverse : this->Settings.Duration;
			return Base + this->Settings.Delays.Repeat;
		}

		float GetTotalAnimTime() const {
			if (this->Settings.RepeatCount < 0) return TNumericLimits<float>::Max();
			return GetCycleTime() * (this->Settings.RepeatCount + 1) - this->Settings.Delays.Repeat;
		}

		float GetMaxElapsed() const {
			return this->Settings.Delays.Start + GetTotalAnimTime() + this->Settings.Delays.End;
		}

		float ComputeProgress(float AnimElapsed, bool& bOutReversing, ECyclePhase& OutPhase) const {
			bOutReversing = false;
			const float CycleTime = GetCycleTime();
			const float CyclePos = CycleTime > 0.f ? FMath::Fmod(AnimElapsed, CycleTime) : 0.f;

			if (this->Settings.LoopMode == ETweenLoopMode::Restart) {
				if (CyclePos >= this->Settings.Duration) { OutPhase = ECyclePhase::RepeatDelay; return 1.f; }
				OutPhase = ECyclePhase::Forward;
				return this->Settings.Duration > 0.f ? CyclePos / this->Settings.Duration : 1.f;
			}

			// PingPong
			if (CyclePos <= this->Settings.Duration) {
				OutPhase = ECyclePhase::Forward;
				return this->Settings.Duration > 0.f ? CyclePos / this->Settings.Duration : 1.f;
			}
			if (CyclePos <= this->Settings.Duration + this->Settings.Delays.Reverse) {
				OutPhase = ECyclePhase::ReverseDelay;
				return 1.f;
			}
			if (CyclePos <= 2.f * this->Settings.Duration + this->Settings.Delays.Reverse) {
				bOutReversing = true;
				OutPhase = ECyclePhase::Reverse;
				const float RevElapsed = CyclePos - this->Settings.Duration - this->Settings.Delays.Reverse;
				return this->Settings.Duration > 0.f ? RevElapsed / this->Settings.Duration : 1.f;
			}
			OutPhase = ECyclePhase::RepeatDelay;
			return 0.f;
		}
	};
}
