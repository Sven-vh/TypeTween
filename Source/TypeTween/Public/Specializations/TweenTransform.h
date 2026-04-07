#pragma once
#include "TweenBase.h"
#include "GameFramework/Actor.h"

namespace TypeTween {

	enum class EComponent : uint8 {
		None = 0,
		Translation = 1 << 0,
		Rotation = 1 << 1,
		Scale = 1 << 2,
		All = Translation | Rotation | Scale
	};
	ENUM_CLASS_FLAGS(EComponent);

	/* Mixin that provides transform tweening logic. Used by both ITween<FTransform> and ITween<AActor>*/
	template<typename Derived>
	class TTransformTweenMixin {
	public:
		Derived& From(FTransform InStart) {
			Start = MoveTemp(InStart);
			ComponentsToTween = EComponent::All;
			return static_cast<Derived&>(*this);
		}
		Derived& To(FTransform InEnd) {
			End = MoveTemp(InEnd);
			ComponentsToTween = EComponent::All;
			return static_cast<Derived&>(*this);
		}

		Derived& By(FTransform InDelta) {
			DefaultStart();
			End = Start.GetValue() + MoveTemp(InDelta);
			ComponentsToTween = EComponent::All;
			return static_cast<Derived&>(*this);
		}

		/* Translation */
		Derived& MoveFrom(const FVector& InStart) {
			DefaultStart();
			Start->SetTranslation(InStart);
			ComponentsToTween |= EComponent::Translation;
			return static_cast<Derived&>(*this);
		}
		Derived& MoveTo(const FVector& InEnd) {
			DefaultEnd();
			End->SetTranslation(InEnd);
			ComponentsToTween |= EComponent::Translation;
			return static_cast<Derived&>(*this);
		}

		/* Quat */
		Derived& RotateFrom(const FQuat& InStart) {
			DefaultStart();
			Start->SetRotation(InStart);
			ComponentsToTween |= EComponent::Rotation;
			return static_cast<Derived&>(*this);
		}
		Derived& RotateTo(const FQuat& InEnd) {
			DefaultEnd();
			End->SetRotation(InEnd);
			ComponentsToTween |= EComponent::Rotation;
			return static_cast<Derived&>(*this);
		}

		/* Rotator */
		Derived& RotateFrom(const FRotator& InStart) {
			DefaultStart();
			Start->SetRotation(FQuat(InStart));
			ComponentsToTween |= EComponent::Rotation;
			return static_cast<Derived&>(*this);
		}
		Derived& RotateTo(const FRotator& InEnd) {
			DefaultEnd();
			End->SetRotation(FQuat(InEnd));
			ComponentsToTween |= EComponent::Rotation;
			return static_cast<Derived&>(*this);
		}

		/* Scale */
		Derived& ScaleFrom(const FVector& InStart) {
			DefaultStart();
			Start->SetScale3D(InStart);
			ComponentsToTween |= EComponent::Scale;
			return static_cast<Derived&>(*this);
		}
		Derived& ScaleTo(const FVector& InEnd) {
			DefaultEnd();
			End->SetScale3D(InEnd);
			ComponentsToTween |= EComponent::Scale;
			return static_cast<Derived&>(*this);
		}

		Derived& SetComponents(EComponent InComponents) {
			ComponentsToTween = InComponents;
			return static_cast<Derived&>(*this);
		}

		Derived& DontMove() { ComponentsToTween &= ~EComponent::Translation; return static_cast<Derived&>(*this); }
		Derived& DontRotate() { ComponentsToTween &= ~EComponent::Rotation; return static_cast<Derived&>(*this); }
		Derived& DontScale() { ComponentsToTween &= ~EComponent::Scale; return static_cast<Derived&>(*this); }

	protected:
		TOptional<FTransform> Start;
		TOptional<FTransform> End;
		EComponent ComponentsToTween = EComponent::None;

		/** Must be implemented by derived class to provide current transform value */
		virtual FTransform GetCurrentTransform() const = 0;

		void DefaultStart() {
			if (!Start.IsSet()) {
				Start = GetCurrentTransform();
			}
		}

		void DefaultEnd() {
			if (!End.IsSet()) {
				End = GetCurrentTransform();
			}
		}

		/** Computes the interpolated transform for the current frame without applying it */
		FTransform ComputeInterpolatedTransform(const FTransform& Current, float Alpha) const {
			FTransform Result = Current;
			if (Start.IsSet() && End.IsSet()) {
				const FTransform& A = Start.GetValue();
				const FTransform& B = End.GetValue();
				if (EnumHasAnyFlags(ComponentsToTween, EComponent::Translation))
					Result.SetTranslation(Lerp(A.GetTranslation(), B.GetTranslation(), Alpha));
				if (EnumHasAnyFlags(ComponentsToTween, EComponent::Rotation))
					Result.SetRotation(FQuat::Slerp(A.GetRotation(), B.GetRotation(), Alpha));
				if (EnumHasAnyFlags(ComponentsToTween, EComponent::Scale))
					Result.SetScale3D(Lerp(A.GetScale3D(), B.GetScale3D(), Alpha));
			}
			return Result;
		}

