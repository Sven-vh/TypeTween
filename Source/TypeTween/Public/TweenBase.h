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
		Derived& StartDelay(float Secs) { Settings.StartDelay = Secs; return Self(); }
		/* Total time for one forward or reverse playthrough, not including delays. Required. */
		Derived& Duration(float Secs) { Settings.Duration = Secs; return Self(); }
		/* [Ping Pong Only] Delay between forward and reverse play */
		Derived& ReverseDelay(float Secs) { Settings.ReverseDelay = Secs; return Self(); }
		/* Delay between the end of one cycle and the start of the next */
		Derived& RepeatDelay(float Secs) { Settings.RepeatDelay = Secs; return Self(); }
		/* Delay after the tween finishes before firing OnComplete */
		Derived& EndDelay(float Secs) { Settings.EndDelay = Secs;  return Self(); }

		/* 0 = play once, -1 = infinite, N = play N+1 times total */
		Derived& Repeat(int32 Count) { Settings.RepeatCount = Count; return Self(); }
		/* Easing function for the tween, see https://easings.net/ for visualization */
		Derived& Ease(ETweenEase E) { Settings.Ease = E; return Self(); }
		/* Tween will reverse direction each cycle instead of jumping back to start */
		Derived& PingPong(bool Enable = true) {
			Settings.LoopMode = Enable ? ETweenLoopMode::PingPong : ETweenLoopMode::Restart;
			return Self();
		}

		Derived& Preset(FTweenConfig Preset) { Settings = Preset.Resolve(); return Self(); }
		Derived& Preset(FTweenSettings Preset) { Settings = MoveTemp(Preset); return Self(); }

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
			if (Settings.RepeatCount < 0) return false;
			return Elapsed >= GetMaxElapsed();
		}

		// -------------------------------------------------- type-erased access (ITweenControl)
		FTweenSettings& GetSettings() override { return Settings; }
		const FTweenSettings& GetSettings() const override { return Settings; }

		FTweenCallbacks& GetCallbacks() override { return Callbacks; }
		const FTweenCallbacks& GetCallbacks() const override { return Callbacks; }

		// ------------------------------------------------------------------ tick
		void Tick(float DeltaTime) {
			if (bPaused || IsDone()) return;

			if (Elapsed == 0.f) Callbacks.BroadcastOnPreStart();

			Elapsed += DeltaTime;
			if (Settings.RepeatCount >= 0)
				Elapsed = FMath::Min(Elapsed, GetMaxElapsed());

			/* Fires every frame, even during delays */
			Callbacks.BroadcastOnTick();

			if (Elapsed < Settings.StartDelay) return;

			if (!bStartFired) {
				bStartFired = true;
				Callbacks.BroadcastOnStart();
				Callbacks.BroadcastOnCycleBegin();
			}

			const float AnimElapsed = Elapsed - Settings.StartDelay;
			const float TotalAnimTime = GetTotalAnimTime();

			if (AnimElapsed >= TotalAnimTime) {
				if (!bFinalized) {
					bFinalized = true;

					const float FinalRaw = (Settings.LoopMode == ETweenLoopMode::PingPong) ? 0.f : 1.f;
					const float FinalAlpha = Detail::ApplyEase(FinalRaw, Settings.Ease);
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
				? 1.f - Detail::ApplyEase(RawAlpha, Settings.Ease)
				: Detail::ApplyEase(RawAlpha, Settings.Ease);

			const bool bActivePhase = (Phase == ECyclePhase::Forward || Phase == ECyclePhase::Reverse);
			if (bActivePhase) {
				Interpolate(FTweenFrame{ Alpha, FrameCount });
			}

			if (FrameCount > 0) {
				if (LastPhase == ECyclePhase::Forward && Phase != ECyclePhase::Forward) {
					Callbacks.BroadcastOnForwardEnd();
				}

				if (Phase == ECyclePhase::Reverse && LastPhase != ECyclePhase::Reverse) {
					Callbacks.BroadcastOnReverseBegin();
				}

				if (Phase == ECyclePhase::RepeatDelay && LastPhase != ECyclePhase::RepeatDelay) {
					Callbacks.BroadcastOnCycleEnd();
				}
			}

			// Cycle boundary
			const float CycleTime = GetCycleTime();
			const int32 CycleIdx = CycleTime > 0.f ? FMath::FloorToInt(AnimElapsed / CycleTime) : 0;
			const bool  bNewCycle = (CycleIdx > LastCycleIndex) && (FrameCount > 0);

			if (bNewCycle) {
				if (LastPhase != ECyclePhase::RepeatDelay) {
					if (Settings.LoopMode == ETweenLoopMode::Restart) Callbacks.BroadcastOnForwardEnd();
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
		TTweenHandle<Derived> ToHandle() {
			return TTweenHandle<Derived>(GetSelfShared());
		}

		/** Implicit conversion to typed handle (enables assignment syntax). */
		operator TTweenHandle<Derived>() {
			return ToHandle();
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

		float GetCycleTime() const {
			const float Base = (Settings.LoopMode == ETweenLoopMode::PingPong)
				? 2.f * Settings.Duration + Settings.ReverseDelay : Settings.Duration;
			return Base + Settings.RepeatDelay;
		}

		float GetTotalAnimTime() const {
			if (Settings.RepeatCount < 0) return TNumericLimits<float>::Max();
			return GetCycleTime() * (Settings.RepeatCount + 1) - Settings.RepeatDelay;
		}

		float GetMaxElapsed() const {
			return Settings.StartDelay + GetTotalAnimTime() + Settings.EndDelay;
		}

		float ComputeProgress(float AnimElapsed, bool& bOutReversing, ECyclePhase& OutPhase) const {
			bOutReversing = false;
			const float CycleTime = GetCycleTime();
			const float CyclePos = CycleTime > 0.f ? FMath::Fmod(AnimElapsed, CycleTime) : 0.f;

			if (Settings.LoopMode == ETweenLoopMode::Restart) {
				if (CyclePos >= Settings.Duration) { OutPhase = ECyclePhase::RepeatDelay; return 1.f; }
				OutPhase = ECyclePhase::Forward;
				return Settings.Duration > 0.f ? CyclePos / Settings.Duration : 1.f;
			}

			// PingPong
			if (CyclePos <= Settings.Duration) {
				OutPhase = ECyclePhase::Forward;
				return Settings.Duration > 0.f ? CyclePos / Settings.Duration : 1.f;
			}
			if (CyclePos <= Settings.Duration + Settings.ReverseDelay) {
				OutPhase = ECyclePhase::ReverseDelay;
				return 1.f;
			}
			if (CyclePos <= 2.f * Settings.Duration + Settings.ReverseDelay) {
				bOutReversing = true;
				OutPhase = ECyclePhase::Reverse;
				const float RevElapsed = CyclePos - Settings.Duration - Settings.ReverseDelay;
				return Settings.Duration > 0.f ? RevElapsed / Settings.Duration : 1.f;
			}
			OutPhase = ECyclePhase::RepeatDelay;
			return 0.f;
		}
	};
}