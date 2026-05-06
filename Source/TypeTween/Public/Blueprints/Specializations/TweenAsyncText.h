// ============================================================
// GENERATED FILE - do not edit by hand.
// To modify all types: edit Scripts/TweenAsyncType.h.template
// To add/remove types:  edit Scripts/generate_tween_types.py
// To regenerate:        run  Scripts/generate_tween_types.py
// ============================================================
#pragma once
#include "CoreMinimal.h"
#include "Blueprints/TweenAsyncBase.h"
#include "TypeTween.h"
#include "Tools/TextLerps.h"
#include "TweenAsyncText.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTextTweenUpdate, FText, CurrentValue);

// -------------------------------------------------------------
// Config struct - exposed to Blueprint details panel
// -------------------------------------------------------------

USTRUCT(BlueprintType)
struct FTweenTextConfig : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText From = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText To = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETextLerpMode LerpMode = ETextLerpMode::Scramble;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETextGlyphSet GlyphSet = ETextGlyphSet::Alphanumeric;

	/** Only used when GlyphSet is set to Custom. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "GlyphSet == ETextGlyphSet::Custom", EditConditionHides))
	FString CustomGlyphs;
};

// -------------------------------------------------------------
// Abstract base - OnUpdate + config + all events
// -------------------------------------------------------------

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncTextBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnTextTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenTextConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const FText& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// -------------------------------------------------------------
// Concrete async node - delegates + Activate, no factory fn
// -------------------------------------------------------------

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncText : public UTweenAsyncTextBase {
	GENERATED_BODY()

	friend class UTweenAsyncTextFactory;

protected:
	virtual void Activate() override {
		if (!WorldContextObject) {
			SetReadyToDestroy();
			return;
		}

		const FTweenSettings Settings = TweenConfig.Resolve();

		auto& Tween = TypeTween::Tween<FText>(WorldContextObject)
			.From(TweenConfig.From)
			.To(TweenConfig.To)
			.Mode(TweenConfig.LerpMode)
			.GlyphSet(TweenConfig.GlyphSet, TweenConfig.CustomGlyphs)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const FText& CurrentValue) {
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

// -------------------------------------------------------------
// Factory - NOT BlueprintType, invisible to UK2Node_AsyncAction
// auto-scanner. K2Node sets ProxyFactoryClass to this and
// ProxyClass to UTweenAsyncText (which keeps BlueprintType
// for delegate pin generation).
// -------------------------------------------------------------

UCLASS()
class TYPETWEEN_API UTweenAsyncTextFactory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Text"
			))
	static UTweenAsyncText* TweenText(
		UObject* InWorldContextObject,
		FTweenTextConfig Tween
	) {
		UTweenAsyncText* Node = NewObject<UTweenAsyncText>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenConfig = Tween;
		Node->RegisterWithGameInstance(InWorldContextObject);
		return Node;
	}
};