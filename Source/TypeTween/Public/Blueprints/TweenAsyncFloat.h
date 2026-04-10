#pragma once
#include "CoreMinimal.h"
#include "TweenAsyncBase.h"
#include "TweenBlueprintTypes.h"
#include "TypeTween.h"
#include "TweenAsyncFloat.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatTweenUpdate, float, CurrentValue);

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncFloat : public UTweenAsyncBase {
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Tweening|Float",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Float",
			ToolTip = "Tweens a float from [From] to [To]. Wire OnCreated as your 'Then' pin. Read CurrentValue on OnUpdate to drive your property each frame."
			))
	static inline UTweenAsyncFloat* TweenFloat(
		UObject* InWorldContextObject,
		float From,
		float To,
		const FTweenSettings& Config
	) {
		UTweenAsyncFloat* Node = NewObject<UTweenAsyncFloat>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenConfig = Config;
		Node->StartValue = From;
		Node->EndValue = To;
		Node->RegisterWithGameInstance(InWorldContextObject);
		return Node;
	}

	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnFloatTweenUpdate OnUpdate;

protected:
	virtual void Activate() override {
		if (!WorldContextObject) {
			SetReadyToDestroy();
			return;
		}

		auto& Tween = TypeTween::Tween<float>(WorldContextObject)
			.From(StartValue)
			.To(EndValue)
			.Preset(TweenConfig)
			.OnUpdate(
				[this](float Alpha, const float& CurrentValue) {
					CallOnUpdate(CurrentValue);
				}
			).OnComplete(
				// TODO: Dangerous cause user can override this.
				[this]() {
					OnTweenComplete();
				}
			);

		ActivateCommon(Tween);
	}

private:

	float StartValue;
	float EndValue;

	void CallOnUpdate(float CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}

	void OnTweenComplete() {
		SetReadyToDestroy();
	}
};