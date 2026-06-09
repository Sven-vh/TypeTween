// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Specializations/ITween.h"
#include "TweenSubsystem.generated.h"

// Manages all active tweens for the current game instance.
// Lives as long as the GameInstance - survives level transitions.
UCLASS()
class TYPETWEEN_API UTypeTweenSubsystem : public UGameInstanceSubsystem, public FTickableGameObject {
	GENERATED_BODY()

public:
	static UTypeTweenSubsystem* Get(const UObject* WorldContext);

	// Called by Tweening::tween(), stores the tween and returns a stable ref.
	template<typename T, typename... Args>
	TypeTween::ITween<T, Args...>& RegisterTween(TSharedRef<TypeTween::ITween<T, Args...>> InTween) {
		TypeTween::ITween<T, Args...>* Raw = &InTween.Get();

		// Store weak self-reference for handle conversion
		Raw->SetSelfWeak(InTween);

		FActiveTween Entry;
		Entry.Control = InTween; // keeps it alive
		Entry.FnTick = [Raw](float Dt) { Raw->Tick(Dt); };
		ActiveTweens.Add(MoveTemp(Entry));

		return *Raw;
	}

	/* Tweening control */
	/*
	* Immediately kills all active tweens. Does not trigger callbacks.
	* @param IncludeHandles If true, also kills tweens held by a handle. If false, only anonymous tweens are affected.
	*/
	UFUNCTION(BlueprintCallable, Category = "TypeTween|General",
		meta = (
			WorldContext = "WorldContextObject",
			DefaultToSelf = "WorldContextObject",
			DisplayName = "Kill All Tweens",
			ToolTip = "Immediately kills all active tweens. Does not trigger callbacks."
			))
	void KillAll(const bool IncludeHandles = false);

	/* Pause all active tweens, they can be resumed later */
	UFUNCTION(BlueprintCallable, Category = "TypeTween|General",
		meta = (
			WorldContext = "WorldContextObject",
			DefaultToSelf = "WorldContextObject",
			DisplayName = "Pause All Tweens",
			ToolTip = "Pauses all currently active tweens, can be resumed individually later. Does NOT pause newly created tweens."
			))
	void PauseTweens();

	/* System Control */
	UFUNCTION(BlueprintCallable, Category = "TypeTween|General",
		meta = (
			WorldContext = "WorldContextObject",
			DefaultToSelf = "WorldContextObject",
			DisplayName = "Disable Tween Subsystem Tick",
			ToolTip = "Disables ticking of the tween subsystem. Active and new tweens will not update until tick is enabled again."
			))
	void DisableTick() { SetTickableTickType(ETickableTickType::Never); }

	UFUNCTION(BlueprintCallable, Category = "TypeTween|General",
		meta = (
			WorldContext = "WorldContextObject",
			DefaultToSelf = "WorldContextObject",
			DisplayName = "Enable Tween Subsystem Tick",
			ToolTip = "Enables ticking of the tween subsystem. Active and new tweens will update as normal."
			))
	void EnableTick() { SetTickableTickType(ETickableTickType::Conditional); }

	// FTickableGameObject interface
	void Tick(float DeltaTime) override;
	TStatId GetStatId() const override;
	bool IsTickable() const override { return !ActiveTweens.IsEmpty(); }
	bool IsTickableWhenPaused() const override { return false; }
	bool IsTickableInEditor() const override { return false; }

private:
	struct FActiveTween {
		TSharedPtr<TypeTween::ITweenControl> Control; // type-erased shared ownership
		TFunction<void(float)> FnTick;
	};

	//TODO: Maybe linked list is better?
	TArray<FActiveTween> ActiveTweens;
};
