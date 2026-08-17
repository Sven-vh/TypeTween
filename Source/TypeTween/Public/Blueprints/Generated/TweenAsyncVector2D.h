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
#include "TweenAsyncVector2D.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVector2DTweenUpdate, FVector2D, CurrentValue);

USTRUCT(BlueprintType)
struct FTweenVector2DSettings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	FVector2D From = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	TArray<FVector2D> To = {FVector2D::UnitVector};

	/* Common tween settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween|Settings")
	FTweenSettings Settings;
};

USTRUCT(BlueprintType)
struct FTweenVector2DHandle {
	GENERATED_BODY()

	TypeTween::TTweenHandle<FVector2D> Handle;
};

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncVector2DBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "TypeTween|Events")
	FOnVector2DTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenVector2DSettings TweenSettings;

	TypeTween::TTweenWeakHandle<FVector2D> TweenHandle;

	FORCEINLINE void CallOnUpdate(const FVector2D& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncVector2D : public UTweenAsyncVector2DBase {
	GENERATED_BODY()

	friend class UTweenAsyncVector2DFactory;

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
				[this](float /*Alpha*/, const FVector2D& CurrentValue) {
					CallOnUpdate(CurrentValue);
				}
			);

		ActivateAdvanced(*TweenHandle.ToShared());
	}
};

UCLASS()
class TYPETWEEN_API UTweenAsyncVector2DFactory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Vector2D"
			))
	static UTweenAsyncVector2D* TweenVector2D(
		UObject* InWorldContextObject,
		FTweenVector2DSettings Tween,
		FTweenVector2DHandle& handle
	) {
		UTweenAsyncVector2D* Node = NewObject<UTweenAsyncVector2D>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenSettings = Tween;
		Node->TweenHandle = TypeTween::Tween<FVector2D>(InWorldContextObject);
		Node->RegisterWithGameInstance(InWorldContextObject);

		/* output */
		handle.Handle = Node->TweenHandle.ToShared();

		return Node;
	}
};

/* conversion function library */
UCLASS()
class TYPETWEEN_API UTweenVector2DFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TypeTween|Types|Vector2D")
	static FTweenVector2DSettings GetVector2DSettings(const FTweenVector2DHandle& In) {
		if (!ensureMsgf(In.Handle, TEXT("GetSettings: Input handle has no tween (nullptr)!"))) {
			return {};
		}

		FTweenVector2DSettings Settings;
		Settings.From = In.Handle->GetStart().Get(FVector2D::ZeroVector);
		Settings.To = In.Handle->GetWaypoints();
		Settings.Settings = In.Handle->GetSettings();
		return Settings;
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Types|Vector2D")
	static void SetVector2DSettings(UPARAM(ref) FTweenVector2DHandle& In, FTweenVector2DSettings Settings) {
		if (!ensureMsgf(In.Handle, TEXT("SetSettings: Input handle has no tween (nullptr)!"))) {
			return;
		}
		In.Handle->From(Settings.From)
			.To(Settings.To)
			.Preset(Settings.Settings);
	}

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "->"), Category = "TypeTween|Types|Vector2D")
	static FTweenHandle ConvertToTweenHandle(const FTweenVector2DHandle& In) {
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
	UFUNCTION(BlueprintCallable, Category = "TypeTween|Types|Vector2D")
	static void PauseTween(UPARAM(ref) FTweenVector2DHandle& In) {
		UTypeTweenLibrary::PauseTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Types|Vector2D")
	static void ResumeTween(UPARAM(ref) FTweenVector2DHandle& In) {
		UTypeTweenLibrary::ResumeTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Types|Vector2D")
	static void RestartTween(UPARAM(ref) FTweenVector2DHandle& In) {
		UTypeTweenLibrary::RestartTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Types|Vector2D")
	static void FinishTween(UPARAM(ref) FTweenVector2DHandle& In) {
		UTypeTweenLibrary::FinishTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Types|Vector2D")
	static void KillTween(UPARAM(ref) FTweenVector2DHandle& In) {
		UTypeTweenLibrary::KillTween(ConvertToTweenHandle(In));
	}

	/* Querying - thin wrappers from UTypeTweenLibrary for UX and ease of use */
	UFUNCTION(BlueprintPure, Category = "TypeTween|Types|Vector2D")
	static bool IsValid(const FTweenVector2DHandle& In) {
		return UTypeTweenLibrary::IsValid(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Types|Vector2D")
	static bool IsDone(const FTweenVector2DHandle& In) {
		return UTypeTweenLibrary::IsDone(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Types|Vector2D")
	static bool IsPlaying(const FTweenVector2DHandle& In) {
		return UTypeTweenLibrary::IsPlaying(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Types|Vector2D")
	static bool IsPaused(const FTweenVector2DHandle& In) {
		return UTypeTweenLibrary::IsPaused(ConvertToTweenHandle(In));
	}
};
