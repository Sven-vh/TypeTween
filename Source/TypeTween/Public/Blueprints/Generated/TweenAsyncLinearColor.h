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
#include "TweenAsyncLinearColor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLinearColorTweenUpdate, FLinearColor, CurrentValue);

// ─────────────────────────────────────────────────────────────
// Config struct — exposed to Blueprint details panel
// ─────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FTweenLinearColorConfig : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor From = FLinearColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor To = FLinearColor::White;
};

// ─────────────────────────────────────────────────────────────
// Abstract base (Simple) — OnUpdate + config only
// ─────────────────────────────────────────────────────────────

// UCLASS(Abstract, BlueprintType)
// class TYPETWEEN_API UTweenAsyncLinearColorBase : public UTweenAsyncBaseSimple {
// 	GENERATED_BODY()

// public:
// 	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
// 	FOnLinearColorTweenUpdate OnUpdate;

// protected:
// 	UPROPERTY()
// 	FTweenLinearColorConfig TweenConfig;

// 	FORCEINLINE void CallOnUpdate(const FLinearColor& CurrentValue) {
// 		if (OnUpdate.IsBound()) {
// 			OnUpdate.Broadcast(CurrentValue);
// 		}
// 	}
// };

// ─────────────────────────────────────────────────────────────
// Abstract base — OnUpdate + config + all events
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncLinearColorBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnLinearColorTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenLinearColorConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const FLinearColor& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Concrete async node (Simple) — OnUpdate + OnComplete only
// ─────────────────────────────────────────────────────────────

// UCLASS(meta = (HideCategories = Object))
// class TYPETWEEN_API UTweenAsyncLinearColorSimple : public UTweenAsyncLinearColorBase {
// 	GENERATED_BODY()

// public:
// 	UFUNCTION(BlueprintCallable, Category = "TypeTween",
// 		meta = (
// 			BlueprintInternalUseOnly = "true",
// 			WorldContext = "InWorldContextObject",
// 			DefaultToSelf = "InWorldContextObject",
// 			DisplayName = "Tween Linear Color",
// 			ToolTip = "Tweens an FLinearColor from [From] to [To]."
// 			))
// 	static UTweenAsyncLinearColorSimple* TweenLinearColor(
// 		UObject* InWorldContextObject,
// 		FTweenLinearColorConfig Tween
// 	) {
// 		UTweenAsyncLinearColorSimple* Node = NewObject<UTweenAsyncLinearColorSimple>();
// 		Node->WorldContextObject = InWorldContextObject;
// 		Node->TweenConfig = Tween;
// 		Node->RegisterWithGameInstance(InWorldContextObject);
// 		return Node;
// 	}

// protected:
// 	virtual void Activate() override {
// 		if (!WorldContextObject) {
// 			SetReadyToDestroy();
// 			return;
// 		}

// 		const FTweenSettings Settings = TweenConfig.Resolve();

// 		auto& Tween = TypeTween::Tween<FLinearColor>(WorldContextObject)
// 			.From(TweenConfig.From)
// 			.To(TweenConfig.To)
// 			.Preset(Settings)
// 			.OnUpdate(
// 				[this](float /*Alpha*/, const FLinearColor& CurrentValue) {
// 					CallOnUpdate(CurrentValue);
// 				}
// 			)
// 			.OnComplete(
// 				[this]() {
// 					OnTweenComplete();
// 				}
// 			);

// 		ActivateSimple(Tween);
// 	}
// };

// ─────────────────────────────────────────────────────────────
// Concrete async node — all events
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncLinearColor : public UTweenAsyncLinearColorBase {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Linear Color",
			ToolTip = "Tweens an FLinearColor from [From] to [To]."
			))
	static UTweenAsyncLinearColor* TweenLinearColor(
		UObject* InWorldContextObject,
		FTweenLinearColorConfig Tween
	) {
		UTweenAsyncLinearColor* Node = NewObject<UTweenAsyncLinearColor>();
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

		auto& Tween = TypeTween::Tween<FLinearColor>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
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