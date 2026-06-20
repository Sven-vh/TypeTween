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
#include "Blueprints/TweenFunctionLibrary.h"
#include "TweenAsyncFloat.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatTweenUpdate, float, CurrentValue);

USTRUCT(BlueprintType)
struct FTweenFloatSettings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	float From = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	float To = 1.f;

	/* Common tween settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween|Settings")
	FTweenSettings Settings;
};

USTRUCT(BlueprintType)
struct FTweenFloatHandle {
	GENERATED_BODY()

	TypeTween::TTweenHandle<float> Handle;
};

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncFloatBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnFloatTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenFloatSettings TweenSettings;

	TypeTween::TTweenWeakHandle<float> TweenHandle;

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

		TweenHandle->From(TweenSettings.From)
			.To(TweenSettings.To)
			.Preset(TweenSettings.Settings)
			.OnUpdate(
				[this](float /*Alpha*/, const float& CurrentValue) {
					CallOnUpdate(CurrentValue);
				}
			);

		ActivateAdvanced(*TweenHandle.ToShared());
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
		FTweenFloatSettings Tween,
		FTweenFloatHandle& handle
	) {
		UTweenAsyncFloat* Node = NewObject<UTweenAsyncFloat>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenSettings = Tween;
		Node->TweenHandle = TypeTween::Tween<float>(InWorldContextObject);
		Node->RegisterWithGameInstance(InWorldContextObject);

		/* output */
		handle.Handle = Node->TweenHandle.ToShared();

		return Node;
	}
};

/* conversion function library */
UCLASS()
class TYPETWEEN_API UTweenFloatFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TypeTween|Config|Float")
	static FTweenFloatSettings GetSettings(const FTweenFloatHandle& In) {
		if (!ensureMsgf(In.Handle, TEXT("GetSettings: Input handle has no tween (nullptr)!"))) {
			return {};
		}

		FTweenFloatSettings Settings;
		Settings.From = In.Handle->GetStart().Get(0.f);
		Settings.To = In.Handle->GetEnd().Get(1.f);
		Settings.Settings = In.Handle->GetSettings();
		return Settings;
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Config|Float")
	static void SetSettings(UPARAM(ref) FTweenFloatHandle& In, FTweenFloatSettings Settings) {
		if (!ensureMsgf(In.Handle, TEXT("SetSettings: Input handle has no tween (nullptr)!"))) {
			return;
		}
		In.Handle->From(Settings.From)
			.To(Settings.To)
			.Preset(Settings.Settings);
	}

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "->"), Category = "TypeTween|Conversions")
	static FTweenHandle ConvertToTweenHandle(const FTweenFloatHandle& In) {
		if (!ensureMsgf(In.Handle, TEXT("ConvertToTweenHandle: Input handle has no tween (nullptr)!"))) {
			return {};
		}

		TSharedPtr<TypeTween::ITweenControl, ESPMode::ThreadSafe> Pinned = In.Handle.GetTypedPtr();
		if (!Pinned.IsValid()) {
			return {};  // Tween was destroyed between the ensure and here
		}

		FTweenHandle Result;
		Result.Handle = TypeTween::FTweenHandle(Pinned);
		return Result;
	}

	/* Control - thin wrappers from UTypeTweenLibrary for UX and ease of use */
	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Float")
	static void PauseTween(UPARAM(ref) FTweenFloatHandle& In) {
		UTypeTweenLibrary::PauseTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Float")
	static void ResumeTween(UPARAM(ref) FTweenFloatHandle& In) {
		UTypeTweenLibrary::ResumeTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Float")
	static void RestartTween(UPARAM(ref) FTweenFloatHandle& In) {
		UTypeTweenLibrary::RestartTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Float")
	static void FinishTween(UPARAM(ref) FTweenFloatHandle& In) {
		UTypeTweenLibrary::FinishTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Float")
	static void KillTween(UPARAM(ref) FTweenFloatHandle& In) {
		UTypeTweenLibrary::KillTween(ConvertToTweenHandle(In));
	}

	/* Querying - thin wrappers from UTypeTweenLibrary for UX and ease of use */
	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Float")
	static bool IsValid(const FTweenFloatHandle& In) {
		return UTypeTweenLibrary::IsValid(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Float")
	static bool IsDone(const FTweenFloatHandle& In) {
		return UTypeTweenLibrary::IsDone(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Float")
	static bool IsPlaying(const FTweenFloatHandle& In) {
		return UTypeTweenLibrary::IsPlaying(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Float")
	static bool IsPaused(const FTweenFloatHandle& In) {
		return UTypeTweenLibrary::IsPaused(ConvertToTweenHandle(In));
	}
};
