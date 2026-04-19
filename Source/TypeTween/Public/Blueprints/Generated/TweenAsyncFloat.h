// ============================================================
// FILE: Public/Blueprints/Generated/TweenAsyncFloat.h
//
// Async nodes + combined K2 node for float tweening.
//
// Runtime classes (UTweenAsyncFloat*) are generated — to modify
// them edit Scripts/TweenAsyncType.h.template and re-run
// Scripts/generate_tween_types.py.
//
// UK2Node_TweenFloat is hand-authored below the WITH_EDITOR
// guard and should be maintained manually.
// ============================================================
#pragma once
#include "CoreMinimal.h"
#include "Blueprints/TweenAsyncBase.h"
#include "TypeTween.h"

#if WITH_EDITOR
#include "K2Node_AsyncAction.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "EdGraphSchema_K2.h"
#endif // WITH_EDITOR

#include "TweenAsyncFloat.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatTweenUpdate, float, CurrentValue);

// ─────────────────────────────────────────────────────────────
// Config struct — exposed to Blueprint details panel
// ─────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FTweenFloatConfig : public FTweenSettingsConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float From = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float To = 1.f;
};

// ─────────────────────────────────────────────────────────────
// Abstract base (Advanced) — OnUpdate + config + all events
// ─────────────────────────────────────────────────────────────

UCLASS(Abstract, BlueprintType)
class TYPETWEEN_API UTweenAsyncFloatBaseAdvanced : public UTweenAsyncBaseAdvanced {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tweening|Events")
	FOnFloatTweenUpdate OnUpdate;

protected:
	UPROPERTY()
	FTweenFloatConfig TweenConfig;

	FORCEINLINE void CallOnUpdate(const float& CurrentValue) {
		if (OnUpdate.IsBound()) {
			OnUpdate.Broadcast(CurrentValue);
		}
	}
};

// ─────────────────────────────────────────────────────────────
// Concrete async node (Advanced) — all events
// ─────────────────────────────────────────────────────────────

UCLASS(meta = (HideCategories = Object))
class TYPETWEEN_API UTweenAsyncFloatAdvanced : public UTweenAsyncFloatBaseAdvanced {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TypeTween",
		meta = (
			BlueprintInternalUseOnly = "true",
			WorldContext = "InWorldContextObject",
			DefaultToSelf = "InWorldContextObject",
			DisplayName = "Tween Float (Advanced)",
			ToolTip = "Tweens a float from [From] to [To]."
			))
	static UTweenAsyncFloatAdvanced* TweenFloatAdvanced(
		UObject* InWorldContextObject,
		FTweenFloatConfig Tween
	) {
		UTweenAsyncFloatAdvanced* Node = NewObject<UTweenAsyncFloatAdvanced>();
		Node->WorldContextObject = InWorldContextObject;
		Node->TweenConfig = Tween;
		Node->RegisterWithGameInstance(InWorldContextObject);
		return Node;
	}

protected:
	virtual void Activate() override {
		if (!WorldContextObject) {
			SetReadyToDestroy();
			return;
		}

		const FTweenSettings Settings = TweenConfig.Resolve();

		auto& Tween = TypeTween::Tween<float>(WorldContextObject)
			.From(TweenConfig.From)
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

		ActivateAdvanced(Tween);
	}
};

// ─────────────────────────────────────────────────────────────
// Combined Blueprint K2 node
//
// One "Tween Float" node with struct-split input pins and all
// lifecycle callbacks hidden behind the ▼ Advanced expander.
//
// Visible inputs:   From, To, Duration, Ease
// Advanced inputs:  RepeatCount, LoopMode, Delays
// Visible outputs:  Then, OnUpdate (+CurrentValue), OnComplete
// Advanced outputs: OnStart, OnCycleBegin, OnForwardEnd,
//                   OnReverseBegin, OnCycleEnd, OnRepeat
//
// Splitting uses Schema->SplitPin so "Recombine Struct Pin" and
// "Promote to Variable" continue to work exactly as they do on
// any other Blueprint struct pin.
//
// ExpandNode is intentionally NOT overridden.  The Kismet
// compiler calls ExpandSplitPins before ExpandNode, so the base
// class sees a fully-connected Tween struct pin and routes it to
// TweenFloatAdvanced correctly.
//
// SETUP: compile this inside your *Editor* module.
// Build.cs dependencies: TypeTween, BlueprintGraph, UnrealEd
// ─────────────────────────────────────────────────────────────

