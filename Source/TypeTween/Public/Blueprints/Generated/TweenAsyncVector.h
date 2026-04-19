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
// Abstract base (Simple) — OnUpdate + config only
// ─────────────────────────────────────────────────────────────

// UCLASS(Abstract, BlueprintType)
// class TYPETWEEN_API UTweenAsyncVectorBase : public UTweenAsyncBaseSimple {
// 	GENERATED_BODY()

// public:
// 	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
// 	FOnVectorTweenUpdate OnUpdate;

// protected:
// 	UPROPERTY()
// 	FTweenVectorConfig TweenConfig;

// 	FORCEINLINE void CallOnUpdate(const FVector& CurrentValue) {
// 		if (OnUpdate.IsBound()) {
// 			OnUpdate.Broadcast(CurrentValue);
// 		}
// 	}
// };

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
// Concrete async node (Simple) — OnUpdate + OnComplete only
// ─────────────────────────────────────────────────────────────

// UCLASS(meta = (HideCategories = Object))
// class TYPETWEEN_API UTweenAsyncVectorSimple : public UTweenAsyncVectorBase {
// 	GENERATED_BODY()

// public:
// 	UFUNCTION(BlueprintCallable, Category = "TypeTween",
// 		meta = (
// 			BlueprintInternalUseOnly = "true",
// 			WorldContext = "InWorldContextObject",
// 			DefaultToSelf = "InWorldContextObject",
// 			DisplayName = "Tween Vector",
// 			ToolTip = "Tweens an FVector from [From] to [To]."
// 			))
// 	static UTweenAsyncVectorSimple* TweenVector(
// 		UObject* InWorldContextObject,
// 		FTweenVectorConfig Tween
// 	) {
// 		UTweenAsyncVectorSimple* Node = NewObject<UTweenAsyncVectorSimple>();
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

// 		auto& Tween = TypeTween::Tween<FVector>(WorldContextObject)
// 			.From(TweenConfig.From)
// 			.To(TweenConfig.To)
// 			.Preset(Settings)
// 			.OnUpdate(
// 				[this](float /*Alpha*/, const FVector& CurrentValue) {
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
class TYPETWEEN_API UTweenAsyncVector : public UTweenAsyncVectorBase {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Vector",
			ToolTip = "Tweens an FVector from [From] to [To]."
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