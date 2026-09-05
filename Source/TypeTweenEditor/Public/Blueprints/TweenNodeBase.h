// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "CoreMinimal.h"
#include "K2Node_AsyncAction.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_AddPinInterface.h"

#include "Blueprints/Generated/TweenAsyncFloat.h"
#include "Blueprints/Specializations/TweenAsyncColor.h"

#include "TweenNodeBase.generated.h"

class FKismetCompilerContext;

/* Abstract only but can't be marked as abstract due to Unreal's UCLASS limitations */
UCLASS()
class TYPETWEENEDITOR_API UK2Node_Tween : public UK2Node_AsyncAction, public IK2Node_AddPinInterface {
	GENERATED_BODY()

	// -- Advanced output delegate pins hidden behind \/ ------------------------

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

	// -- "To" waypoint array -----------------------------------------------
	// FTween<Type>Settings::To is a TArray<T>, but we don't want a raw array
	// pin in the graph. Instead we expose N scalar "Tween_To_<i>" pins (like
	// Make Array does) and reassemble them into the array at compile time.
	//
	// NumToPins is a serialized UPROPERTY (not just editor-session state)
	// so the pin *count* survives an asset reload on its own, independent of
	// pin-reconstruction timing. This matters because Unreal reattaches old
	// pin links to newly-allocated pins *by name* before PostReconstructNode
	// ever runs, so these pins must already exist - with the right names -
	// by the time AllocateDefaultPins() returns. See the .cpp for details.
	UPROPERTY()
	int32 NumToPins = 1;

	void CreateToElementPins();

	bool GetToElementPinType(const UEdGraphPin* TweenPin, FEdGraphPinType& OutType) const;

	/* Tween_To_N pins are created in AllocateDefaultPins() (see comment
	   there), which runs before "Tween_From" exists (that's split off later,
	   in EnsureSplitState()), so they end up appended at the very end of the
	   Pins array. This moves them to sit right after "Tween_From" instead,
	   which is where they visually belong. Only called when "Tween" is
	   currently split (see EnsureToPinsState). */
	void ReorderToPins();

	/* Hides the real "Tween_To" split pin and keeps Tween_To_N positioned
	   and visible/hidden correctly. Unlike EnsureSplitState()'s guarded
	   splits (which only ever run once, so the user's "Recombine to base
	   pin" choice sticks across reconstruction), this must re-run on every
	   reconstruction: bHidden doesn't persist on its own, since pins are
	   recreated fresh each time. If the user has recombined "Tween" back
	   into one struct pin, Tween_To_N is hidden too, since nothing consumes
	   it in that state - the user is expected to feed the whole struct
	   (including "To") themselves at that point, e.g. via Make Struct. */
	void EnsureToPinsState();

public:
	// -- Display ---------------------------------------------------------------

	/* https://www.alt-codes.net/triangle-symbols */
	virtual FText GetTooltipText() const override {
		return NSLOCTEXT("TypeTween", "Tween_Tooltip",
			"Tweens a value from [From] to [To].\n"
			"Right-click [To] to Add/Remove waypoint pins.\n"
			"Expand ? for loop, delay, and lifecycle event pins.\n"
			"Right-click any input pin to Recombine or Promote to Variable.");
	}

	// -- Pin allocation --------------------------------------------------------

	virtual void AllocateDefaultPins() override;

	/* Migrates old saved graphs: before "To" became an array, this node had
	   a single scalar pin literally named "Tween_To". That name still exists
	   on the node today (now hidden, now array-typed), so Unreal's default
	   by-name pin matching would bind the old scalar data straight onto it -
	   a type mismatch ("Internal connection error"). GetRedirectPinNames is
	   only consulted when no exact name match exists, so it never fires here.
	   This override runs first and relabels the legacy old pin so it lands
	   on "Tween_To_0" instead. */
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;

	virtual void PostPlacedNewNode() override;

	virtual void PostReconstructNode() override;

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& Reg) const override;

	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;

	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	/* Lets old saved graphs (from before "To" became an array) keep their
	   wire: the old single "Tween_To" pin now maps onto "Tween_To_0". */
	virtual void GetRedirectPinNames(const UEdGraphPin& Pin, TArray<FString>& RedirectPinNames) const override;

	// -- IK2Node_AddPinInterface -------------------------------------------

	virtual void AddInputPin() override;

	virtual void RemoveInputPin(UEdGraphPin* Pin) override;

	virtual bool CanAddPin() const override;

	virtual bool CanRemovePin(const UEdGraphPin* Pin) const override;

protected:
	/* Optional splits pins for types */
	virtual void CustomizedSplits() {};

	/* Optional advanced for types */
	virtual void CustomizedAdvanced() {};

private:

	void EnsureSplitState();

	void EnsureHiddenState();

	void EnsureAdvancedView();
};