#if WITH_EDITOR

UCLASS()
class TYPETWEEN_API UK2Node_TweenFloat : public UK2Node_AsyncAction {
	GENERATED_BODY()

	// ── Advanced output delegate pins hidden behind ▼ ────────────────────────

	static TArrayView<const FName> AdvancedDelegatePins() {
		static const FName Names[] = {
			FName(TEXT("OnStart")),
			FName(TEXT("OnCycleBegin")),
			FName(TEXT("OnForwardEnd")),
			FName(TEXT("OnReverseBegin")),
			FName(TEXT("OnCycleEnd")),
			FName(TEXT("OnRepeat")),
		};
		return MakeArrayView(Names);
	}


public:
	// ── Construction ──────────────────────────────────────────────────────────

	UK2Node_TweenFloat() {
		ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UTweenAsyncFloatAdvanced, TweenFloatAdvanced);
		ProxyFactoryClass = UTweenAsyncFloatAdvanced::StaticClass();
		ProxyClass = UTweenAsyncFloatAdvanced::StaticClass();
	}

	// ── Display ───────────────────────────────────────────────────────────────

	virtual FText GetNodeTitle(ENodeTitleType::Type /*TitleType*/) const override {
		return NSLOCTEXT("TypeTween", "TweenFloat_Title", "Tween Float");
	}

	virtual FText GetTooltipText() const override {
		return NSLOCTEXT("TypeTween", "TweenFloat_Tooltip",
			"Tweens a float from [From] to [To].\n"
			"Expand ▼ for loop, delay, and lifecycle event pins.\n"
			"Right-click any input pin to Recombine or Promote to Variable.");
	}

	virtual FText GetMenuCategory() const override {
		return NSLOCTEXT("TypeTween", "TweenFloat_Category", "TypeTween");
	}

	// ── Pin allocation ────────────────────────────────────────────────────────

	virtual void AllocateDefaultPins() override {
		Super::AllocateDefaultPins();

		// Mark the advanced output delegate pins that Super created.
		// The input sub-pins don't exist yet (splitting happens in
		// PostPlacedNewNode / PostReconstructNode), so they're handled there.
		for (const FName& Name : AdvancedDelegatePins())
			if (UEdGraphPin* P = FindPin(Name))
				P->bAdvancedView = true;

		AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
	}

	virtual void PostPlacedNewNode() override {
		Super::PostPlacedNewNode();
		EnsureSplitState();
	}

	virtual void PostReconstructNode() override {
		Super::PostReconstructNode();
		EnsureSplitState();
	}

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& Reg) const override {
		UClass* Cls = GetClass();
		if (Reg.IsOpenForRegistration(Cls))
			Reg.AddBlueprintAction(Cls, UBlueprintNodeSpawner::Create(Cls));
	}

private:

	void EnsureSplitState() {
		const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();

		// Level 1: FTweenFloatConfig → From, To, Settings
		if (UEdGraphPin* P = FindPin(TEXT("Tween")))
			if (P->SubPins.Num() == 0)
				Schema->SplitPin(P);

		// Level 2: FTweenSettings → Duration, Ease, RepeatCount, LoopMode, Delays
		if (UEdGraphPin* P = FindPin(TEXT("Tween_Settings")))
			if (P->SubPins.Num() == 0)
				Schema->SplitPin(P);

		// Level 3: FTweenDelays → Start, Reverse, Repeat, End
		// Pre-splitting here means individual float pins are already visible
		// when ▼ is opened, rather than a blob the user would need to split.
		if (UEdGraphPin* P = FindPin(TEXT("Tween_Settings_Delays")))
			if (P->SubPins.Num() == 0)
				Schema->SplitPin(P);

		// Mark all Tween_Settings_* sub-pins as advanced except Duration and Ease.
		// Prefix walk means Delays sub-pins (Tween_Settings_Delays_*) are caught
		// automatically without maintaining an explicit list.
		for (UEdGraphPin* P : Pins) {
			const FString Name = P->PinName.ToString();
			if (!Name.StartsWith(TEXT("Tween_Settings_"))) { continue; }
			if (Name == TEXT("Tween_Settings_Duration")) { continue; }
			if (Name == TEXT("Tween_Settings_Ease")) { continue; }
			P->bAdvancedView = true;
		}
	}
};

#endif // WITH_EDITOR