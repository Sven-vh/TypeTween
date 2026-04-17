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
// Abstract base (Simple) — OnUpdate + config only
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncDoubleBase : public UTweenAsyncBaseSimple {
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
// Abstract base (Advanced) — OnUpdate + config + all events
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncDoubleBaseAdvanced : public UTweenAsyncBaseAdvanced {
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
// Concrete async node (Simple) — OnUpdate + OnComplete only
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncDoubleSimple : public UTweenAsyncDoubleBase {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Double",
			ToolTip = "Tweens a double from [From] to [To]."
			))
	static UTweenAsyncDoubleSimple* TweenDouble(
		UObject* InWorldContextObject,
		FTweenDoubleConfig Tween
	) {
		UTweenAsyncDoubleSimple* Node = NewObject<UTweenAsyncDoubleSimple>();
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

		ActivateSimple(Tween);
	}
};

// ─────────────────────────────────────────────────────────────
// Concrete async node (Advanced) — all events
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncDoubleAdvanced : public UTweenAsyncDoubleBaseAdvanced {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Double (Advanced)",
			ToolTip = "Tweens a double from [From] to [To]."
			))
	static UTweenAsyncDoubleAdvanced* TweenDoubleAdvanced(
		UObject* InWorldContextObject,
		FTweenDoubleConfig Tween
	) {
		UTweenAsyncDoubleAdvanced* Node = NewObject<UTweenAsyncDoubleAdvanced>();
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