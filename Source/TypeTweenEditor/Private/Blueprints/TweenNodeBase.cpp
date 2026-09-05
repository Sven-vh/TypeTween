#include "Blueprints/TweenNodeBase.h"
#include "KismetCompiler.h"
#include "K2Node_MakeArray.h"
#include "Kismet2/BlueprintEditorUtils.h"

void UK2Node_Tween::AllocateDefaultPins() {
	Super::AllocateDefaultPins();

	// Mark the advanced output delegate pins that Super created.
	for (const FName& Name : AdvancedDelegatePins())
		if (UEdGraphPin* P = FindPin(Name))
			P->bAdvancedView = true;

	AdvancedPinDisplay = ENodeAdvancedPins::Hidden;

	// Create the "Tween_To_0.. NumToPins-1" pins here, not in
	// PostPlacedNewNode/PostReconstructNode. Unreal's own node-reconstruction
	// flow (UK2Node::ReconstructNode -> ReallocatePinsDuringReconstruction)
	// calls AllocateDefaultPins() and then re-attaches the *old* node's pin
	// links onto the newly-allocated pins by matching names, all *before*
	// PostReconstructNode() is ever called. If these pins were only created
	// in PostPlacedNewNode/PostReconstructNode (as "Tween" itself's real
	// split sub-pins are), any wire the user had plugged into Tween_To_0 (or
	// Tween_To_1, etc.) would silently vanish the next time the graph is
	// reloaded or the node is reconstructed, because the pins wouldn't exist
	// yet when the by-name rewiring pass runs.
	//
	// We can determine the correct pin type for "Tween_To_N" directly from
	// the (still unsplit, at this point) "Tween" pin's struct type, via
	// reflection on its "To" TArray property's element type - so we don't
	// need "Tween_From" or the split hierarchy to exist yet.
	CreateToElementPins();
}

void UK2Node_Tween::CreateToElementPins() {
	UEdGraphPin* TweenPin = FindPin(TEXT("Tween"));
	if (!TweenPin) { return; }

	FEdGraphPinType ElementPinType;
	if (!GetToElementPinType(TweenPin, ElementPinType)) { return; }

	NumToPins = FMath::Max(NumToPins, 1);
	for (int32 i = 0; i < NumToPins; ++i) {
		const FName PinName(*FString::Printf(TEXT("Tween_To_%d"), i));

		// The internal FName stays "Tween_To_0", "Tween_To_1", ... (needed
		// for stable lookups elsewhere), but the first pin *displays* as
		// plain "Tween To", matching the pre-array UX; only pin 1+ show a
		// number, since they only exist once the user has actually added
		// extra waypoints.
		const FText FriendlyName = (i == 0)
			? NSLOCTEXT("TypeTween", "Tween_To_First", "Tween To")
			: FText::Format(NSLOCTEXT("TypeTween", "Tween_To_Nth", "Tween To {0}"), FText::AsNumber(i));

		UEdGraphPin* Pin = FindPin(PinName);
		if (!Pin) {
			Pin = CreatePin(EGPD_Input, ElementPinType, PinName);
		}
		Pin->PinFriendlyName = FriendlyName;
	}
}

bool UK2Node_Tween::GetToElementPinType(const UEdGraphPin* TweenPin, FEdGraphPinType& OutType) const {
	const UScriptStruct* TweenStruct = Cast<UScriptStruct>(TweenPin->PinType.PinSubCategoryObject.Get());
	if (!TweenStruct) { return false; }

	const FArrayProperty* ToProp = CastField<FArrayProperty>(TweenStruct->FindPropertyByName(TEXT("To")));
	if (!ToProp || !ToProp->Inner) { return false; }

	const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
	return Schema->ConvertPropertyToPinType(ToProp->Inner, OutType);
}

