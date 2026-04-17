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
#include "TweenAsyncTransform.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransformTweenUpdate, FTransform, CurrentValue);

// ─────────────────────────────────────────────────────────────
// Config struct — exposed to Blueprint details panel
// ─────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FTweenTransformConfig : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform From;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform To;
};

// ─────────────────────────────────────────────────────────────
// Abstract base (Simple) — OnUpdate + config only
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncTransformBase : public UTweenAsyncBaseSimple {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnTransformTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenTransformConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const FTransform& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Abstract base (Advanced) — OnUpdate + config + all events
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncTransformBaseAdvanced : public UTweenAsyncBaseAdvanced {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnTransformTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenTransformConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const FTransform& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Concrete async node (Simple) — OnUpdate + OnComplete only
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncTransformSimple : public UTweenAsyncTransformBase {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Transform",
			ToolTip = "Tweens an FTransform from [From] to [To]."
			))
	static UTweenAsyncTransformSimple* TweenTransform(
		UObject* InWorldContextObject,
		FTweenTransformConfig Tween
	) {
		UTweenAsyncTransformSimple* Node = NewObject<UTweenAsyncTransformSimple>();
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

		auto& Tween = TypeTween::Tween<FTransform>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const FTransform& CurrentValue) {
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
class TYPETWEEN_API UTweenAsyncTransformAdvanced : public UTweenAsyncTransformBaseAdvanced {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Transform (Advanced)",
			ToolTip = "Tweens an FTransform from [From] to [To]."
			))
	static UTweenAsyncTransformAdvanced* TweenTransformAdvanced(
		UObject* InWorldContextObject,
		FTweenTransformConfig Tween
	) {
		UTweenAsyncTransformAdvanced* Node = NewObject<UTweenAsyncTransformAdvanced>();
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

		auto& Tween = TypeTween::Tween<FTransform>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const FTransform& CurrentValue) {
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