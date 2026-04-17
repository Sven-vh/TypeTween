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
#include "TweenAsyncFloat.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatTweenUpdate, float, CurrentValue);

// ─────────────────────────────────────────────────────────────
// Config struct — exposed to Blueprint details panel
// ─────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FTweenFloatConfig : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float From = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float To = 1.f;
};

// ─────────────────────────────────────────────────────────────
// Abstract base (Simple) — OnUpdate + config only
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncFloatBase : public UTweenAsyncBaseSimple {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnFloatTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenFloatConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const float& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Abstract base (Advanced) — OnUpdate + config + all events
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncFloatBaseAdvanced : public UTweenAsyncBaseAdvanced {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnFloatTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenFloatConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const float& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Concrete async node (Simple) — OnUpdate + OnComplete only
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncFloatSimple : public UTweenAsyncFloatBase {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Float",
			ToolTip = "Tweens a float from [From] to [To]."
			))
	static UTweenAsyncFloatSimple* TweenFloat(
		UObject* InWorldContextObject,
		FTweenFloatConfig Tween
	) {
		UTweenAsyncFloatSimple* Node = NewObject<UTweenAsyncFloatSimple>();
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

		auto& Tween = TypeTween::Tween<float>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const float& CurrentValue) {
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
class TYPETWEEN_API UTweenAsyncFloatAdvanced : public UTweenAsyncFloatBaseAdvanced {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Float (Advanced)",
			ToolTip = "Tweens a float from [From] to [To]."
			))
	static UTweenAsyncFloatAdvanced* TweenFloatAdvanced(
		UObject* InWorldContextObject,
		FTweenFloatConfig Tween
	) {
		UTweenAsyncFloatAdvanced* Node = NewObject<UTweenAsyncFloatAdvanced>();
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

		auto& Tween = TypeTween::Tween<float>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const float& CurrentValue) {
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