void UK2Node_Tween::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) {
	// Find a pre-array-"To" save: a pin literally named "Tween_To" whose
	// type is NOT a container. Today's "Tween_To" (the real, hidden split
	// pin) is TArray<T>, so this distinguishes an old scalar save from the
	// current pin, regardless of GetRedirectPinNames (which never gets a
	// chance to run, since "Tween_To" still exists as an exact name match).
	//
	// Renaming it here, on the *old* (about to be discarded/matched-away)
	// pin object, is safe: it's not part of the live node's pin bookkeeping,
	// it's just data Unreal is about to use to rewire the freshly-allocated
	// new pins by name. After the rename, it matches "Tween_To_0" exactly -
	// same element type, so the link or default value transfers cleanly.
	for (UEdGraphPin* OldPin : OldPins) {
		if (OldPin
			&& OldPin->PinName == TEXT("Tween_To")
			&& OldPin->PinType.ContainerType != EPinContainerType::Array) {
			OldPin->PinName = TEXT("Tween_To_0");
			break;
		}
	}

	Super::ReallocatePinsDuringReconstruction(OldPins);
}

void UK2Node_Tween::PostPlacedNewNode() {
	Super::PostPlacedNewNode();
	EnsureSplitState();
	EnsureToPinsState();
	EnsureAdvancedView();
	EnsureHiddenState();
}

void UK2Node_Tween::PostReconstructNode() {
	Super::PostReconstructNode();
	// Only EnsureToPinsState() re-runs here, not EnsureSplitState() - the
	// latter's guarded splits are meant to fire once, so a user's later
	// "Recombine to base pin" choice persists across reconstruction.
	EnsureToPinsState();
	EnsureAdvancedView();
	EnsureHiddenState();
}

void UK2Node_Tween::GetMenuActions(FBlueprintActionDatabaseRegistrar& Reg) const {
	UClass* Cls = GetClass();
	/* Only register concrete subclasses, never the base itself */
	if (Cls == UK2Node_Tween::StaticClass()) { return; }
	if (Reg.IsOpenForRegistration(Cls))
		Reg.AddBlueprintAction(Cls, UBlueprintNodeSpawner::Create(Cls));
}

void UK2Node_Tween::PinDefaultValueChanged(UEdGraphPin* Pin) {
	Super::PinDefaultValueChanged(Pin);

	static const FName Names[] = {
		TEXT("Tween_Settings_Ease"),
		TEXT("Tween_Settings_RepeatCount"),
		TEXT("Tween_Settings_LoopMode"),
	};

	for (const FName& Name : Names) {
		if (Pin->PinName == Name) {
			EnsureHiddenState();
			if (UEdGraph* Graph = GetGraph()) {
				Graph->NotifyGraphChanged();
			}
		}
	}
}

void UK2Node_Tween::EnsureSplitState() {
	const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();

	// Level 1: FTweenFloatConfig  From, To, Settings
	// These guards only ever fire once (called from PostPlacedNewNode only).
	// After that, the user is free to right-click "Recombine to base pin"
	// on any of these and have it stick across reconstruction - exactly
	// like any other split struct pin in Blueprint. (Previously this ran
	// from PostReconstructNode too, which force-re-split them on every
	// reconstruction and made recombine appear to silently do nothing.)
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

	CustomizedSplits();
}

void UK2Node_Tween::EnsureToPinsState() {
	// "Tween_To" only exists while "Tween" is actually split. If the user
	// recombined "Tween" back into one struct pin, this is null - in that
	// case there's nothing to hide/reorder, and ExpandNode() already
	// no-ops gracefully (FindPin("Tween_To") returns null there too), so
	// Super::ExpandNode() just treats "Tween" as an ordinary input pin.
	UEdGraphPin* ArrayPin = FindPin(TEXT("Tween_To"));
	const bool bToIsSplit = (ArrayPin != nullptr);

	if (ArrayPin) {
		ArrayPin->bHidden = true;
	}

	// Keep Tween_To_N in sync with whether "To" is actually usable right
	// now: visible (and positioned right after Tween_From) while split,
	// hidden - since nothing would consume them - once recombined.
	int32 Index = 0;
	while (UEdGraphPin* P = FindPin(FName(*FString::Printf(TEXT("Tween_To_%d"), Index)))) {
		P->bHidden = !bToIsSplit;
		++Index;
	}

	if (bToIsSplit) {
		ReorderToPins();
	}
}

