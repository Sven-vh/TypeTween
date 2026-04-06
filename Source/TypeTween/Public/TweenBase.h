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
		Derived& StartDelay(float Secs) { Config.StartDelay = Secs; return Self(); }
		/* Total time for one forward or reverse playthrough, not including delays. Required. */
		Derived& Duration(float Secs) { Config.Duration = Secs; return Self(); }
		/* [Ping Pong Only] Delay between forward and reverse play */
		Derived& ReverseDelay(float Secs) { Config.ReverseDelay = Secs; return Self(); }
		/* Delay between the end of one cycle and the start of the next */
		Derived& RepeatDelay(float Secs) { Config.RepeatDelay = Secs; return Self(); }
		/* Delay after the tween finishes before firing OnComplete */
		Derived& EndDelay(float Secs) { Config.EndDelay = Secs;  return Self(); }

		/* 0 = play once, -1 = infinite, N = play N+1 times total */
		Derived& Repeat(int32 Count) { Config.RepeatCount = Count; return Self(); }
		/* Easing function for the tween, see https://easings.net/ for visualization */
		Derived& Ease(ETweenEase E) { Config.Ease = E; return Self(); }
		/* Tween will reverse direction each cycle instead of jumping back to start */
		Derived& PingPong(bool Enable = true) {
			Config.LoopMode = Enable ? ETweenLoopMode::PingPong : ETweenLoopMode::Loop;
			return Self();
		}


		Derived& Preset(FTweenConfig Preset) { Config = Preset.Resolve(); return Self(); }

		// -------------------------------------------------- callbacks
		/* Fired first frame when the tween is triggered, before StartDelay begins */
		Derived& OnPreStart(TFunction<void()> Fn) { OnPreStartCB = MoveTemp(Fn); return Self(); }
		/* (Start Delay...) */
		/* Fired once the tween starts playing (after StartDelay) */
		Derived& OnStart(TFunction<void()> Fn) { OnStartCB = MoveTemp(Fn); return Self(); }
		/* Fired at the start of each new cycle (after StartDelay or RepeatDelay) */
		Derived& OnCycleBegin(TFunction<void()> Fn) { OnCycleBeginCB = MoveTemp(Fn); return Self(); }
		/* (Duration...) */
		/* Fired at the end of the forward phase of each cycle (before ReverseDelay), when t == 1 */
		Derived& OnForwardEnd(TFunction<void()> Fn) { OnForwardEndCB = MoveTemp(Fn); return Self(); }
		/* (Reverse Delay...) */
		/* [Ping Pong Only] Fired at the start of the reverse phase of each cycle (after ReverseDelay) */
		Derived& OnReverseBegin(TFunction<void()> Fn) { OnReverseBeginCB = MoveTemp(Fn); return Self(); }
		/* (Duration...) */
		/* Fired at the end of each cycle (before EndDelay or RepeatDelay), when t == 0 */
		Derived& OnCycleEnd(TFunction<void()> Fn) { OnCycleEndCB = MoveTemp(Fn); return Self(); }
		/* (Repeat Delay...) */
		/* Fired at the end of each cycle except the last one */
		Derived& OnRepeat(TFunction<void()> Fn) { OnRepeatCB = MoveTemp(Fn); return Self(); }
		/* (End Delay...) */
		/* Fired once the tween finishes playing (after EndDelay + repeats) */
		Derived& OnComplete(TFunction<void()> Fn) { OnCompleteCB = MoveTemp(Fn); return Self(); }

		/* Fired every tick, including during delays. Unlike OnUpdate, always fires even when  is not changing */
		Derived& OnTick(TFunction<void()> Fn) { OnTickCB = MoveTemp(Fn); return Self(); }
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
			if (Config.RepeatCount < 0) return false;
			return Elapsed >= GetMaxElapsed();
		}

		// ------------------------------------------------------------------ tick
		void Tick(float DeltaTime) {
			if (bPaused || IsDone()) return;

			if (Elapsed == 0.f && OnPreStartCB) OnPreStartCB();

			Elapsed += DeltaTime;
			if (Config.RepeatCount >= 0)
				Elapsed = FMath::Min(Elapsed, GetMaxElapsed());

			/* Fires every frame, even during delays */
			if (OnTickCB) OnTickCB();

			if (Elapsed < Config.StartDelay) return;

			if (!bStartFired) {
				bStartFired = true;
				if (OnStartCB)      OnStartCB();
				if (OnCycleBeginCB) OnCycleBeginCB();
			}

			const float AnimElapsed = Elapsed - Config.StartDelay;
			const float TotalAnimTime = GetTotalAnimTime();

			if (AnimElapsed >= TotalAnimTime) {
				if (!bFinalized) {
					bFinalized = true;

					const float FinalRaw = (Config.LoopMode == ETweenLoopMode::PingPong) ? 0.f : 1.f;
					const float FinalAlpha = Detail::ApplyEase(FinalRaw, Config.Ease);
					Self().Interpolate(FTweenFrame{ FinalAlpha, FrameCount });
					FrameCount++;
					if (LastPhase == ECyclePhase::Forward && OnForwardEndCB) OnForwardEndCB();
					if (OnCycleEndCB) OnCycleEndCB();
				}

				if (!bCompleteFired && Elapsed >= GetMaxElapsed()) {
					bCompleteFired = true;
					if (OnCompleteCB) OnCompleteCB();
				}
				return;
			}

			bFinalized = false;

			bool bReversing = false;
			ECyclePhase Phase = ECyclePhase::Forward;
			const float RawAlpha = ComputeProgress(AnimElapsed, bReversing, Phase);
			const float Alpha = bReversing
				? 1.f - Detail::ApplyEase(RawAlpha, Config.Ease)
				: Detail::ApplyEase(RawAlpha, Config.Ease);

			const bool bActivePhase = (Phase == ECyclePhase::Forward || Phase == ECyclePhase::Reverse);
			if (bActivePhase) {
				Self().Interpolate(FTweenFrame{ Alpha, FrameCount });
			}

			if (FrameCount > 0) {
				if (LastPhase == ECyclePhase::Forward && Phase != ECyclePhase::Forward) {
					if (OnForwardEndCB) OnForwardEndCB();
				}

				if (Phase == ECyclePhase::Reverse && LastPhase != ECyclePhase::Reverse) {
					if (OnReverseBeginCB) OnReverseBeginCB();
				}

				if (Phase == ECyclePhase::RepeatDelay && LastPhase != ECyclePhase::RepeatDelay) {
					if (OnCycleEndCB) OnCycleEndCB();
				}
			}

			// Cycle boundary
			const float CycleTime = GetCycleTime();
			const int32 CycleIdx = CycleTime > 0.f ? FMath::FloorToInt(AnimElapsed / CycleTime) : 0;
			const bool  bNewCycle = (CycleIdx > LastCycleIndex) && (FrameCount > 0);

			if (bNewCycle) {
				if (LastPhase != ECyclePhase::RepeatDelay) {
					if (Config.LoopMode == ETweenLoopMode::Loop && OnForwardEndCB) OnForwardEndCB();
					if (OnCycleEndCB) OnCycleEndCB();
				}
				if (OnRepeatCB)     OnRepeatCB();
				if (OnCycleBeginCB) OnCycleBeginCB();
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
		FTweenSettings Config;

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

		/* Frame 0 */
		TFunction<void()> OnPreStartCB;
		/* Start Delay... */
		TFunction<void()> OnStartCB;
		TFunction<void()> OnCycleBeginCB;
		/* Duration... */
		TFunction<void()> OnForwardEndCB;
		/* Reverse Delay... */
		TFunction<void()> OnReverseBeginCB; /* [Ping Pong Only] */
		/* Duration... */
		TFunction<void()> OnCycleEndCB;
		/* Repeat Delay... */
		TFunction<void()> OnRepeatCB;
		/* End Delay... */
		TFunction<void()> OnCompleteCB;
		/* Every tick */
		TFunction<void()> OnTickCB;

		Derived& Self() { return *static_cast<Derived*>(this); }

		float GetCycleTime() const {
			const float Base = (Config.LoopMode == ETweenLoopMode::PingPong)
				? 2.f * Config.Duration + Config.ReverseDelay : Config.Duration;
			return Base + Config.RepeatDelay;
		}

		float GetTotalAnimTime() const {
			if (Config.RepeatCount < 0) return TNumericLimits<float>::Max();
			return GetCycleTime() * (Config.RepeatCount + 1) - Config.RepeatDelay;
		}

		float GetMaxElapsed() const {
			return Config.StartDelay + GetTotalAnimTime() + Config.EndDelay;
		}

		float ComputeProgress(float AnimElapsed, bool& bOutReversing, ECyclePhase& OutPhase) const {
			bOutReversing = false;
			const float CycleTime = GetCycleTime();
			const float CyclePos = CycleTime > 0.f ? FMath::Fmod(AnimElapsed, CycleTime) : 0.f;

			if (Config.LoopMode == ETweenLoopMode::Loop) {
				if (CyclePos >= Config.Duration) { OutPhase = ECyclePhase::RepeatDelay; return 1.f; }
				OutPhase = ECyclePhase::Forward;
				return Config.Duration > 0.f ? CyclePos / Config.Duration : 1.f;
			}

			// PingPong
			if (CyclePos <= Config.Duration) {
				OutPhase = ECyclePhase::Forward;
				return Config.Duration > 0.f ? CyclePos / Config.Duration : 1.f;
			}
			if (CyclePos <= Config.Duration + Config.ReverseDelay) {
				OutPhase = ECyclePhase::ReverseDelay;
				return 1.f;
			}
			if (CyclePos <= 2.f * Config.Duration + Config.ReverseDelay) {
				bOutReversing = true;
				OutPhase = ECyclePhase::Reverse;
				const float RevElapsed = CyclePos - Config.Duration - Config.ReverseDelay;
				return Config.Duration > 0.f ? RevElapsed / Config.Duration : 1.f;
			}
			OutPhase = ECyclePhase::RepeatDelay;
			return 0.f;
		}
	};
}