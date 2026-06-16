// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "TweenTypes.h"
#include "TweenControl.h"
#include "TweenHandle.h"
#include "TweenAsyncBase.generated.h"

USTRUCT(BlueprintType)
struct TYPETWEEN_API FTweenHandle {
	GENERATED_BODY()

	TypeTween::FTweenHandle Handle;
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
class TYPETWEEN_API UTweenAsyncBase : public UTweenAsyncBaseSimple {
	GENERATED_BODY()
public:

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
