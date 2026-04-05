#include "TweenSubsystem.h"
#include "Engine/GameInstance.h"

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
    // If a caller holds an FTweenHandle (shared_ptr), use_count > 1 — keep it.
    ActiveTweens.RemoveAll([](const FActiveTween& T) {
        return T.FnIsDone() && T.Lifetime.IsUnique();
        });

    // print ActiveTweens size
	UE_LOG(LogTemp, Log, TEXT("ActiveTweens: %d"), ActiveTweens.Num());
}

void UTweenSubsystem::StopAll() {
    ActiveTweens.Empty();
}

TStatId UTweenSubsystem::GetStatId() const {
    RETURN_QUICK_DECLARE_CYCLE_STAT(UTweenSubsystem, STATGROUP_Tickables);
}