void UK2Node_Tween::ReorderToPins() {
	UEdGraphPin* FromPin = FindPin(TEXT("Tween_From"));
	if (!FromPin) { return; }

	int32 InsertIndex = Pins.IndexOfByKey(FromPin) + 1;

	int32 Index = 0;
	while (UEdGraphPin* P = FindPin(FName(*FString::Printf(TEXT("Tween_To_%d"), Index)))) {
		const int32 CurrentIndex = Pins.IndexOfByKey(P);
		if (CurrentIndex != INDEX_NONE && CurrentIndex != InsertIndex) {
			Pins.RemoveAt(CurrentIndex);
			if (CurrentIndex < InsertIndex) { --InsertIndex; }
			Pins.Insert(P, InsertIndex);
		}
		++InsertIndex;
		++Index;
	}
}

void UK2Node_Tween::EnsureHiddenState() {
	// when ease != CustomCurve:
	//		hide CustomEaseCurve
	const UEdGraphPin* EasePin = FindPin(TEXT("Tween_Settings_Ease"));
	if (EasePin) {
		const UEnum* EaseEnum = StaticEnum<ETweenEase>();
		if (EaseEnum) {
			const FString& EaseValue = EasePin->DefaultValue.IsEmpty()
				? EasePin->AutogeneratedDefaultValue
				: EasePin->DefaultValue;
			const int64 EnumValue = EaseEnum->GetValueByNameString(EaseValue);
			const ETweenEase CurrentEase = static_cast<ETweenEase>(EnumValue);

			// Hide CustomEaseCurve if Ease != CustomCurve
			if (UEdGraphPin* CustomEaseCurvePin = FindPin(TEXT("Tween_Settings_CustomEaseCurve"))) {
				CustomEaseCurvePin->bHidden = (CurrentEase != ETweenEase::CustomCurve);
			}
		}
	}

	// when repeat count != 0
	//		show dleays repeat 
	//		On Repeat
	const UEdGraphPin* RepeatCountPin = FindPin(TEXT("Tween_Settings_RepeatCount"));
	if (RepeatCountPin) {
		const FString& RepeatCountValue = RepeatCountPin->DefaultValue.IsEmpty()
			? RepeatCountPin->AutogeneratedDefaultValue
			: RepeatCountPin->DefaultValue;
		const int32 RepeatCount = FCString::Atoi(*RepeatCountValue);

		// Hide Delays.Repeat and OnRepeat if RepeatCount == 0
		if (UEdGraphPin* RepeatDelayPin = FindPin(TEXT("Tween_Settings_Delays_Repeat"))) {
			RepeatDelayPin->bHidden = (RepeatCount == 0);
		}
		if (UEdGraphPin* OnRepeatPin = FindPin(TEXT("OnRepeat"))) {
			OnRepeatPin->bHidden = (RepeatCount == 0);
		}
	}

	// when loop mode == ping pong:
	//		show Delays reverse 
	//		show On Reverse Begin
	const UEdGraphPin* LoopModePin = FindPin(TEXT("Tween_Settings_LoopMode"));
	if (LoopModePin) {
		const FString& LoopModeValue = LoopModePin->DefaultValue.IsEmpty()
			? LoopModePin->AutogeneratedDefaultValue
			: LoopModePin->DefaultValue;
		const int64 EnumValue = StaticEnum<ETweenLoopMode>()->GetValueByNameString(LoopModeValue);
		const ETweenLoopMode CurrentLoopMode = static_cast<ETweenLoopMode>(EnumValue);

		if (UEdGraphPin* ReverseDelayPin = FindPin(TEXT("Tween_Settings_Delays_Reverse"))) {
			ReverseDelayPin->bHidden = (CurrentLoopMode != ETweenLoopMode::PingPong);
		}
		if (UEdGraphPin* OnReverseBeginPin = FindPin(TEXT("OnReverseBegin"))) {
			OnReverseBeginPin->bHidden = (CurrentLoopMode != ETweenLoopMode::PingPong);
		}
	}
}

void UK2Node_Tween::EnsureAdvancedView() {
	/* Hide all Tween settings except Duration and Ease by default */
	for (UEdGraphPin* P : Pins) {
		const FString Name = P->PinName.ToString();

		if (!Name.StartsWith(TEXT("Tween_Settings"))) { continue; }
		if (Name == TEXT("Tween_Settings_Duration")) { continue; }
		if (Name == TEXT("Tween_Settings_Ease")) { continue; }
		if (Name == TEXT("Tween_Settings_CustomEaseCurve")) { continue; }

		P->bAdvancedView = true;
	}

	CustomizedAdvanced();
}

