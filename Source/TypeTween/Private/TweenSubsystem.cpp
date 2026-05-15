// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#include "TweenSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

UTweenSubsystem* UTweenSubsystem::Get(const UObject* WorldContext) {
	if (!WorldContext) return nullptr;
	const UWorld* World = GEngine->GetWorldFromContextObject(
		WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;
	const UGameInstance* GI = World->GetGameInstance();
	return GI ? GI->GetSubsystem<UTweenSubsystem>() : nullptr;
}

void UTweenSubsystem::Tick(float DeltaTime) {
	for (FActiveTween& T : ActiveTweens)
		T.FnTick(DeltaTime);

	// Remove tweens that are done AND have no external handle keeping them alive.
	// If a caller holds an FTweenHandle (shared_ptr), use_count > 1 - keep it.
	ActiveTweens.RemoveAll([](const FActiveTween& T) {
		return T.FnIsDone() && T.Lifetime.IsUnique();
		});
}

void UTweenSubsystem::StopAll() {
	ActiveTweens.Empty();
}

TStatId UTweenSubsystem::GetStatId() const {
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTweenSubsystem, STATGROUP_Tickables);
}
