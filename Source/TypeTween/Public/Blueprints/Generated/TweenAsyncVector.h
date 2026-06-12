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
#include "TweenAsyncVector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVectorTweenUpdate, FVector, CurrentValue);

USTRUCT(BlueprintType)
struct FTweenVectorSettings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	FVector From = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	FVector To = FVector::OneVector;

	/* Common tween settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween|Settings")
	FTweenSettings Settings;
};

USTRUCT(BlueprintType)
struct FTweenVectorHandle {
	GENERATED_BODY()

	TypeTween::TTweenHandle<FVector> Handle;
};

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncVectorBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnVectorTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenVectorSettings TweenSettings;

	TypeTween::TTweenWeakHandle<FVector> TweenHandle;

	FORCEINLINE void CallOnUpdate(const FVector& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncVector : public UTweenAsyncVectorBase {
	GENERATED_BODY()

	friend class UTweenAsyncVectorFactory;

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
				[this](float /*Alpha*/, const FVector& CurrentValue) {
					CallOnUpdate(CurrentValue);
				}
			)
			.OnComplete(
				[this]() {
					OnTweenComplete();
				}
			);

		ActivateAdvanced(*TweenHandle.ToShared());
	}
};

UCLASS()
class TYPETWEEN_API UTweenAsyncVectorFactory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Vector"
			))
	static UTweenAsyncVector* TweenVector(
		UObject* InWorldContextObject,
		FTweenVectorSettings Tween,
		FTweenVectorHandle& handle
	) {
		UTweenAsyncVector* Node = NewObject<UTweenAsyncVector>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenSettings = Tween;
		Node->TweenHandle = TypeTween::Tween<FVector>(InWorldContextObject);
		Node->RegisterWithGameInstance(InWorldContextObject);

		/* output */
		handle.Handle = Node->TweenHandle.ToShared();

		return Node;
	}
};

/* conversion function library */
UCLASS()
class TYPETWEEN_API UTweenVectorFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TypeTween|Config|Vector")
	static FTweenVectorSettings GetSettings(const FTweenVectorHandle& In) {
		if (!ensureMsgf(In.Handle, TEXT("GetSettings: Input handle has no tween (nullptr)!"))) {
			return {};
		}

		FTweenVectorSettings Settings;
		Settings.From = In.Handle->GetStart().Get(FVector::ZeroVector);
		Settings.To = In.Handle->GetEnd().Get(FVector::OneVector);
		Settings.Settings = In.Handle->GetSettings();
		return Settings;
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Config|Vector")
	static void SetSettings(UPARAM(ref) FTweenVectorHandle& In, FTweenVectorSettings Settings) {
		if (!ensureMsgf(In.Handle, TEXT("SetSettings: Input handle has no tween (nullptr)!"))) {
			return;
		}
		In.Handle->From(Settings.From)
			.To(Settings.To)
			.Preset(Settings.Settings);
	}

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "->"), Category = "TypeTween|Conversions")
	static FTweenHandle ConvertToTweenHandle(const FTweenVectorHandle& In) {
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
	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Vector")
	static void PauseTween(UPARAM(ref) FTweenVectorHandle& In) {
		UTypeTweenLibrary::PauseTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Vector")
	static void ResumeTween(UPARAM(ref) FTweenVectorHandle& In) {
		UTypeTweenLibrary::ResumeTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Vector")
	static void RestartTween(UPARAM(ref) FTweenVectorHandle& In) {
		UTypeTweenLibrary::RestartTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Vector")
	static void FinishTween(UPARAM(ref) FTweenVectorHandle& In) {
		UTypeTweenLibrary::FinishTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Vector")
	static void KillTween(UPARAM(ref) FTweenVectorHandle& In) {
		UTypeTweenLibrary::KillTween(ConvertToTweenHandle(In));
	}

	/* Querying - thin wrappers from UTypeTweenLibrary for UX and ease of use */
	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Vector")
	static bool IsValid(const FTweenVectorHandle& In) {
		return UTypeTweenLibrary::IsValid(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Vector")
	static bool IsDone(const FTweenVectorHandle& In) {
		return UTypeTweenLibrary::IsDone(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Vector")
	static bool IsPlaying(const FTweenVectorHandle& In) {
		return UTypeTweenLibrary::IsPlaying(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Vector")
	static bool IsPaused(const FTweenVectorHandle& In) {
		return UTypeTweenLibrary::IsPaused(ConvertToTweenHandle(In));
	}
};
