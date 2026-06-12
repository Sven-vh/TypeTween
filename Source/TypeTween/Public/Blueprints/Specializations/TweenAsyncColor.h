// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "CoreMinimal.h"
#include "Blueprints/TweenAsyncBase.h"
#include "TypeTween.h"
#include "Blueprints/TweenFunctionLibrary.h"
#include "TweenAsyncColor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorTweenUpdate, FLinearColor, CurrentValue);

USTRUCT(BlueprintType)
struct FTweenColorSettings : public FTweenSettings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	FLinearColor From = FLinearColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	FLinearColor To = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	EColorLerpMode ColorSpace = EColorLerpMode::Linear;

	FTweenColorSettings& operator=(const FTweenSettings& Other) {
		FTweenSettings::operator=(Other); // copies Duration, Ease, RepeatCount, etc.
		return *this;
	}
};

USTRUCT(BlueprintType)
struct FTweenColorHandle {
	GENERATED_BODY()

	TypeTween::TTweenHandle<FLinearColor> Handle;
};

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncColorBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnColorTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenColorSettings TweenSettings;

	TypeTween::TTweenWeakHandle<FLinearColor> TweenHandle;

	FORCEINLINE void CallOnUpdate(const FLinearColor& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncColor : public UTweenAsyncColorBase {
	GENERATED_BODY()

	friend class UTweenAsyncColorFactory;

protected:
	virtual void Activate() override {
		if (!WorldContextObject) {
			SetReadyToDestroy();
			return;
		}

		TweenHandle->From(TweenSettings.From)
			.To(TweenSettings.To)
			.ColorSpace(TweenSettings.ColorSpace)
			.Preset(TweenSettings)
			.OnUpdate(
				[this](float /*Alpha*/, const FLinearColor& CurrentValue) {
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
class TYPETWEEN_API UTweenAsyncColorFactory : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Linear Color"
			))
	static UTweenAsyncColor* TweenColor(
		UObject* InWorldContextObject,
		FTweenColorSettings Tween,
		FTweenColorHandle& handle
	) {
		UTweenAsyncColor* Node = NewObject<UTweenAsyncColor>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenSettings = Tween;
		Node->TweenHandle = TypeTween::Tween<FLinearColor>(InWorldContextObject);
		Node->RegisterWithGameInstance(InWorldContextObject);

		/* output */
		handle.Handle = Node->TweenHandle.ToShared();

		return Node;
	}
};

/* conversion function library */
UCLASS()
class TYPETWEEN_API UTweenColorFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TypeTween|Config|Color")
	static FTweenColorSettings GetSettings(const FTweenColorHandle& In) {
		if (!ensureMsgf(In.Handle, TEXT("GetSettings: Input handle has no tween (nullptr)!"))) {
			return {};
		}

		FTweenColorSettings Settings;
		Settings = In.Handle->GetSettings();
		Settings.From = In.Handle->GetStart().Get(FLinearColor::Black);
		Settings.To = In.Handle->GetEnd().Get(FLinearColor::White);
		return Settings;
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Config|Color")
	static void SetSettings(UPARAM(ref) FTweenColorHandle& In, FTweenColorSettings Settings) {
		if (!ensureMsgf(In.Handle, TEXT("SetSettings: Input handle has no tween (nullptr)!"))) {
			return;
		}
		In.Handle->From(Settings.From)
			.To(Settings.To)
			.ColorSpace(Settings.ColorSpace)
			.Preset(Settings);
	}

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "->"), Category = "TypeTween|Conversions")
	static FTweenHandle ConvertToTweenHandle(const FTweenColorHandle& In) {
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
	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Color")
	static void PauseTween(UPARAM(ref) FTweenColorHandle& In) {
		UTypeTweenLibrary::PauseTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Color")
	static void ResumeTween(UPARAM(ref) FTweenColorHandle& In) {
		UTypeTweenLibrary::ResumeTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Color")
	static void RestartTween(UPARAM(ref) FTweenColorHandle& In) {
		UTypeTweenLibrary::RestartTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Color")
	static void FinishTween(UPARAM(ref) FTweenColorHandle& In) {
		UTypeTweenLibrary::FinishTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Color")
	static void KillTween(UPARAM(ref) FTweenColorHandle& In) {
		UTypeTweenLibrary::KillTween(ConvertToTweenHandle(In));
	}

	/* Querying - thin wrappers from UTypeTweenLibrary for UX and ease of use */
	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Color")
	static bool IsValid(const FTweenColorHandle& In) {
		return UTypeTweenLibrary::IsValid(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Color")
	static bool IsDone(const FTweenColorHandle& In) {
		return UTypeTweenLibrary::IsDone(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Color")
	static bool IsPlaying(const FTweenColorHandle& In) {
		return UTypeTweenLibrary::IsPlaying(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Color")
	static bool IsPaused(const FTweenColorHandle& In) {
		return UTypeTweenLibrary::IsPaused(ConvertToTweenHandle(In));
	}
};