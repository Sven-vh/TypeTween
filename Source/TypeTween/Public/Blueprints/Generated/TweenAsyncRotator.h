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
#include "TweenAsyncRotator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRotatorTweenUpdate, FRotator, CurrentValue);

// ─────────────────────────────────────────────────────────────
// Config struct — exposed to Blueprint details panel
// ─────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FTweenRotatorConfig : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator From = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator To = FRotator::ZeroRotator;
};

// ─────────────────────────────────────────────────────────────
// Abstract base (Simple) — OnUpdate + config only
// ─────────────────────────────────────────────────────────────

// UCLASS(Abstract, BlueprintType)
// class TYPETWEEN_API UTweenAsyncRotatorBase : public UTweenAsyncBaseSimple {
// 	GENERATED_BODY()

// public:
// 	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
// 	FOnRotatorTweenUpdate OnUpdate;

// protected:
// 	UPROPERTY()
// 	FTweenRotatorConfig TweenConfig;

// 	FORCEINLINE void CallOnUpdate(const FRotator& CurrentValue) {
// 		if (OnUpdate.IsBound()) {
// 			OnUpdate.Broadcast(CurrentValue);
// 		}
// 	}
// };

// ─────────────────────────────────────────────────────────────
// Abstract base — OnUpdate + config + all events
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncRotatorBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnRotatorTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenRotatorConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const FRotator& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Concrete async node (Simple) — OnUpdate + OnComplete only
// ─────────────────────────────────────────────────────────────

// UCLASS(meta = (HideCategories = Object))
// class TYPETWEEN_API UTweenAsyncRotatorSimple : public UTweenAsyncRotatorBase {
// 	GENERATED_BODY()

// public:
// 	UFUNCTION(BlueprintCallable, Category = "TypeTween",
// 		meta = (
// 			BlueprintInternalUseOnly = "true",
// 			WorldContext = "InWorldContextObject",
// 			DefaultToSelf = "InWorldContextObject",
// 			DisplayName = "Tween Rotator",
// 			ToolTip = "Tweens an FRotator from [From] to [To]."
// 			))
// 	static UTweenAsyncRotatorSimple* TweenRotator(
// 		UObject* InWorldContextObject,
// 		FTweenRotatorConfig Tween
// 	) {
// 		UTweenAsyncRotatorSimple* Node = NewObject<UTweenAsyncRotatorSimple>();
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

// 		auto& Tween = TypeTween::Tween<FRotator>(WorldContextObject)
// 			.From(TweenConfig.From)
// 			.To(TweenConfig.To)
// 			.Preset(Settings)
// 			.OnUpdate(
// 				[this](float /*Alpha*/, const FRotator& CurrentValue) {
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
class TYPETWEEN_API UTweenAsyncRotator : public UTweenAsyncRotatorBase {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Rotator",
			ToolTip = "Tweens an FRotator from [From] to [To]."
			))
	static UTweenAsyncRotator* TweenRotator(
		UObject* InWorldContextObject,
		FTweenRotatorConfig Tween
	) {
		UTweenAsyncRotator* Node = NewObject<UTweenAsyncRotator>();
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

		auto& Tween = TypeTween::Tween<FRotator>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const FRotator& CurrentValue) {
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