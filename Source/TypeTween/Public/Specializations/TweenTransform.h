#pragma once
#include "TweenBase.h"

namespace TypeTween {

	enum class EComponent : uint8 {
		None = 0,
		Translation = 1 << 0,
		Rotation = 1 << 1,
		Scale = 1 << 2,
		All = Translation | Rotation | Scale
	};
	ENUM_CLASS_FLAGS(EComponent);

	template<>
	class ITween<FTransform> : public Detail::TweenBase<ITween<FTransform>> {
	public:
		explicit ITween(FTransform* InValue) : Value(InValue) {}

		ITween& From(FTransform InStart) {
			Start = MoveTemp(InStart);
			ComponentsToTween = EComponent::All;
			return *this;
		}
		ITween& To(FTransform InEnd) {
			End = MoveTemp(InEnd);
			ComponentsToTween = EComponent::All;
			return *this;
		}

		ITween& By(FTransform InDelta) {
			DefaultStart();
			End = Start.GetValue() + MoveTemp(InDelta);
			ComponentsToTween = EComponent::All;
			return *this;
		}

		ITween& MoveFrom(const FVector& InStart) {
			DefaultStart();
			Start->SetTranslation(InStart);
			ComponentsToTween |= EComponent::Translation;
			return *this;
		}
		ITween& MoveTo(const FVector& InEnd) {
			DefaultEnd();
			End = *Start;
			End->SetTranslation(InEnd);
			ComponentsToTween |= EComponent::Translation;
			return *this;
		}

		ITween& RotateFrom(const FQuat& InStart) {
			DefaultStart();
			Start->SetRotation(InStart);
			ComponentsToTween |= EComponent::Rotation;
			return *this;
		}
		ITween& RotateTo(const FQuat& InEnd) {
			DefaultEnd();
			End = *Start;
			End->SetRotation(InEnd);
			ComponentsToTween |= EComponent::Rotation;
			return *this;
		}

		ITween& ScaleFrom(const FVector& InStart) {
			DefaultStart();
			Start->SetScale3D(InStart);
			ComponentsToTween |= EComponent::Scale;
			return *this;
		}
		ITween& ScaleTo(const FVector& InEnd) {
			DefaultEnd();
			End = *Start;
			End->SetScale3D(InEnd);
			ComponentsToTween |= EComponent::Scale;
			return *this;
		}

		ITween& SetComponents(EComponent InComponents) {
			ComponentsToTween = InComponents;
			return *this;
		}

		ITween& DontMove() { ComponentsToTween &= ~EComponent::Translation; return *this; }
		ITween& DontRotate() { ComponentsToTween &= ~EComponent::Rotation; return *this; }
		ITween& DontScale() { ComponentsToTween &= ~EComponent::Scale; return *this; }

		/* Typed update callback, Receives Alpha [0..1] and current Value */
		ITween& OnUpdate(TFunction<void(float Alpha, const FTransform& Value)> Fn) {
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
				const FTransform& A = Start.GetValue();
				const FTransform& B = End.GetValue();
				FTransform Result = *Value;
				if (EnumHasAnyFlags(ComponentsToTween, EComponent::Translation))
					Result.SetTranslation(Lerp(A.GetTranslation(), B.GetTranslation(), Frame.Alpha));
				if (EnumHasAnyFlags(ComponentsToTween, EComponent::Rotation))
					Result.SetRotation(FQuat::Slerp(A.GetRotation(), B.GetRotation(), Frame.Alpha));
				if (EnumHasAnyFlags(ComponentsToTween, EComponent::Scale))
					Result.SetScale3D(Lerp(A.GetScale3D(), B.GetScale3D(), Frame.Alpha));
				*Value = Result;
			}
			// Fire typed OnUpdate callback with value
			if (OnUpdateCB && Value) OnUpdateCB(Frame.Alpha, *Value);
		}


	private:
		FTransform* Value = nullptr;

		TOptional<FTransform> Start;
		TOptional<FTransform> End;

		EComponent ComponentsToTween = EComponent::None;

		TFunction<void(float, const FTransform&)> OnUpdateCB;

		void DefaultStart() {
			if (!Start.IsSet()) {
				if (Value)
					Start = *Value;
				else
					Start = FTransform::Identity;
			}
		}

		void DefaultEnd() {
			if (!End.IsSet()) {
				if (Value)
					End = *Value;
				else
					End = FTransform::Identity;
			}
		}
	};
}