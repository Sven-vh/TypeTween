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
		ITween& To(FLinearColor InEnd) { End = MoveTemp(InEnd);   return *this; }
		/* Relative Value, adds to Start. If Start not provided, uses current value as Start */
		ITween& By(FLinearColor InDelta) {
			if (!Start.IsSet() && Value)
				Start = *Value; End = Start.GetValue() + MoveTemp(InDelta);
			return *this;
		}

		ITween& LerpMode(EColorLerpMode InMode) { Mode = InMode;   return *this; }

		/* Typed update callback, Receives Alpha [0..1] and current Value */
		ITween& OnUpdate(TFunction<void(float Alpha, const FLinearColor& Value)> Fn) {
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
				*Value = Detail::ColorLerp::Lerp(
					Start.GetValue(),
					End.GetValue(),
					Frame.Alpha,
					Mode);
			}

			// Fire typed OnUpdate callback with value
			if (OnUpdateCB && Value) OnUpdateCB(Frame.Alpha, *Value);
		}

		/* Getters for handles */
		FLinearColor* GetValue() { return Value; }
		const FLinearColor* GetValue() const { return Value; }

		TOptional<FLinearColor>& GetStart() { return Start; }
		const TOptional<FLinearColor>& GetStart() const { return Start; }

		TOptional<FLinearColor>& GetEnd() { return End; }
		const TOptional<FLinearColor>& GetEnd() const { return End; }

		EColorLerpMode GetColorMode() const { return Mode; }

	private:
		FLinearColor* Value = nullptr;
		TOptional<FLinearColor> Start;
		TOptional<FLinearColor> End;
		EColorLerpMode Mode = EColorLerpMode::Linear;
		TFunction<void(float, const FLinearColor&)> OnUpdateCB;
	};
}