void UK2Node_Tween::GetRedirectPinNames(const UEdGraphPin& Pin, TArray<FString>& RedirectPinNames) const {
	Super::GetRedirectPinNames(Pin, RedirectPinNames);

	// Graphs saved before "To" became an array had a single "Tween_To" pin.
	// Let it redirect onto today's first waypoint pin so old wires survive.
	if (Pin.PinName == TEXT("Tween_To_0")) {
		RedirectPinNames.Add(TEXT("Tween_To"));
	}
}

void UK2Node_Tween::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) {
	// IMPORTANT: this must run *before* Super::ExpandNode(). The generic
	// struct-split recombination for our "Tween" pin (Tween_From / Tween_To
	// / Tween_Settings -> a synthesized "Make FTween<Type>Settings" node) is
	// performed by UK2Node::ExpandSplitPin(), which Super::ExpandNode()
	// reaches internally (UK2Node_CallFunction::ExpandNode calls
	// Super::ExpandNode(), i.e. UK2Node::ExpandNode(), which walks this
	// node's own split pins). Whatever "Tween_To" is connected to *at that
	// point* is what gets baked into the call. So we build the "To" array
	// from our Tween_To_0..N pins and wire it into the hidden "Tween_To"
	// pin first, and only then let Super recombine everything.
	UEdGraphPin* ArrayPin = FindPin(TEXT("Tween_To"));
	if (ArrayPin) {
		TArray<UEdGraphPin*> ElementPins;
		int32 Index = 0;
		while (UEdGraphPin* P = FindPin(FName(*FString::Printf(TEXT("Tween_To_%d"), Index)))) {
			ElementPins.Add(P);
			++Index;
		}

		if (ElementPins.Num() > 0) {
			UK2Node_MakeArray* MakeArrayNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(this, SourceGraph);
			MakeArrayNode->NumInputs = ElementPins.Num();
			MakeArrayNode->AllocateDefaultPins();

			const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

			for (int32 i = 0; i < ElementPins.Num(); ++i) {
				UEdGraphPin* Src = ElementPins[i];
				// UK2Node_MakeArray names its inputs "[0]", "[1]", etc.
				UEdGraphPin* Dst = MakeArrayNode->FindPin(FName(*FString::Printf(TEXT("[%d]"), i)));
				if (!Dst) { continue; }

				Dst->PinType = Src->PinType;

				if (Src->LinkedTo.Num() > 0) {
					CompilerContext.MovePinLinksToIntermediate(*Src, *Dst);
				} else {
					Dst->DefaultValue = Src->DefaultValue;
					Dst->DefaultObject = Src->DefaultObject;
					Dst->DefaultTextValue = Src->DefaultTextValue;
				}
			}

			UEdGraphPin* ArrayOutputPin = MakeArrayNode->GetOutputPin();
			ArrayOutputPin->PinType = ArrayPin->PinType;

			Schema->TryCreateConnection(ArrayOutputPin, ArrayPin);
		}
	}

	Super::ExpandNode(CompilerContext, SourceGraph);
}

void UK2Node_Tween::AddInputPin() {
	Modify();
	++NumToPins;
	ReconstructNode();

	if (UBlueprint* BP = GetBlueprint()) {
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
	}
}

void UK2Node_Tween::RemoveInputPin(UEdGraphPin* Pin) {
	if (!CanRemovePin(Pin)) { return; }

	Modify();
	--NumToPins;
	Pin->MarkAsGarbage();
	Pins.Remove(Pin);
	ReconstructNode();

	if (UBlueprint* BP = GetBlueprint()) {
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
	}
}

bool UK2Node_Tween::CanAddPin() const {
	return true;
}

bool UK2Node_Tween::CanRemovePin(const UEdGraphPin* Pin) const {
	// Always keep at least one waypoint pin.
	return Pin
		&& Pin->PinName.ToString().StartsWith(TEXT("Tween_To_"))
		&& NumToPins > 1;
}
