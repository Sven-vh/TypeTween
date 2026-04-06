#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "ITween.h"
#include "TweenSubsystem.generated.h"

// Manages all active tweens for the current game instance.
// Lives as long as the GameInstance - survives level transitions.
UCLASS()
class TYPETWEEN_API UTweenSubsystem : public UGameInstanceSubsystem, public FTickableGameObject {
	GENERATED_BODY()

public:
	static UTweenSubsystem* Get(const UObject* WorldContext);

	// Called by Tweening::tween(), stores the tween and returns a stable ref.
	template<typename T, typename... Args>
	TypeTween::ITween<T, Args...>& RegisterTween(TSharedRef<TypeTween::ITween<T, Args...>> InTween) {
		TypeTween::ITween<T, Args...>* Raw = &InTween.Get();

		// Store weak self-reference for handle conversion
		Raw->SetSelfWeak(InTween);

		FActiveTween Entry;
		Entry.Lifetime = InTween; // keeps it alive
		Entry.FnTick = [Raw](float Dt) { Raw->Tick(Dt); };
		Entry.FnIsDone = [Raw]() { return Raw->IsDone(); };
		ActiveTweens.Add(MoveTemp(Entry));

		return *Raw;
	}

	// Immediately cancel all active tweens.
	UFUNCTION(BlueprintCallable, Category = "Tweening")
	void StopAll();

	// FTickableGameObject interface
	void Tick(float DeltaTime) override;
	TStatId GetStatId() const override;
	bool IsTickable()         const override { return !ActiveTweens.IsEmpty(); }
	bool IsTickableWhenPaused() const override { return false; }
	bool IsTickableInEditor()   const override { return false; }

private:
	struct FActiveTween {
		TSharedPtr<void>       Lifetime; // type-erased shared ownership
		TFunction<void(float)> FnTick;
		TFunction<bool()>      FnIsDone;
	};

	TArray<FActiveTween> ActiveTweens;
};