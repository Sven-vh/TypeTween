#pragma once
#include "TweenBase.h"
#include "TweenTraits.h"
#include ""
#include "Math/Color.h"
#include "Tools/ColorLerps.h"

enum class ETextLerpMode : uint8 {

};

namespace TypeTween {

	template<>
	class ITween<FText> : public Detail::TweenBase<ITween<FText>> {
	public:
		explicit ITween(FText* InValue) : Value(InValue) {}

		/* Start Value [T=0], if not provided, will use current value */
		ITween& From(FText InStart) { Start = MoveTemp(InStart); return *this; }
		/* End Value [T=1], if not provided, will use current value */
		ITween& To(FText InEnd) { End = MoveTemp(InEnd);   return *this; }

		/* Typed update callback, Receives Alpha [0..1] and current Value */
		ITween& OnUpdate(TFunction<void(float Alpha, const FText& Value)> Fn) {
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
				//*Value = Detail::ColorLerp::Lerp(
				//	Start.GetValue(),
				//	End.GetValue(),
				//	Frame.Alpha,
				//	Mode);
			}

			// Fire typed OnUpdate callback with value
			if (OnUpdateCB && Value) OnUpdateCB(Frame.Alpha, *Value);
		}

		/* Getters for handles */
		FText* GetValue() { return Value; }
		const FText* GetValue() const { return Value; }

		TOptional<FString>& GetStart() { return Start; }
		const TOptional<FString>& GetStart() const { return Start; }
		TOptional<FString>& GetEnd() { return End; }
		const TOptional<FString>& GetEnd() const { return End; }

	private:
		FText* Value = nullptr;
		TOptional<FString> Start;
		TOptional<FString> End;
		TFunction<void(float, const FText&)> OnUpdateCB;
	};
}