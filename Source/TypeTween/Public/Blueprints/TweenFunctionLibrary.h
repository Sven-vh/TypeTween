// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "CoreMinimal.h"

#include "TweenAsyncBase.h"
#include "TweenFunctionLibrary.generated.h"

UCLASS()
class TYPETWEEN_API UTypeTweenLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween|General",
		meta = (
			WorldContext = "WorldContextObject",
			DefaultToSelf = "WorldContextObject",
			DisplayName = "Kill All Tweens"
			))
	static void KillAllTweens(UObject* WorldContextObject);

	/* Controlling Tweens */
	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control")
	static void PauseTween(const FTweenHandle& TweenHandle);

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control")
	static void ResumeTween(const FTweenHandle& TweenHandle);

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control")
	static void RestartTween(const FTweenHandle& TweenHandle);

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control")
	static void FinishTween(const FTweenHandle& TweenHandle);

	/* Querying Tweens */
	UFUNCTION(BlueprintPure, Category = "TypeTween|Control")
	static bool IsDone(const FTweenHandle& TweenHandle);

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control")
	static bool IsPlaying(const FTweenHandle& TweenHandle);

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control")
	static bool IsPaused(const FTweenHandle& TweenHandle);

	/* Getting/Setting Config during runtime */
	UFUNCTION(BlueprintPure, Category = "TypeTween|Config")
	static const FTweenSettings& GetSettings(const FTweenHandle& TweenHandle);

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Config")
	static void SetSettings(UPARAM(ref) FTweenHandle& TweenHandle, const FTweenSettings& Settings);
};