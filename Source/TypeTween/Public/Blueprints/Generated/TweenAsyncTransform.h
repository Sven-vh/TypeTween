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
// Abstract base — OnUpdate + config + all events
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncTransformBase : public UTweenAsyncBase {
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
// Concrete async node — delegates + Activate, no factory fn
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncTransform : public UTweenAsyncTransformBase {
	GENERATED_BODY()

	friend class UTweenAsyncTransformFactory;

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

// ─────────────────────────────────────────────────────────────
// Factory — NOT BlueprintType, invisible to UK2Node_AsyncAction
// auto-scanner. K2Node sets ProxyFactoryClass to this and
// ProxyClass to UTweenAsyncTransform (which keeps BlueprintType
// for delegate pin generation).
// ─────────────────────────────────────────────────────────────

UCLASS()
class TYPETWEEN_API UTweenAsyncTransformFactory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Transform"
			))
	static UTweenAsyncTransform* TweenTransform(
		UObject* InWorldContextObject,
		FTweenTransformConfig Tween
	) {
		UTweenAsyncTransform* Node = NewObject<UTweenAsyncTransform>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenConfig = Tween;
		Node->RegisterWithGameInstance(InWorldContextObject);
		return Node;
	}
};
