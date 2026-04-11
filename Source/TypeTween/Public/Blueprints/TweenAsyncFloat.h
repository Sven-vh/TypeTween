#pragma once
#include "CoreMinimal.h"
#include "TweenAsyncBase.h"
#include "TweenBlueprintTypes.h"
#include "TypeTween.h"
#include "TweenAsyncFloat.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatTweenUpdate, float, CurrentValue);

USTRUCT(BlueprintType)
struct FTweenFloatConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float From = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float To = 1.f;

	/* Visible in the blueprint */
	UPROPERTY(BlueprintReadWrite, meta = (HideInDetailPanel))
	FTweenSettings Settings;

	/* Visible in the details panel, used for presets + overrides */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "My Settings"))
	FTweenConfig Config;

	FTweenSettings Resolve() const {
		return Settings.IsSet() ? Settings : Config.Resolve();
	}
};

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncFloatBase : public UTweenAsyncBaseSimple {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnFloatTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenFloatConfig TweenConfig;

	void CallOnUpdate(float CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}

	void OnTweenComplete() {
		SetReadyToDestroy();
	}
};

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncFloatSimple : public UTweenAsyncFloatBase {
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Tweening|Float",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Float",
			ToolTip = "Tweens a float from [From] to [To].\n\nPrimary use: wire a FTweenFloatConfig variable (supports presets + overrides in the Details panel).\n\nAlternative: expand Advanced and wire a Preset pin directly to bypass the config.",
			AdvancedDisplay = "PresetOverride"
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

		//TODO: maybe it should be Config where
		const FTweenSettings Settings = TweenConfig.Resolve();

		auto& Tween = TypeTween::Tween<float>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float Alpha, const float& CurrentValue) {
					CallOnUpdate(CurrentValue);
				}
			).OnComplete(
				// TODO: Maybe dangerous cause user can override this.
				[this]() {
					OnTweenComplete();
				}
			);

		ActivateSimple(Tween);
	}
};