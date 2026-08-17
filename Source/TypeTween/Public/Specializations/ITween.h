// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "TweenBase.h"
#include "TweenTraits.h"

namespace TypeTween {

	// -----------------------------------------------------------------------
	// Default specialization - works for any IsLerpable type:
	//   float, double, FVector, FVector2D, FRotator, FQuat, FColor, etc.
	// -----------------------------------------------------------------------
	template<typename T>
		requires Traits::THasLerper<T>
	class ITween<T> : public Detail::TweenBase<ITween<T>> {
	public:
		explicit ITween(T* InValue) : Value(InValue) {}

		/* Start Value [T=0], if not provided, will use current value */
		ITween& From(T InStart) { Start = MoveTemp(InStart); return *this; }
		/* End Value [T=1], if not provided, will use current value */
		ITween& To(T InEnd) { Waypoints.Add(MoveTemp(InEnd)); return *this; }
		/* Multiple waypoints, interpolates through each in order */
		ITween& To(TArray<T> InWaypoints) { Waypoints = MoveTemp(InWaypoints); return *this; }
		/* Relative Value, adds to Start. If Start not provided, uses current value as Start */
		ITween& By(T InDelta) {
			T Base;
			if (Waypoints.Num() > 0)	Base = Waypoints.Last();
			else if (Start.IsSet())		Base = Start.GetValue();
			else if (Value)				Base = *Value;

			Waypoints.Add(Base + MoveTemp(InDelta));
			return *this;
		}

		/* Typed update callback, Receives Alpha [0..1] and current Value */
		ITween& OnUpdate(TFunction<void(float Alpha, const T& Value)> Fn) {
			OnUpdateCB = MoveTemp(Fn); return *this;
		}

		ITween& OnUpdate(TFunction<void(const T& Value)> Fn) {
			OnUpdateCB = [Fn = MoveTemp(Fn)](float, const T& NewValue) { Fn(NewValue); };
			return *this;
		}

		// ---- Only contract with TweenBase: called each tick ----
		void Interpolate(const Detail::FTweenFrame& Frame) {
			if (Frame.FrameCount == 0) {
				if (!Start.IsSet() && Value) Start = *Value;
				if (Waypoints.Num() == 0 && Value) Waypoints.Add(*Value);
			}

			if (Value && Start.IsSet() && Waypoints.Num() > 0) {
				const int32 Segments = Waypoints.Num(); // Start->WP0, WP0->WP1, ...
				const float Scaled = FMath::Clamp(Frame.Alpha, 0.0f, 1.0f) * Segments;

				int32 SegIndex = FMath::Clamp(FMath::FloorToInt(Scaled), 0, Segments - 1);
				float LocalAlpha = Scaled - SegIndex;

				// Edge case: Alpha == 1.0 exactly lands on the last point cleanly
				if (SegIndex == Segments - 1 && Scaled >= Segments) LocalAlpha = 1.0f;

				const T& A = (SegIndex == 0) ? Start.GetValue() : Waypoints[SegIndex - 1];
				const T& B = Waypoints[SegIndex];

				*Value = Lerp(A, B, LocalAlpha);
			}

			if (OnUpdateCB && Value) OnUpdateCB(Frame.Alpha, *Value);
		}

		/* Getters for handles */
		T* GetValue() { return Value; }
		const T* GetValue() const { return Value; }

		TOptional<T>& GetStart() { return Start; }
		const TOptional<T>& GetStart() const { return Start; }

		TArray<T>& GetWaypoints() { return Waypoints; }
		const TArray<T>& GetWaypoints() const { return Waypoints; }

	private:
		T* Value = nullptr;
		TOptional<T> Start;
		TArray<T> Waypoints;
		TFunction<void(float, const T&)> OnUpdateCB;
	};


	// -----------------------------------------------------------------------
	// Owning variant, the tween stores its own T internally.
	// Used by Tween<T>(WorldContext) and Tween<T>(InitialValue, WorldContext).
	// -----------------------------------------------------------------------
	template<typename T>
		requires Traits::THasITween<T>
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

	template<>
	class ITween<void> : public Detail::TweenBase<ITween<void>> {
	public:
		ITween() = default;

		ITween& OnUpdate(TFunction<void(float)> Fn) {
			OnUpdateCB = MoveTemp(Fn);
			return *this;
		}

		ITween& OnUpdate(TFunction<void()> Fn) {
			OnUpdateCB = [Fn = MoveTemp(Fn)](float) { Fn(); };
			return *this;
		}

		// Contract with TweenBase
		void Interpolate(const Detail::FTweenFrame& Frame) {
			// Fire typed callback
			if (OnUpdateCB) OnUpdateCB(Frame.Alpha);
		}

	private:
		TFunction<void(float)> OnUpdateCB;
	};
}

