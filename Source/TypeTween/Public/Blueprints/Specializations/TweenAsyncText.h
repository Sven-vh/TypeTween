// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "CoreMinimal.h"
#include "Blueprints/TweenAsyncBase.h"
#include "TypeTween.h"
#include "Tools/TextLerps.h"
#include "Blueprints/TweenFunctionLibrary.h"
#include "TweenAsyncText.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTextTweenUpdate, FText, CurrentValue);

USTRUCT(BlueprintType)
struct FTweenTextSettings : public FTweenSettings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	FText From = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	FText To = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	ETextLerpMode LerpMode = ETextLerpMode::Scramble;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween")
	ETextGlyphSet GlyphSet = ETextGlyphSet::Alphanumeric;

	/** Only used when GlyphSet is set to Custom. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TypeTween",
		meta = (EditCondition = "GlyphSet == ETextGlyphSet::Custom", EditConditionHides))
	FString CustomGlyphs;

	FTweenTextSettings& operator=(const FTweenSettings& Other) {
		FTweenSettings::operator=(Other); // copies Duration, Ease, RepeatCount, etc.
		return *this;
	}
};

USTRUCT(BlueprintType)
struct FTweenTextHandle {
	GENERATED_BODY()

	TypeTween::TTweenHandle<FText> Handle;
};

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncTextBase : public UTweenAsyncBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnTextTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenTextSettings TweenSettings;

	TypeTween::TTweenWeakHandle<FText> TweenHandle;

	FORCEINLINE void CallOnUpdate(const FText& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

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

		TweenHandle->From(TweenSettings.From)
			.To(TweenSettings.To)
			.Mode(TweenSettings.LerpMode)
			.GlyphSet(TweenSettings.GlyphSet, TweenSettings.CustomGlyphs)
			.Preset(TweenSettings)
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

		ActivateAdvanced(*TweenHandle.ToShared());
	}
};

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
		FTweenTextSettings Tween,
		FTweenTextHandle& handle
	) {
		UTweenAsyncText* Node = NewObject<UTweenAsyncText>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenSettings = Tween;
		Node->TweenHandle = TypeTween::Tween<FText>(InWorldContextObject);
		Node->RegisterWithGameInstance(InWorldContextObject);

		/* output */
		handle.Handle = Node->TweenHandle.ToShared();

		return Node;
	}
};

/* conversion function library */
UCLASS()
class TYPETWEEN_API UTweenTextFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TypeTween|Config|Text")
	static FTweenTextSettings GetSettings(const FTweenTextHandle& In) {
		if (!ensureMsgf(In.Handle, TEXT("GetSettings: Input handle has no tween (nullptr)!"))) {
			return {};
		}

		FTweenTextSettings Settings;
		Settings = In.Handle->GetSettings();
		Settings.From = FText::FromString(In.Handle->GetStart().Get(FString()));
		Settings.To = FText::FromString(In.Handle->GetEnd().Get(FString()));
		return Settings;
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Config|Text")
	static void SetSettings(UPARAM(ref) FTweenTextHandle& In, FTweenTextSettings Settings) {
		if (!ensureMsgf(In.Handle, TEXT("SetSettings: Input handle has no tween (nullptr)!"))) {
			return;
		}
		In.Handle->From(Settings.From)
			.To(Settings.To)
			.Mode(Settings.LerpMode)
			.GlyphSet(Settings.GlyphSet, Settings.CustomGlyphs)
			.Preset(Settings);
	}

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "->"), Category = "TypeTween|Conversions")
	static FTweenHandle ConvertToTweenHandle(const FTweenTextHandle& In) {
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
	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Text")
	static void PauseTween(UPARAM(ref) FTweenTextHandle& In) {
		UTypeTweenLibrary::PauseTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Text")
	static void ResumeTween(UPARAM(ref) FTweenTextHandle& In) {
		UTypeTweenLibrary::ResumeTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Text")
	static void RestartTween(UPARAM(ref) FTweenTextHandle& In) {
		UTypeTweenLibrary::RestartTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Text")
	static void FinishTween(UPARAM(ref) FTweenTextHandle& In) {
		UTypeTweenLibrary::FinishTween(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintCallable, Category = "TypeTween|Control|Text")
	static void KillTween(UPARAM(ref) FTweenTextHandle& In) {
		UTypeTweenLibrary::KillTween(ConvertToTweenHandle(In));
	}

	/* Querying - thin wrappers from UTypeTweenLibrary for UX and ease of use */
	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Text")
	static bool IsValid(const FTweenTextHandle& In) {
		return UTypeTweenLibrary::IsValid(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Text")
	static bool IsDone(const FTweenTextHandle& In) {
		return UTypeTweenLibrary::IsDone(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Text")
	static bool IsPlaying(const FTweenTextHandle& In) {
		return UTypeTweenLibrary::IsPlaying(ConvertToTweenHandle(In));
	}

	UFUNCTION(BlueprintPure, Category = "TypeTween|Control|Text")
	static bool IsPaused(const FTweenTextHandle& In) {
		return UTypeTweenLibrary::IsPaused(ConvertToTweenHandle(In));
	}
};