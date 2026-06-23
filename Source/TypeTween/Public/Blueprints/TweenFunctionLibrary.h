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

	/* Controlling Tweens */
	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control")
	static void PauseTween(FTweenHandle TweenHandle);

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control")
	static void ResumeTween(FTweenHandle TweenHandle);

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control")
	static void RestartTween(FTweenHandle TweenHandle);

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control")
	static void FinishTween(FTweenHandle TweenHandle);

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control")
	static void KillTween(FTweenHandle TweenHandle);

	/* Querying Tweens */
	UFUNCTION(BlueprintPure, Category = "TypeTween|Control",
		meta = (
			DisplayName = "Is Tween Valid",
			ToolTip = "Returns true if a tween has been assigned and is valid."
			))
	static bool IsValid(const FTweenHandle& TweenHandle);
	UFUNCTION(BlueprintPure, Category = "TypeTween|Control",
		meta = (
			DisplayName = "Is Tween Done",
			ToolTip = "Returns true if the tween has completed. Infinite tweens will never be done."
			))
	static bool IsDone(const FTweenHandle& TweenHandle);

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control",
		meta = (
			DisplayName = "Is Tween Playing",
			ToolTip = "Returns true if the tween is currently playing. False when done or paused."
			))
	static bool IsPlaying(const FTweenHandle& TweenHandle);

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control",
		meta = (
			DisplayName = "Is Tween Paused",
			ToolTip = "Returns true if the tween is currently paused."
			))
	static bool IsPaused(const FTweenHandle& TweenHandle);

	/* Getting/Setting Config during runtime */
	UFUNCTION(BlueprintPure, Category = "TypeTween|Config")
	static const FTweenSettings& GetSettings(const FTweenHandle& TweenHandle);

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Config")
	static void SetSettings(FTweenHandle TweenHandle, const FTweenSettings& Settings);
};