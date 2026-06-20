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
#include "TweenAsyncDouble.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoubleTweenUpdate, double, CurrentValue);

USTRUCT(BlueprintType)
struct FTweenDoubleSettings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	double From = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	double To = 1.0;

	/* Common tween settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween|Settings")
	FTweenSettings Settings;
};

USTRUCT(BlueprintType)
struct FTweenDoubleHandle {
	GENERATED_BODY()

	TypeTween::TTweenHandle<double> Handle;
};

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncDoubleBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnDoubleTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenDoubleSettings TweenSettings;

	TypeTween::TTweenWeakHandle<double> TweenHandle;

	FORCEINLINE void CallOnUpdate(const double& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncDouble : public UTweenAsyncDoubleBase {
	GENERATED_BODY()

	friend class UTweenAsyncDoubleFactory;

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
				[this](float /*Alpha*/, const double& CurrentValue) {
					CallOnUpdate(CurrentValue);
				}
			);

		ActivateAdvanced(*TweenHandle.ToShared());
	}
};

UCLASS()
class TYPETWEEN_API UTweenAsyncDoubleFactory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Double"
			))
	static UTweenAsyncDouble* TweenDouble(
		UObject* InWorldContextObject,
		FTweenDoubleSettings Tween,
		FTweenDoubleHandle& handle
	) {
		UTweenAsyncDouble* Node = NewObject<UTweenAsyncDouble>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenSettings = Tween;
		Node->TweenHandle = TypeTween::Tween<double>(InWorldContextObject);
		Node->RegisterWithGameInstance(InWorldContextObject);

		/* output */
		handle.Handle = Node->TweenHandle.ToShared();

		return Node;
	}
};

/* conversion function library */
UCLASS()
class TYPETWEEN_API UTweenDoubleFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TypeTween|Config|Double")
	static FTweenDoubleSettings GetSettings(const FTweenDoubleHandle& In) {
		if (!ensureMsgf(In.Handle, TEXT("GetSettings: Input handle has no tween (nullptr)!"))) {
			return {};
		}

		FTweenDoubleSettings Settings;
		Settings.From = In.Handle->GetStart().Get(0.0);
		Settings.To = In.Handle->GetEnd().Get(1.0);
		Settings.Settings = In.Handle->GetSettings();
		return Settings;
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Config|Double")
	static void SetSettings(UPARAM(ref) FTweenDoubleHandle& In, FTweenDoubleSettings Settings) {
		if (!ensureMsgf(In.Handle, TEXT("SetSettings: Input handle has no tween (nullptr)!"))) {
			return;
		}
		In.Handle->From(Settings.From)
			.To(Settings.To)
			.Preset(Settings.Settings);
	}

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "->"), Category = "TypeTween|Conversions")
	static FTweenHandle ConvertToTweenHandle(const FTweenDoubleHandle& In) {
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
	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Double")
	static void PauseTween(UPARAM(ref) FTweenDoubleHandle& In) {
		UTypeTweenLibrary::PauseTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Double")
	static void ResumeTween(UPARAM(ref) FTweenDoubleHandle& In) {
		UTypeTweenLibrary::ResumeTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Double")
	static void RestartTween(UPARAM(ref) FTweenDoubleHandle& In) {
		UTypeTweenLibrary::RestartTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Double")
	static void FinishTween(UPARAM(ref) FTweenDoubleHandle& In) {
		UTypeTweenLibrary::FinishTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Double")
	static void KillTween(UPARAM(ref) FTweenDoubleHandle& In) {
		UTypeTweenLibrary::KillTween(ConvertToTweenHandle(In));
	}

	/* Querying - thin wrappers from UTypeTweenLibrary for UX and ease of use */
	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Double")
	static bool IsValid(const FTweenDoubleHandle& In) {
		return UTypeTweenLibrary::IsValid(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Double")
	static bool IsDone(const FTweenDoubleHandle& In) {
		return UTypeTweenLibrary::IsDone(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Double")
	static bool IsPlaying(const FTweenDoubleHandle& In) {
		return UTypeTweenLibrary::IsPlaying(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Double")
	static bool IsPaused(const FTweenDoubleHandle& In) {
		return UTypeTweenLibrary::IsPaused(ConvertToTweenHandle(In));
	}
};
