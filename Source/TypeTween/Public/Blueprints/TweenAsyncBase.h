#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "TweenTypes.h"
#include "TweenControl.h"
#include "TweenAsyncBase.generated.h"

USTRUCT(BlueprintType)
struct TYPETWEEN_API FTweenSettingsConfig {
	GENERATED_BODY()

	/* Wire a runtime FTweenSettings directly (takes priority over Config below) */
	UPROPERTY(BlueprintReadWrite, meta = (HideInDetailPanel))
	FTweenSettings Settings;

	/* Preset + overrides — visible in the Details panel */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Settings"))
	FTweenConfig Config;

	FTweenSettings Resolve() const {
		return Settings.IsSet() ? Settings : Config.Resolve();
	}
};

/*
Only include OnComplete for simplicity
*/
UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncBaseSimple : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FTweenSimpleDelegate OnComplete;

protected:
	void ActivateSimple(TypeTween::ITweenControl& Tween);

	void OnTweenComplete();

	UPROPERTY()
	UObject* WorldContextObject = nullptr;
};

/*
All other events
*/
UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncBaseAdvanced : public UTweenAsyncBaseSimple {
	GENERATED_BODY()
public:
	// TODO: for handles
	//UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	//FTweenSimpleDelegate OnCreated;

	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FTweenSimpleDelegate OnStart;

	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FTweenSimpleDelegate OnCycleBegin;

	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FTweenSimpleDelegate OnForwardEnd;

	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FTweenSimpleDelegate OnReverseBegin;

	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FTweenSimpleDelegate OnCycleEnd;

	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FTweenSimpleDelegate OnRepeat;

protected:
	void ActivateAdvanced(TypeTween::ITweenControl& Tween);
};