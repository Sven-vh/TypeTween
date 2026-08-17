// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "TweenBase.h"
#include "TweenTraits.h"
#include "Internationalization/Text.h"
#include "Tools/TextLerps.h"

namespace TypeTween {
	template<>
	class ITween<FText> : public Detail::TweenBase<ITween<FText>> {
	public:
		explicit ITween(FText* InValue) : Value(InValue) {}

		/** Start string [T=0]. If not provided, uses current value. */
		ITween& From(FText InStart) {
			Start = InStart.ToString();
			return *this;
		}

		/** End string [T=1]. If not provided, uses current value. */
		ITween& To(FText InEnd) {
			Waypoints.Add(InEnd.ToString());
			return *this;
		}

		/** Multiple waypoints, interpolates through each in order. */
		ITween& To(TArray<FText> InWaypoints) {
			Waypoints.Reset(InWaypoints.Num());
			for (FText& Text : InWaypoints) {
				Waypoints.Add(Text.ToString());
			}
			return *this;
		}

		/** Interpolation algorithm. Analogous to LerpMode() on FLinearColor. */
		ITween& Mode(ETextLerpMode InMode) {
			LerpMode = InMode;
			return *this;
		}

		/** Character pool used as noise during Scramble mode.
		 *  Optionally pass a custom string when GlyphSet is ETextGlyphSet::Custom. */
		ITween& GlyphSet(ETextGlyphSet InGlyphSet, FString InCustom = FString()) {
			Glyphs = Detail::TextLerp::GetGlyphs(InGlyphSet, InCustom);
			return *this;
		}

		/** Typed update callback. Receives alpha [0..1] and current FText value. */
		ITween& OnUpdate(TFunction<void(float Alpha, const FText& Value)> Fn) {
			OnUpdateCB = MoveTemp(Fn);
			return *this;
		}

		ITween& OnUpdate(TFunction<void(const FText& Value)> Fn) {
			OnUpdateCB = [Fn = MoveTemp(Fn)](float, const FText& NewValue) { Fn(NewValue); };
			return *this;
		}

		// ---- Only contract with TweenBase: called each tick ----
		void Interpolate(const Detail::FTweenFrame& Frame) {
			if (Frame.FrameCount == 0) {
				// Snapshot current value as fallback if From/To were not provided
				if (!Start.IsSet() && Value) Start = Value->ToString();
				if (Waypoints.Num() == 0 && Value) Waypoints.Add(Value->ToString());

				// Pre-compute edit operations once per segment - O(M*N) each, cached for all subsequent ticks
				if (LerpMode == ETextLerpMode::EditDistance && Start.IsSet() && Waypoints.Num() > 0) {
					CachedOpsPerSegment.Reset(Waypoints.Num());
					for (int32 i = 0; i < Waypoints.Num(); ++i) {
						const FString& A = (i == 0) ? Start.GetValue() : Waypoints[i - 1];
						const FString& B = Waypoints[i];
						CachedOpsPerSegment.Add(Detail::TextLerp::ComputeEditOps(A, B));
					}
				}
			}

			if (Value && Start.IsSet() && Waypoints.Num() > 0) {
				const int32 Segments = Waypoints.Num(); // Start->WP0, WP0->WP1, ...
				const float Scaled = FMath::Clamp(Frame.Alpha, 0.0f, 1.0f) * Segments;

				int32 SegIndex = FMath::Clamp(FMath::FloorToInt(Scaled), 0, Segments - 1);
				float LocalAlpha = Scaled - SegIndex;

				// Edge case: Alpha == 1.0 exactly lands on the last point cleanly
				if (SegIndex == Segments - 1 && Scaled >= Segments) LocalAlpha = 1.0f;

				const FString& A = (SegIndex == 0) ? Start.GetValue() : Waypoints[SegIndex - 1];
				const FString& B = Waypoints[SegIndex];

				static const TArray<Detail::TextLerp::FEditOp> EmptyOps;
				const TArray<Detail::TextLerp::FEditOp>& Ops =
					CachedOpsPerSegment.IsValidIndex(SegIndex) ? CachedOpsPerSegment[SegIndex] : EmptyOps;

				const FString Result = Detail::TextLerp::Lerp(
					A,
					B,
					LocalAlpha,
					LerpMode,
					Glyphs,
					Ops);

				*Value = FText::FromString(Result);
			}

			if (OnUpdateCB && Value)
				OnUpdateCB(Frame.Alpha, *Value);
		}

		// ---- Getters for handles ----
		FText* GetValue() { return Value; }
		const FText* GetValue() const { return Value; }

		TOptional<FString>& GetStart() { return Start; }
		const TOptional<FString>& GetStart() const { return Start; }

		TArray<FString>& GetWaypoints() { return Waypoints; }
		const TArray<FString>& GetWaypoints() const { return Waypoints; }

		ETextLerpMode GetMode() const { return LerpMode; }

	private:
		FText* Value = nullptr;

		TOptional<FString> Start;
		TArray<FString> Waypoints;

		ETextLerpMode LerpMode = ETextLerpMode::Scramble;

		FString Glyphs = Detail::TextLerp::GetGlyphs(ETextGlyphSet::Alphanumeric, FString());

		TFunction<void(float, const FText&)> OnUpdateCB;
		TArray<TArray<Detail::TextLerp::FEditOp>> CachedOpsPerSegment; // only populated for EditDistance mode, one entry per segment
	};

} // namespace TypeTween
