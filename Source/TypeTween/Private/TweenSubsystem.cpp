// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#include "TweenSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

UTypeTweenSubsystem* UTypeTweenSubsystem::Get(const UObject* WorldContext) {
	if (!WorldContext) return nullptr;
	const UWorld* World = GEngine->GetWorldFromContextObject(
		WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;
	const UGameInstance* GI = World->GetGameInstance();
	return GI ? GI->GetSubsystem<UTypeTweenSubsystem>() : nullptr;
}

void UTypeTweenSubsystem::Tick(float DeltaTime) {

	for (FActiveTween& T : ActiveTweens) {
		T.FnTick(DeltaTime);
	}

	// Remove tweens that are done AND have no intentional external handle.
	// or that are killed, regardless of handles.
	ActiveTweens.RemoveAll([](const FActiveTween& T) {
		return T.Control->IsKilled() || (T.Control->IsDone() && T.Control.GetSharedReferenceCount() <= 2);
		});
}

void UTypeTweenSubsystem::KillAll(const bool IncludeHandles) {
	if (IncludeHandles) {
		ActiveTweens.Empty();
	} else {
		// Don't remove tweens that have intentional external handles (count > 2, same threshold as Tick).
		// Killed tweens are always removed regardless of handles.
		ActiveTweens.RemoveAll([](const FActiveTween& T) {
			return T.Control->IsKilled() || T.Control.GetSharedReferenceCount() <= 2;
			});
	}
}

void UTypeTweenSubsystem::PauseTweens() {
	for (FActiveTween& T : ActiveTweens) {
		T.Control->Pause();
	}
}

TStatId UTypeTweenSubsystem::GetStatId() const {
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTypeTweenSubsystem, STATGROUP_Tickables);
}
