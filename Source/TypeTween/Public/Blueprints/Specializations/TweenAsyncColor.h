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
#include "TweenAsyncColor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorTweenUpdate, FLinearColor, CurrentValue);

// ─────────────────────────────────────────────────────────────
// Config struct — exposed to Blueprint details panel
// ─────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FTweenColorConfig : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor From = FLinearColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor To = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EColorLerpMode LerpMode = EColorLerpMode::Linear;
};

// ─────────────────────────────────────────────────────────────
// Abstract base — OnUpdate + config + all events
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncColorBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnColorTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenColorConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const FLinearColor& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Concrete async node — delegates + Activate, no factory fn
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncColor : public UTweenAsyncColorBase {
	GENERATED_BODY()

	friend class UTweenAsyncColorFactory;

protected:
	virtual void Activate() override {
		if (!WorldContextObject) {
			SetReadyToDestroy();
			return;
		}

		const FTweenSettings Settings = TweenConfig.Resolve();

		auto& Tween = TypeTween::Tween<FLinearColor>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.LerpMode(TweenConfig.LerpMode)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const FLinearColor& CurrentValue) {
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
// ProxyClass to UTweenAsyncLinearColor (which keeps BlueprintType
// for delegate pin generation).
// ─────────────────────────────────────────────────────────────

UCLASS()
class TYPETWEEN_API UTweenAsyncColorFactory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Linear Color"
			))
	static UTweenAsyncColor* TweenColor(
		UObject* InWorldContextObject,
		FTweenColorConfig Tween
	) {
		UTweenAsyncColor* Node = NewObject<UTweenAsyncColor>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenConfig = Tween;
		Node->RegisterWithGameInstance(InWorldContextObject);
		return Node;
	}
};
