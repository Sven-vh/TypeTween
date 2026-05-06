// ============================================================
// GENERATED FILE — do not edit by hand.
// To modify all types: edit Scripts/TweenAsyncType.h.template
// To add/remove types:  edit Scripts/generate_tween_types.py
// To regenerate:        run  Scripts/generate_tween_types.py
// ============================================================
#pragma once
#include "CoreMinimal.h"
#include "Blueprints/TweenAsyncBase.h"
#include "TypeTween.h"
#include "TweenAsyncInt32.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInt32TweenUpdate, int32, CurrentValue);

USTRUCT(BlueprintType)
struct FTweenInt32Config : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 From = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 To = 1;
};

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncInt32Base : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnInt32TweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenInt32Config TweenConfig;

	FORCEINLINE void CallOnUpdate(const int32& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncInt32 : public UTweenAsyncInt32Base {
	GENERATED_BODY()

	friend class UTweenAsyncInt32Factory;

protected:
	virtual void Activate() override {
		if (!WorldContextObject) {
			SetReadyToDestroy();
			return;
		}

		const FTweenSettings Settings = TweenConfig.Resolve();

		auto& Tween = TypeTween::Tween<int32>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const int32& CurrentValue) {
					CallOnUpdate(CurrentValue);
				}
			)
			.OnComplete(
				[this]() {
					OnTweenComplete();
				}
			);

		ActivateAdvanced(Tween);
	}
};

UCLASS()
class TYPETWEEN_API UTweenAsyncInt32Factory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Int32"
			))
	static UTweenAsyncInt32* TweenInt32(
		UObject* InWorldContextObject,
		FTweenInt32Config Tween
	) {
		UTweenAsyncInt32* Node = NewObject<UTweenAsyncInt32>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenConfig = Tween;
		Node->RegisterWithGameInstance(InWorldContextObject);
		return Node;
	}
};