		/** Initializes Start/End from current transform if not set. Call on first frame. */
		void InitializeDefaults() {
			const FTransform Current = GetCurrentTransform();
			if (!Start.IsSet()) Start = Current;
			if (!End.IsSet()) End = Current;
		}
	};

	/* Tweens a pointer to an FTransform value */
	template<>
	class ITween<FTransform> : public Detail::TweenBase<ITween<FTransform>>, public TTransformTweenMixin<ITween<FTransform>> {
		using Mixin = TTransformTweenMixin<ITween<FTransform>>;
	public:
		explicit ITween(FTransform* InValue) : Value(InValue) {}

		/* Typed update callback, Receives Alpha [0..1] and current Value */
		ITween& OnUpdate(TFunction<void(float Alpha, const FTransform& Value)> Fn) {
			OnUpdateCB = MoveTemp(Fn); return *this;
		}

		// ---- Only contract with TweenBase: called each tick ----
		void Interpolate(const Detail::FTweenFrame& Frame) {
			if (Frame.FrameCount == 0) {
				Mixin::InitializeDefaults();
			}
			if (Value && Mixin::Start.IsSet() && Mixin::End.IsSet()) {
				*Value = Mixin::ComputeInterpolatedTransform(*Value, Frame.Alpha);
			}
			// Fire typed OnUpdate callback with value
			if (OnUpdateCB && Value) OnUpdateCB(Frame.Alpha, *Value);
		}

	protected:
		FTransform GetCurrentTransform() const override {
			return Value ? *Value : FTransform::Identity;
		}

	private:
		FTransform* Value = nullptr;
		TFunction<void(float, const FTransform&)> OnUpdateCB;
	};

	/* Tweens an Actor's transform with support for teleport/sweep modes */
	template<>
	class ITween<AActor> : public Detail::TweenBase<ITween<AActor>>, public TTransformTweenMixin<ITween<AActor>> {
		using Mixin = TTransformTweenMixin<ITween<AActor>>;
	public:
		explicit ITween(AActor* InActor) : Actor(InActor) {
			// Initialize Start from actor's current transform
			if (Actor) {
				Mixin::Start = Actor->GetActorTransform();
			}
		}

		/** Enable teleport mode (no physics, no sweep) */
		ITween& Teleport(bool bEnable = true) {
			bTeleport = bEnable;
			if (bEnable) bSweep = false;
			return *this;
		}

		/** Enable sweep mode (collision detection during movement) */
		ITween& Sweep(bool bEnable = true) {
			bSweep = bEnable;
			if (bEnable) bTeleport = false;
			return *this;
		}

		/** Set the hit result output for sweep mode */
		ITween& SweepHitResult(FHitResult* OutHitResult) {
			HitResult = OutHitResult;
			return *this;
		}

		/** Typed update callback for Actor tweens */
		ITween& OnUpdate(TFunction<void(float Alpha, AActor* Actor)> Fn) {
			OnActorUpdateCB = MoveTemp(Fn);
			return *this;
		}

		/** Typed update callback with transform */
		ITween& OnUpdate(TFunction<void(float Alpha, const FTransform& Transform)> Fn) {
			OnTransformUpdateCB = MoveTemp(Fn);
			return *this;
		}

		// ---- Only contract with TweenBase: called each tick ----
		void Interpolate(const Detail::FTweenFrame& Frame) {
			if (!Actor) return;

			if (Frame.FrameCount == 0) {
				Mixin::InitializeDefaults();
			}

			if (Mixin::Start.IsSet() && Mixin::End.IsSet()) {
				const FTransform NewTransform = Mixin::ComputeInterpolatedTransform(Actor->GetActorTransform(), Frame.Alpha);

				// Apply to actor with appropriate method
				if (bSweep) {
					Actor->SetActorTransform(NewTransform, bSweep, HitResult, ETeleportType::None);
				} else if (bTeleport) {
					Actor->SetActorTransform(NewTransform, false, nullptr, ETeleportType::TeleportPhysics);
				} else {
					Actor->SetActorTransform(NewTransform, false, nullptr, ETeleportType::None);
				}

				// Fire typed OnUpdate callbacks
				if (OnTransformUpdateCB) OnTransformUpdateCB(Frame.Alpha, NewTransform);
			}

			if (OnActorUpdateCB) OnActorUpdateCB(Frame.Alpha, Actor);
		}

	protected:
		FTransform GetCurrentTransform() const override {
			return Actor ? Actor->GetActorTransform() : FTransform::Identity;
		}

	private:
		AActor* Actor = nullptr;
		bool bTeleport = false;
		bool bSweep = false;
		FHitResult* HitResult = nullptr;
		TFunction<void(float, AActor*)> OnActorUpdateCB;
		TFunction<void(float, const FTransform&)> OnTransformUpdateCB;
	};
}
