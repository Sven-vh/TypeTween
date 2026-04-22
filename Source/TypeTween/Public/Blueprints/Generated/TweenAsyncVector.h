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
#include "TweenAsyncVector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVectorTweenUpdate, FVector, CurrentValue);

// ─────────────────────────────────────────────────────────────
// Config struct — exposed to Blueprint details panel
// ─────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FTweenVectorConfig : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector From = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector To = FVector::OneVector;
};

// ─────────────────────────────────────────────────────────────
// Abstract base — OnUpdate + config + all events
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncVectorBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnVectorTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenVectorConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const FVector& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Concrete async node — delegates + Activate, no factory fn
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncVector : public UTweenAsyncVectorBase {
	GENERATED_BODY()

	friend class UTweenAsyncVectorFactory;

protected:
	virtual void Activate() override {
		if (!WorldContextObject) {
			SetReadyToDestroy();
			return;
		}

		const FTweenSettings Settings = TweenConfig.Resolve();

		auto& Tween = TypeTween::Tween<FVector>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const FVector& CurrentValue) {
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
// ProxyClass to UTweenAsyncVector (which keeps BlueprintType
// for delegate pin generation).
// ─────────────────────────────────────────────────────────────

UCLASS()
class TYPETWEEN_API UTweenAsyncVectorFactory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Vector"
			))
	static UTweenAsyncVector* TweenVector(
		UObject* InWorldContextObject,
		FTweenVectorConfig Tween
	) {
		UTweenAsyncVector* Node = NewObject<UTweenAsyncVector>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenConfig = Tween;
		Node->RegisterWithGameInstance(InWorldContextObject);
		return Node;
	}
};
