// ============================================================
// GENERATED FILE — do not edit by hand.
// To modify all types: edit Scripts/TweenAsyncType.h.template
// To add/remove types:  edit Scripts/generate_tween_types.py
// To regenerate:        run  Scripts/generate_tween_types.py
// ============================================================
#pragma once
#include "CoreMinimal.h"
#include "Blueprints/TweenAsyncBase.h"
#include "TypeTween.h"
#include "TweenAsyncVector2D.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVector2DTweenUpdate, FVector2D, CurrentValue);

// ─────────────────────────────────────────────────────────────
// Config struct — exposed to Blueprint details panel
// ─────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FTweenVector2DConfig : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D From = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D To = FVector2D::UnitVector;
};

// ─────────────────────────────────────────────────────────────
// Abstract base (Simple) — OnUpdate + config only
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncVector2DBase : public UTweenAsyncBaseSimple {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnVector2DTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenVector2DConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const FVector2D& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Abstract base (Advanced) — OnUpdate + config + all events
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncVector2DBaseAdvanced : public UTweenAsyncBaseAdvanced {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnVector2DTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenVector2DConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const FVector2D& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Concrete async node (Simple) — OnUpdate + OnComplete only
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncVector2DSimple : public UTweenAsyncVector2DBase {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Vector2D",
			ToolTip = "Tweens an FVector2D from [From] to [To]."
			))
	static UTweenAsyncVector2DSimple* TweenVector2D(
		UObject* InWorldContextObject,
		FTweenVector2DConfig Tween
	) {
		UTweenAsyncVector2DSimple* Node = NewObject<UTweenAsyncVector2DSimple>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenConfig = Tween;
		Node->RegisterWithGameInstance(InWorldContextObject);
		return Node;
	}

protected:
	virtual void Activate() override {
		if (!WorldContextObject) {
			SetReadyToDestroy();
			return;
		}

		const FTweenSettings Settings = TweenConfig.Resolve();

		auto& Tween = TypeTween::Tween<FVector2D>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const FVector2D& CurrentValue) {
					CallOnUpdate(CurrentValue);
				}
			)
			.OnComplete(
				[this]() {
					OnTweenComplete();
				}
			);

		ActivateSimple(Tween);
	}
};

// ─────────────────────────────────────────────────────────────
// Concrete async node (Advanced) — all events
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncVector2DAdvanced : public UTweenAsyncVector2DBaseAdvanced {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Vector2D (Advanced)",
			ToolTip = "Tweens an FVector2D from [From] to [To]."
			))
	static UTweenAsyncVector2DAdvanced* TweenVector2DAdvanced(
		UObject* InWorldContextObject,
		FTweenVector2DConfig Tween
	) {
		UTweenAsyncVector2DAdvanced* Node = NewObject<UTweenAsyncVector2DAdvanced>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenConfig = Tween;
		Node->RegisterWithGameInstance(InWorldContextObject);
		return Node;
	}

protected:
	virtual void Activate() override {
		if (!WorldContextObject) {
			SetReadyToDestroy();
			return;
		}

		const FTweenSettings Settings = TweenConfig.Resolve();

		auto& Tween = TypeTween::Tween<FVector2D>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const FVector2D& CurrentValue) {
					CallOnUpdate(CurrentValue);
				}
			)
			.OnComplete(
				[this]() {
					OnTweenComplete();
				}
			);

		ActivateAdvanced(Tween);
	}
};