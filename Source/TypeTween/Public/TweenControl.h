#pragma once

namespace TypeTween {

	/**
	 * Abstract interface for tween runtime control.
	 * Enables type-erased handles (FTweenHandle) to control any tween
	 * without knowing its value type.
	 */
	class ITweenControl {
	public:
		virtual ~ITweenControl() = default;

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
	};

}
