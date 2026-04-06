#pragma once
#include "TweenTypes.h"

namespace TypeTween {

	/**
	 * Abstract interface for tween runtime control.
	 * Enables type-erased handles (FTweenHandle) to control any tween
	 * without knowing its value type.
	 */
	class ITweenControl {
	public:
		virtual ~ITweenControl() = default;

		// ---- Runtime control ----
		
		/** Pause the tween. */
		virtual void Pause() = 0;

		/** Resume a paused tween. */
		virtual void Resume() = 0;

		/** Reset all state and replay from the beginning. */
		virtual void Restart() = 0;

		/** Jump to the end immediately. */
		virtual void Finish() = 0;

		/** Returns true if the tween is paused. */
		virtual bool IsPaused() const = 0;

		/** Returns true if the tween has completed (false for infinite repeats). */
		virtual bool IsDone() const = 0;

		// ---- Type-erased access to settings and callbacks ----
		
		/** Access tween settings (Duration, Ease, RepeatCount, etc.) */
		virtual FTweenSettings& GetSettings() = 0;
		virtual const FTweenSettings& GetSettings() const = 0;

		/** Access callback container (BP delegates + C++ TFunctions) */
		virtual FTweenCallbacks& GetCallbacks() = 0;
		virtual const FTweenCallbacks& GetCallbacks() const = 0;
	};

}
