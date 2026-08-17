// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
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
		virtual ~TTransformTweenMixin() = default;
		/* Transform input */
		Derived& From(FTransform InStart) {
			Start = MoveTemp(InStart);
			ComponentsToTween = EComponent::All;
			return static_cast<Derived&>(*this);
		}
		/* End Value [T=1], if not provided, will use current value */
		Derived& To(FTransform InEnd) {
			Waypoints.Add(MoveTemp(InEnd));
			ComponentsToTween = EComponent::All;
			return static_cast<Derived&>(*this);
		}
		/* Multiple waypoints, interpolates through each in order */
		Derived& To(TArray<FTransform> InWaypoints) {
			Waypoints = MoveTemp(InWaypoints);
			ComponentsToTween = EComponent::All;
			return static_cast<Derived&>(*this);
		}
		Derived& By(FTransform InDelta) {
			FTransform Base;
			if (Waypoints.Num() > 0)	Base = Waypoints.Last();
			else if (Start.IsSet())		Base = Start.GetValue();
			else							Base = GetCurrentTransform();

			Waypoints.Add(Base + MoveTemp(InDelta));
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
			GetOrAddLastWaypoint().SetTranslation(InEnd);
			ComponentsToTween |= EComponent::Translation;
			return static_cast<Derived&>(*this);
		}
		Derived& MoveBy(const FVector& Offset) {
			DefaultStart();
			GetOrAddLastWaypoint().SetTranslation(Start->GetTranslation() + Offset);
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
			GetOrAddLastWaypoint().SetRotation(InEnd);
			ComponentsToTween |= EComponent::Rotation;
			return static_cast<Derived&>(*this);
		}
		Derived& RotateBy(const FQuat& Offset) {
			DefaultStart();
			GetOrAddLastWaypoint().SetRotation(Offset * Start->GetRotation());
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
			GetOrAddLastWaypoint().SetRotation(FQuat(InEnd));
			ComponentsToTween |= EComponent::Rotation;
			return static_cast<Derived&>(*this);
		}
		Derived& RotateBy(const FRotator& Offset) {
			DefaultStart();
			GetOrAddLastWaypoint().SetRotation(FQuat(Offset) * Start->GetRotation());
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
			GetOrAddLastWaypoint().SetScale3D(InEnd);
			ComponentsToTween |= EComponent::Scale;
			return static_cast<Derived&>(*this);
		}
		Derived& ScaleBy(const FVector& Offset) {
			DefaultStart();
			GetOrAddLastWaypoint().SetScale3D(Start->GetScale3D() + Offset);
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

		TOptional<FTransform>& GetStart() { return Start; }
		const TOptional<FTransform>& GetStart() const { return Start; }

		TArray<FTransform>& GetWaypoints() { return Waypoints; }
		const TArray<FTransform>& GetWaypoints() const { return Waypoints; }

	protected:
		TOptional<FTransform> Start;
		TArray<FTransform> Waypoints;
		EComponent ComponentsToTween = EComponent::None;

		/** Must be implemented by derived class to provide current transform value */
		virtual FTransform GetCurrentTransform() const = 0;

		void DefaultStart() {
			if (!Start.IsSet()) {
				Start = GetCurrentTransform();
			}
		}

		/** Returns the waypoint currently being built, creating one from the current transform if none exists yet */
		FTransform& GetOrAddLastWaypoint() {
			if (Waypoints.Num() == 0) {
				Waypoints.Add(GetCurrentTransform());
			}
			return Waypoints.Last();
		}

		/** Computes the interpolated transform for the current frame without applying it */
		FTransform ComputeInterpolatedTransform(const FTransform& Current, float Alpha) const {
			FTransform Result = Current;
			if (Start.IsSet() && Waypoints.Num() > 0) {
				const int32 Segments = Waypoints.Num(); // Start->WP0, WP0->WP1, ...
				const float Scaled = FMath::Clamp(Alpha, 0.0f, 1.0f) * Segments;

				int32 SegIndex = FMath::Clamp(FMath::FloorToInt(Scaled), 0, Segments - 1);
				float LocalAlpha = Scaled - SegIndex;

				// Edge case: Alpha == 1.0 exactly lands on the last point cleanly
				if (SegIndex == Segments - 1 && Scaled >= Segments) LocalAlpha = 1.0f;

				const FTransform& A = (SegIndex == 0) ? Start.GetValue() : Waypoints[SegIndex - 1];
				const FTransform& B = Waypoints[SegIndex];

				if (EnumHasAnyFlags(ComponentsToTween, EComponent::Translation))
					Result.SetTranslation(Lerp(A.GetTranslation(), B.GetTranslation(), LocalAlpha));
				if (EnumHasAnyFlags(ComponentsToTween, EComponent::Rotation))
					Result.SetRotation(FQuat::Slerp(A.GetRotation(), B.GetRotation(), LocalAlpha));
				if (EnumHasAnyFlags(ComponentsToTween, EComponent::Scale))
					Result.SetScale3D(Lerp(A.GetScale3D(), B.GetScale3D(), LocalAlpha));
			}
			return Result;
		}

		/** Initializes Start/Waypoints from current transform if not set. Call on first frame. */
		void InitializeDefaults() {
			const FTransform Current = GetCurrentTransform();
			if (!Start.IsSet()) Start = Current;
			if (Waypoints.Num() == 0) Waypoints.Add(Current);
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

		ITween& OnUpdate(TFunction<void(const FTransform& Value)> Fn) {
			OnUpdateCB = [Fn = MoveTemp(Fn)](float, const FTransform& NewValue) {
				Fn(NewValue);
				};
			return *this;
		}

		// ---- Only contract with TweenBase: called each tick ----
		void Interpolate(const Detail::FTweenFrame& Frame) {
			if (Frame.FrameCount == 0) {
				Mixin::InitializeDefaults();
			}
			if (Value && Mixin::Start.IsSet() && Mixin::Waypoints.Num() > 0) {
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

		ITween& OnUpdate(TFunction<void(const FTransform& Transform)> Fn) {
			OnTransformUpdateCB = [Fn = MoveTemp(Fn)](float, const FTransform& Transform) {
				Fn(Transform);
				};
			return *this;
		}

		// ---- Only contract with TweenBase: called each tick ----
		void Interpolate(const Detail::FTweenFrame& Frame) {
			if (!Actor) return;

			if (Frame.FrameCount == 0) {
				Mixin::InitializeDefaults();
			}

			if (Mixin::Start.IsSet() && Mixin::Waypoints.Num() > 0) {
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
