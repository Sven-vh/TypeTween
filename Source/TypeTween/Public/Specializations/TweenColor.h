// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "TweenBase.h"
#include "TweenTraits.h"
#include "Math/Color.h"
#include "Tools/ColorLerps.h"

namespace TypeTween {

	template<>
	class ITween<FLinearColor> : public Detail::TweenBase<ITween<FLinearColor>> {
	public:
		explicit ITween(FLinearColor* InValue) : Value(InValue) {}

		/* Start Value [T=0], if not provided, will use current value */
		ITween& From(FLinearColor InStart) { Start = MoveTemp(InStart); return *this; }
		/* End Value [T=1], if not provided, will use current value */
		ITween& To(FLinearColor InEnd) { Waypoints.Add(MoveTemp(InEnd)); return *this; }
		/* Multiple waypoints, interpolates through each in order */
		ITween& To(TArray<FLinearColor> InWaypoints) { Waypoints = MoveTemp(InWaypoints); return *this; }
		/* Relative Value, adds to Start. If Start not provided, uses current value as Start */
		ITween& By(FLinearColor InDelta) {
			FLinearColor Base;
			if (Waypoints.Num() > 0)	Base = Waypoints.Last();
			else if (Start.IsSet())		Base = Start.GetValue();
			else if (Value)				Base = *Value;

			Waypoints.Add(Base + MoveTemp(InDelta));
			return *this;
		}

		ITween& ColorSpace(EColorLerpMode InMode) { Mode = InMode;   return *this; }

		/* Typed update callback, Receives Alpha [0..1] and current Value */
		ITween& OnUpdate(TFunction<void(float Alpha, const FLinearColor& Value)> Fn) {
			OnUpdateCB = MoveTemp(Fn); return *this;
		}

		ITween& OnUpdate(TFunction<void(const FLinearColor& Value)> Fn) {
			OnUpdateCB = [Fn = MoveTemp(Fn)](float, const FLinearColor& NewValue) { Fn(NewValue); };
			return *this;
		}

		// ---- Only contract with TweenBase: called each tick ----
		void Interpolate(const Detail::FTweenFrame& Frame) {
			if (Frame.FrameCount == 0) {
				/* If no start or waypoints provided, use current value */
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

				const FLinearColor& A = (SegIndex == 0) ? Start.GetValue() : Waypoints[SegIndex - 1];
				const FLinearColor& B = Waypoints[SegIndex];

				*Value = Detail::ColorLerp::Lerp(A, B, LocalAlpha, Mode);
			}

			// Fire typed OnUpdate callback with value
			if (OnUpdateCB && Value) OnUpdateCB(Frame.Alpha, *Value);
		}

		/* Getters for handles */
		FLinearColor* GetValue() { return Value; }
		const FLinearColor* GetValue() const { return Value; }

		TOptional<FLinearColor>& GetStart() { return Start; }
		const TOptional<FLinearColor>& GetStart() const { return Start; }

		TArray<FLinearColor>& GetWaypoints() { return Waypoints; }
		const TArray<FLinearColor>& GetWaypoints() const { return Waypoints; }

		EColorLerpMode GetColorMode() const { return Mode; }

	private:
		FLinearColor* Value = nullptr;
		TOptional<FLinearColor> Start;
		TArray<FLinearColor> Waypoints;
		EColorLerpMode Mode = EColorLerpMode::Linear;
		TFunction<void(float, const FLinearColor&)> OnUpdateCB;
	};
}
