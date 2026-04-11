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
	UPROPERTY()
	UObject* WorldContextObject = nullptr;
};

/*
Only include OnComplete for simplicity
*/
UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncBaseSimple : public UTweenAsyncBase {
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FTweenSimpleDelegate OnComplete;

protected:
	void ActivateSimple(TypeTween::ITweenControl& Tween);
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