// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
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


public:
	// -- Display ---------------------------------------------------------------

	/* https://www.alt-codes.net/triangle-symbols */
	virtual FText GetTooltipText() const override {
		return NSLOCTEXT("TypeTween", "Tween_Tooltip",
			"Tweens a value from [From] to [To].\n"
			"Expand ? for loop, delay, and lifecycle event pins.\n"
			"Right-click any input pin to Recombine or Promote to Variable.");
	}

	// -- Pin allocation --------------------------------------------------------

	virtual void AllocateDefaultPins() override;

	virtual void PostPlacedNewNode() override;

	virtual void PostReconstructNode() override;

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& Reg) const override;

	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;

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
