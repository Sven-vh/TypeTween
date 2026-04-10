#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "TweenTypes.h"
#include "TweenControl.h"
#include "TweenAsyncBase.generated.h"

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncBase : public UBlueprintAsyncActionBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FTweenSimpleDelegate OnCreated;

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

	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FTweenSimpleDelegate OnComplete;

protected:
	void ActivateCommon(TypeTween::ITweenControl& Tween);

	UPROPERTY()
	UObject* WorldContextObject = nullptr;

	UPROPERTY()
	FTweenConfig TweenConfig;
};