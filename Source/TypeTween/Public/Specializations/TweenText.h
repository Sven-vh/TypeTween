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
			End = InEnd.ToString();
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

		// ---- Only contract with TweenBase: called each tick ----
		void Interpolate(const Detail::FTweenFrame& Frame) {
			if (Frame.FrameCount == 0) {
				// Snapshot current value as fallback if From/To were not provided
				if (!Start.IsSet() && Value) Start = Value->ToString();
				if (!End.IsSet() && Value) End = Value->ToString();

				// Pre-compute edit operations once — O(M*N), cached for all subsequent ticks
				if (LerpMode == ETextLerpMode::EditDistance && Start.IsSet() && End.IsSet())
					CachedOps = Detail::TextLerp::ComputeEditOps(Start.GetValue(), End.GetValue());
			}

			if (Value && Start.IsSet() && End.IsSet()) {
				const FString Result = Detail::TextLerp::Lerp(
					Start.GetValue(),
					End.GetValue(),
					Frame.Alpha,
					LerpMode,
					Glyphs,
					CachedOps);

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

		TOptional<FString>& GetEnd() { return End; }
		const TOptional<FString>& GetEnd() const { return End; }

		ETextLerpMode GetMode() const { return LerpMode; }

	private:
		FText* Value = nullptr;

		TOptional<FString> Start;
		TOptional<FString> End;

		ETextLerpMode LerpMode = ETextLerpMode::Scramble;

		FString Glyphs = Detail::TextLerp::GetGlyphs(ETextGlyphSet::Alphanumeric, FString());

		TFunction<void(float, const FText&)> OnUpdateCB;
		TArray<Detail::TextLerp::FEditOp> CachedOps; // only populated for EditDistance mode
	};

} // namespace TypeTween