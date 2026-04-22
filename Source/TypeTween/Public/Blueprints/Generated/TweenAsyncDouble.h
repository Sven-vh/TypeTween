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
#include "TweenAsyncDouble.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoubleTweenUpdate, double, CurrentValue);

// ─────────────────────────────────────────────────────────────
// Config struct — exposed to Blueprint details panel
// ─────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FTweenDoubleConfig : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double From = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double To = 1.0;
};

// ─────────────────────────────────────────────────────────────
// Abstract base — OnUpdate + config + all events
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncDoubleBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnDoubleTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenDoubleConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const double& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Concrete async node — delegates + Activate, no factory fn
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncDouble : public UTweenAsyncDoubleBase {
	GENERATED_BODY()

	friend class UTweenAsyncDoubleFactory;

protected:
	virtual void Activate() override {
		if (!WorldContextObject) {
			SetReadyToDestroy();
			return;
		}

		const FTweenSettings Settings = TweenConfig.Resolve();

		auto& Tween = TypeTween::Tween<double>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const double& CurrentValue) {
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
// ProxyClass to UTweenAsyncDouble (which keeps BlueprintType
// for delegate pin generation).
// ─────────────────────────────────────────────────────────────

UCLASS()
class TYPETWEEN_API UTweenAsyncDoubleFactory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Double"
			))
	static UTweenAsyncDouble* TweenDouble(
		UObject* InWorldContextObject,
		FTweenDoubleConfig Tween
	) {
		UTweenAsyncDouble* Node = NewObject<UTweenAsyncDouble>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenConfig = Tween;
		Node->RegisterWithGameInstance(InWorldContextObject);
		return Node;
	}
};
