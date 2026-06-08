// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
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
#include "TweenAsyncFloat.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatTweenUpdate, float, CurrentValue);

USTRUCT(BlueprintType)
struct FTweenFloatConfig : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	float From = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	float To = 1.f;
};

USTRUCT(BlueprintType)
struct FTweenFloatHandle {
	GENERATED_BODY()

	TypeTween::TweenHandle<float> Handle;
};

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncFloatBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnFloatTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenFloatConfig TweenConfig;

	FTweenFloatHandle TweenHandle;

	FORCEINLINE void CallOnUpdate(const float& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncFloat : public UTweenAsyncFloatBase {
	GENERATED_BODY()

	friend class UTweenAsyncFloatFactory;

protected:
	virtual void Activate() override {
		if (!WorldContextObject) {
			SetReadyToDestroy();
			return;
		}

		const FTweenSettings Settings = TweenConfig.Resolve();

		//auto& Tween = TypeTween::Tween<float>(WorldContextObject)
		//	.From(TweenConfig.From)
		//	.To(TweenConfig.To)
		//	.Preset(Settings)
		//	.OnUpdate(
		//		[this](float /*Alpha*/, const float& CurrentValue) {
		//			CallOnUpdate(CurrentValue);
		//		}
		//	)
		//	.OnComplete(
		//		[this]() {
		//			OnTweenComplete();
		//		}
		//	);

		TweenHandle.Handle->From(TweenConfig.From)
			.To(TweenConfig.To)
			.Preset(Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const float& CurrentValue) {
					CallOnUpdate(CurrentValue);
				}
			)
			.OnComplete(
				[this]() {
					OnTweenComplete();
				}
			);

		ActivateAdvanced(*TweenHandle.Handle);
	}
};

UCLASS()
class TYPETWEEN_API UTweenAsyncFloatFactory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Float"
			))
	static UTweenAsyncFloat* TweenFloat(
		UObject* InWorldContextObject,
		FTweenFloatConfig Tween,
		FTweenFloatHandle& test
	) {
		UTweenAsyncFloat* Node = NewObject<UTweenAsyncFloat>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenConfig = Tween;
		Node->TweenHandle.Handle = TypeTween::Tween<float>(InWorldContextObject);
		Node->RegisterWithGameInstance(InWorldContextObject);

		/* output */
		test = Node->TweenHandle;

		return Node;
	}
};

/* conversion function library */
UCLASS()
class TYPETWEEN_API UTweenFloatFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "->"), Category = "TypeTween|Conversions")
	static FTweenHandle ConvertToTweenHandle(const FTweenFloatHandle& In) {
		if (!ensureMsgf(In.Handle, TEXT("ConvertToTweenHandle: Input handle has no tween (nullptr)!"))) {
			return {};
		}

		return FTweenHandle{ In.Handle };
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Config")
	static FTweenFloatConfig GetConfig(const FTweenFloatHandle& In) {
		if (!ensureMsgf(In.Handle, TEXT("GetConfig: Input handle has no tween (nullptr)!"))) {
			return {};
		}

		FTweenFloatConfig Config;
		Config.From = In.Handle->GetStart().Get(0.f);
		Config.To = In.Handle->GetEnd().Get(1.f);
		Config.Settings = In.Handle->GetSettings();
		return Config;
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Config")
	static void SetConfig(UPARAM(ref) FTweenFloatHandle& In, FTweenFloatConfig Config) {
		if (!ensureMsgf(In.Handle, TEXT("SetConfig: Input handle has no tween (nullptr)!"))) {
			return;
		}
		In.Handle->From(Config.From)
			.To(Config.To)
			.Preset(Config.Resolve());
	}

};