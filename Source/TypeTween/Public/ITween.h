#pragma once
#include "TweenBase.h"

namespace TypeTween {

	// Concept: T supports  A + (B - A) * float
	template<typename T>
	concept IsLerpable = requires(T A, T B, float F) {
		{ A + (B - A) * F } -> std::convertible_to<T>;
	};

	/* Forward Declaration */
	template<typename T, typename... Args>
	class ITween;

	// -----------------------------------------------------------------------
	// Default specialization — works for any IsLerpable type:
	//   float, double, FVector, FVector2D, FRotator, FQuat, FColor, etc.
	// -----------------------------------------------------------------------
	template<typename T>
		requires IsLerpable<T>
	class ITween<T> : public Detail::TweenBase<ITween<T>> {
	public:
		explicit ITween(T* InValue) : Value(InValue) {}

		/* Start Value [T=0], if not provided, will use current value */
		ITween& From(T InStart) { Start = MoveTemp(InStart); return *this; }
		/* End Value [T=1], if not provided, will use current value */
		ITween& To(T InEnd) { End = MoveTemp(InEnd);   return *this; }
		/* Relative Value, adds to Start. If Start not provided, uses current value as Start */
		ITween& By(T InDelta) {
			if (!Start.IsSet() && Value)
				Start = *Value; End = Start.GetValue() + MoveTemp(InDelta);
			return *this;
		}

		/* Typed update callback, Receives Alpha [0..1] and current Value */
		ITween& OnUpdate(TFunction<void(float Alpha, const T& Value)> Fn) {
			OnUpdateCB = MoveTemp(Fn); return *this;
		}

		// ---- Only contract with TweenBase: called each tick ----
		void Interpolate(const Detail::FTweenFrame& Frame) {
			if (Frame.FrameCount == 0) {
				/* If no start or end provided, use current value */
				if (!Start.IsSet() && Value) Start = *Value;
				if (!End.IsSet() && Value) End = *Value;
			}

			if (Value && Start.IsSet() && End.IsSet()) {
				const T& A = Start.GetValue();
				const T& B = End.GetValue();
				/* Default interpolation */
				*Value = A + (B - A) * Frame.Alpha;
			}

			if (OnUpdateCB && Value) OnUpdateCB(Frame.Alpha, *Value);
		}

		/* Getters for handles */
		T* GetValue() { return Value; }
		const T* GetValue() const { return Value; }

		TOptional<T>& GetStart() { return Start; }
		const TOptional<T>& GetStart() const { return Start; }

		TOptional<T>& GetEnd() { return End; }
		const TOptional<T>& GetEnd() const { return End; }

	private:
		T* Value = nullptr;
		TOptional<T> Start;
		TOptional<T> End;
		TFunction<void(float, const T&)> OnUpdateCB;
	};

	template<>
	class ITween<void> : public Detail::TweenBase<ITween<void>> {
	public:
		ITween() = default;

		ITween& OnUpdate(TFunction<void(float)> Fn) {
			OnUpdateCB = MoveTemp(Fn);
			return *this;
		}

		// Contract with TweenBase
		void Interpolate(const Detail::FTweenFrame& Frame) {
			if (OnUpdateCB) OnUpdateCB(Frame.Alpha);
		}

	private:
		TFunction<void(float)> OnUpdateCB;
	};


	// -----------------------------------------------------------------------
	// Owning variant, the tween stores its own T internally.
	// Used by Tween<T>(WorldContext) and Tween<T>(InitialValue, WorldContext).
	// -----------------------------------------------------------------------
	template<typename T>
		requires IsLerpable<T>
	class ITweenOwned : public ITween<T> {
	public:
		// No initial value, T is default-constructed
		ITweenOwned() : ITween<T>(&InternalValue) {
		}

		// Optional starting value passed via the free-function overload
		explicit ITweenOwned(T InitialValue) : ITween<T>(&InternalValue), InternalValue(MoveTemp(InitialValue)) {
		}

	private:
		T InternalValue{};
	};
}