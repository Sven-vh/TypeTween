#pragma once
#include "CoreMinimal.h"
#include "K2Node_AsyncAction.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "EdGraphSchema_K2.h"

#include "Blueprints/Generated/TweenAsyncFloat.h"
#include "Blueprints/Specializations/TweenAsyncColor.h"

#include "TweenNodeBase.generated.h"

/* Abstract only but can't be marked as abstract due to Unreal's UCLASS limitations */
UCLASS()
class TYPETWEENEDITOR_API UK2Node_Tween : public UK2Node_AsyncAction {
	GENERATED_BODY()

	// ── Advanced output delegate pins hidden behind \/ ────────────────────────

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
	// ── Display ───────────────────────────────────────────────────────────────

	/* https://www.alt-codes.net/triangle-symbols */
	virtual FText GetTooltipText() const override {
		return NSLOCTEXT("TypeTween", "Tween_Tooltip",
			"Tweens a value from [From] to [To].\n"
			"Expand ▼ for loop, delay, and lifecycle event pins.\n"
			"Right-click any input pin to Recombine or Promote to Variable.");
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
		/* Only register concrete subclasses, never the base itself */
		if (Cls == UK2Node_Tween::StaticClass()) { return; }
		if (Reg.IsOpenForRegistration(Cls))
			Reg.AddBlueprintAction(Cls, UBlueprintNodeSpawner::Create(Cls));
	}

private:

	void EnsureSplitState() {
		const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();

		// Level 1: FTweenFloatConfig  From, To, Settings
		if (UEdGraphPin* P = FindPin(TEXT("Tween")))
			if (P->SubPins.Num() == 0)
				Schema->SplitPin(P);

		// Level 2: FTweenSettings  Duration, Ease, RepeatCount, LoopMode, Delays
		if (UEdGraphPin* P = FindPin(TEXT("Tween_Settings")))
			if (P->SubPins.Num() == 0)
				Schema->SplitPin(P);

		// Level 3: FTweenDelays Start, Reverse, Repeat, End
		// Pre-splitting here means individual float pins are already visible
		// when \/ is opened, rather than a blob the user would need to split.
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