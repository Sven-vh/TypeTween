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
#include "TweenAsyncRotator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRotatorTweenUpdate, FRotator, CurrentValue);

USTRUCT(BlueprintType)
struct FTweenRotatorSettings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	FRotator From = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	FRotator To = FRotator::ZeroRotator;

	/* Common tween settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween|Settings")
	FTweenSettings Settings;
};

USTRUCT(BlueprintType)
struct FTweenRotatorHandle {
	GENERATED_BODY()

	TypeTween::TTweenHandle<FRotator> Handle;
};

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncRotatorBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnRotatorTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenRotatorSettings TweenSettings;

	TypeTween::TTweenWeakHandle<FRotator> TweenHandle;

	FORCEINLINE void CallOnUpdate(const FRotator& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncRotator : public UTweenAsyncRotatorBase {
	GENERATED_BODY()

	friend class UTweenAsyncRotatorFactory;

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
				[this](float /*Alpha*/, const FRotator& CurrentValue) {
					CallOnUpdate(CurrentValue);
				}
			);

		ActivateAdvanced(*TweenHandle.ToShared());
	}
};

UCLASS()
class TYPETWEEN_API UTweenAsyncRotatorFactory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Rotator"
			))
	static UTweenAsyncRotator* TweenRotator(
		UObject* InWorldContextObject,
		FTweenRotatorSettings Tween,
		FTweenRotatorHandle& handle
	) {
		UTweenAsyncRotator* Node = NewObject<UTweenAsyncRotator>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenSettings = Tween;
		Node->TweenHandle = TypeTween::Tween<FRotator>(InWorldContextObject);
		Node->RegisterWithGameInstance(InWorldContextObject);

		/* output */
		handle.Handle = Node->TweenHandle.ToShared();

		return Node;
	}
};

/* conversion function library */
UCLASS()
class TYPETWEEN_API UTweenRotatorFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TypeTween|Config|Rotator")
	static FTweenRotatorSettings GetSettings(const FTweenRotatorHandle& In) {
		if (!ensureMsgf(In.Handle, TEXT("GetSettings: Input handle has no tween (nullptr)!"))) {
			return {};
		}

		FTweenRotatorSettings Settings;
		Settings.From = In.Handle->GetStart().Get(FRotator::ZeroRotator);
		Settings.To = In.Handle->GetEnd().Get(FRotator::ZeroRotator);
		Settings.Settings = In.Handle->GetSettings();
		return Settings;
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Config|Rotator")
	static void SetSettings(UPARAM(ref) FTweenRotatorHandle& In, FTweenRotatorSettings Settings) {
		if (!ensureMsgf(In.Handle, TEXT("SetSettings: Input handle has no tween (nullptr)!"))) {
			return;
		}
		In.Handle->From(Settings.From)
			.To(Settings.To)
			.Preset(Settings.Settings);
	}

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "->"), Category = "TypeTween|Conversions")
	static FTweenHandle ConvertToTweenHandle(const FTweenRotatorHandle& In) {
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
	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Rotator")
	static void PauseTween(UPARAM(ref) FTweenRotatorHandle& In) {
		UTypeTweenLibrary::PauseTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Rotator")
	static void ResumeTween(UPARAM(ref) FTweenRotatorHandle& In) {
		UTypeTweenLibrary::ResumeTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Rotator")
	static void RestartTween(UPARAM(ref) FTweenRotatorHandle& In) {
		UTypeTweenLibrary::RestartTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Rotator")
	static void FinishTween(UPARAM(ref) FTweenRotatorHandle& In) {
		UTypeTweenLibrary::FinishTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Rotator")
	static void KillTween(UPARAM(ref) FTweenRotatorHandle& In) {
		UTypeTweenLibrary::KillTween(ConvertToTweenHandle(In));
	}

	/* Querying - thin wrappers from UTypeTweenLibrary for UX and ease of use */
	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Rotator")
	static bool IsValid(const FTweenRotatorHandle& In) {
		return UTypeTweenLibrary::IsValid(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Rotator")
	static bool IsDone(const FTweenRotatorHandle& In) {
		return UTypeTweenLibrary::IsDone(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Rotator")
	static bool IsPlaying(const FTweenRotatorHandle& In) {
		return UTypeTweenLibrary::IsPlaying(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Rotator")
	static bool IsPaused(const FTweenRotatorHandle& In) {
		return UTypeTweenLibrary::IsPaused(ConvertToTweenHandle(In));
	}
};
