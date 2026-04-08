#pragma once
#include "CoreMinimal.h"
#include "TweenTraits.h"
#include "TweenTypes.generated.h"

// -----------------------------------------------------------------------
// Dynamic Delegates for Blueprint-bindable callbacks
// -----------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTweenSimpleDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTweenAlphaDelegate, float, Alpha);

UENUM(BlueprintType)
enum class ETweenEase : uint8 {
	Linear,
	InSine, OutSine, InOutSine,
	InQuad, OutQuad, InOutQuad,
	InCubic, OutCubic, InOutCubic,
	InQuart, OutQuart, InOutQuart,
	InQuint, OutQuint, InOutQuint,
	InExpo, OutExpo, InOutExpo,
	InCirc, OutCirc, InOutCirc,
	InBack, OutBack, InOutBack,
	InElastic, OutElastic, InOutElastic,
	InBounce, OutBounce, InOutBounce,
};

UENUM(BlueprintType)
enum class ETweenLoopMode : uint8 {
	Restart   UMETA(DisplayName = "Restart (0->1, 0->1...)"),
	PingPong  UMETA(DisplayName = "Ping-pong (0->1->0...)"),
};

/* Struct to hold all tween settings, used for presets and inline configuration. */
USTRUCT()
struct TYPETWEEN_API FTweenSettings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Tween", meta = (
		ToolTip = "Total time for one forward or reverse playthrough, not including delays. Required."
		))
	float Duration = 1.f;

	UPROPERTY(EditAnywhere, Category = "Tween", meta = (
		ToolTip = "Easing function for the tween, see https://easings.net/ for visualization"
		))
	ETweenEase Ease = ETweenEase::Linear;

	UPROPERTY(EditAnywhere, Category = "Tween", meta = (
		ToolTip = "0 = play once, -1 = infinite, N = play N+1 times total"
		))
	int32 RepeatCount = 0;

	UPROPERTY(EditAnywhere, Category = "Tween", meta = (
		ToolTip = "How the tween loops back after reaching the end. Restart: jumps back to start. PingPong: reverses direction each cycle."
		))
	ETweenLoopMode LoopMode = ETweenLoopMode::Restart;

	UPROPERTY(EditAnywhere, Category = "Timing", meta = (
		ToolTip = "Delay before the tween starts playing."
		))
	float StartDelay = 0.f;

	UPROPERTY(EditAnywhere, Category = "Timing", meta = (
		ToolTip = "Delay before the tween starts playing in reverse."
		))
	float ReverseDelay = 0.f;

	UPROPERTY(EditAnywhere, Category = "Timing", meta = (
		ToolTip = "Delay before the tween repeats."
		))
	float RepeatDelay = 0.f;

	UPROPERTY(EditAnywhere, Category = "Timing", meta = (
		ToolTip = "Delay after the tween finishes playing before firing OnComplete."
		))
	float EndDelay = 0.f;
};

// -----------------------------------------------------------------------
// Callback container - holds both Blueprint delegates and C++ TFunctions
// -----------------------------------------------------------------------
USTRUCT(BlueprintType)
struct TYPETWEEN_API FTweenCallbacks {
	GENERATED_BODY()

	void OnPreStart(TFunction<void()> Callback) { OnPreStartFn = MoveTemp(Callback); }
	void OnStart(TFunction<void()> Callback) { OnStartFn = MoveTemp(Callback); }
	void OnCycleBegin(TFunction<void()> Callback) { OnCycleBeginFn = MoveTemp(Callback); }
	void OnForwardEnd(TFunction<void()> Callback) { OnForwardEndFn = MoveTemp(Callback); }
	void OnReverseBegin(TFunction<void()> Callback) { OnReverseBeginFn = MoveTemp(Callback); }
	void OnCycleEnd(TFunction<void()> Callback) { OnCycleEndFn = MoveTemp(Callback); }
	void OnRepeat(TFunction<void()> Callback) { OnRepeatFn = MoveTemp(Callback); }
	void OnComplete(TFunction<void()> Callback) { OnCompleteFn = MoveTemp(Callback); }
	void OnTick(TFunction<void()> Callback) { OnTickFn = MoveTemp(Callback); }

private:
	template<typename T>
	friend class TypeTween::Detail::TweenBase;
	// ---- Blueprint-bindable delegates ----

	/** Fired first frame when the tween is triggered, before StartDelay begins. */
	UPROPERTY(BlueprintAssignable, Category = "Tween|Events")
	FTweenSimpleDelegate OnPreStartDel;

	/** Fired once the tween starts playing (after StartDelay). */
	UPROPERTY(BlueprintAssignable, Category = "Tween|Events")
	FTweenSimpleDelegate OnStartDel;

	/** Fired at the start of each new cycle (after StartDelay or RepeatDelay). */
	UPROPERTY(BlueprintAssignable, Category = "Tween|Events")
	FTweenSimpleDelegate OnCycleBeginDel;
	/** Fired at the end of the forward phase of each cycle (before ReverseDelay), when t == 1. */
	UPROPERTY(BlueprintAssignable, Category = "Tween|Events")
	FTweenSimpleDelegate OnForwardEndDel;

	/** [Ping Pong Only] Fired at the start of the reverse phase of each cycle (after ReverseDelay). */
	UPROPERTY(BlueprintAssignable, Category = "Tween|Events")
	FTweenSimpleDelegate OnReverseBeginDel;
	/** Fired at the end of each cycle (before EndDelay or RepeatDelay), when t == 0. */
	UPROPERTY(BlueprintAssignable, Category = "Tween|Events")
	FTweenSimpleDelegate OnCycleEndDel;

	/** Fired at the end of each cycle except the last one. */
	UPROPERTY(BlueprintAssignable, Category = "Tween|Events")
	FTweenSimpleDelegate OnRepeatDel;
	/** Fired once the tween finishes playing (after EndDelay + repeats). */
	UPROPERTY(BlueprintAssignable, Category = "Tween|Events")
	FTweenSimpleDelegate OnCompleteDel;

	/** Fired every tick, including during delays. */
	UPROPERTY(BlueprintAssignable, Category = "Tween|Events")
	FTweenSimpleDelegate OnTickDel;
	/** Fired during interpolation with the current alpha value [0..1]. Type-erased version. */
	UPROPERTY(BlueprintAssignable, Category = "Tween|Events")
	FTweenAlphaDelegate OnUpdateDel;

	// ---- C++ callbacks (not exposed to Blueprint) ----
	TFunction<void()> OnPreStartFn;
	TFunction<void()> OnStartFn;
	TFunction<void()> OnCycleBeginFn;
	TFunction<void()> OnForwardEndFn;
	TFunction<void()> OnReverseBeginFn;
	TFunction<void()> OnCycleEndFn;
	TFunction<void()> OnRepeatFn;
	TFunction<void()> OnCompleteFn;
	TFunction<void()> OnTickFn;
	// Note: Typed OnUpdate with value stays in ITween<T>

		// ---- Broadcast methods (fires both BP delegate and C++ TFunction) ----
	void BroadcastOnPreStart() { OnPreStartDel.Broadcast();     if (OnPreStartFn) OnPreStartFn(); }
	void BroadcastOnStart() { OnStartDel.Broadcast();        if (OnStartFn) OnStartFn(); }
	void BroadcastOnCycleBegin() { OnCycleBeginDel.Broadcast();   if (OnCycleBeginFn) OnCycleBeginFn(); }
	void BroadcastOnForwardEnd() { OnForwardEndDel.Broadcast();   if (OnForwardEndFn) OnForwardEndFn(); }
	void BroadcastOnReverseBegin() { OnReverseBeginDel.Broadcast(); if (OnReverseBeginFn) OnReverseBeginFn(); }
	void BroadcastOnCycleEnd() { OnCycleEndDel.Broadcast();     if (OnCycleEndFn) OnCycleEndFn(); }
	void BroadcastOnRepeat() { OnRepeatDel.Broadcast();       if (OnRepeatFn) OnRepeatFn(); }
	void BroadcastOnComplete() { OnCompleteDel.Broadcast();     if (OnCompleteFn) OnCompleteFn(); }
	void BroadcastOnTick() { OnTickDel.Broadcast();         if (OnTickFn) OnTickFn(); }
	void BroadcastOnUpdate(float Alpha) { OnUpdateDel.Broadcast(Alpha); }

};

/* Struct to specify which tween settings to override when using a preset. */
/* A bit ugly to copy each field, but necessary for Unreal's property system. */
/* Feel free to make an issue/PR to adress this! */
USTRUCT()
struct TYPETWEEN_API FTweenOverrides {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle))
	bool bDuration = false;
	UPROPERTY(EditAnywhere, Category = "Tween", meta = (EditCondition = "bDuration",
		ToolTip = "Total time for one forward or reverse playthrough, not including delays."))
	float Duration = 1.f;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle))
	bool bEase = false;
	UPROPERTY(EditAnywhere, Category = "Tween", meta = (EditCondition = "bEase",
		ToolTip = "Easing function for the tween, see https://easings.net/"))
	ETweenEase Ease = ETweenEase::Linear;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle))
	bool bRepeatCount = false;
	UPROPERTY(EditAnywhere, Category = "Tween", meta = (EditCondition = "bRepeatCount",
		ToolTip = "0 = play once, -1 = infinite, N = play N+1 times total"))
	int32 RepeatCount = 0;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle))
	bool bLoopMode = false;
	UPROPERTY(EditAnywhere, Category = "Tween", meta = (EditCondition = "bLoopMode",
		ToolTip = "How the tween loops back after reaching the end."))
	ETweenLoopMode LoopMode = ETweenLoopMode::Restart;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle))
	bool bStartDelay = false;
	UPROPERTY(EditAnywhere, Category = "Timing", meta = (EditCondition = "bStartDelay"))
	float StartDelay = 0.f;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle))
	bool bReverseDelay = false;
	UPROPERTY(EditAnywhere, Category = "Timing", meta = (EditCondition = "bReverseDelay"))
	float ReverseDelay = 0.f;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle))
	bool bRepeatDelay = false;
	UPROPERTY(EditAnywhere, Category = "Timing", meta = (EditCondition = "bRepeatDelay"))
	float RepeatDelay = 0.f;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle))
	bool bEndDelay = false;
	UPROPERTY(EditAnywhere, Category = "Timing", meta = (EditCondition = "bEndDelay"))
	float EndDelay = 0.f;
};

/* Data asset class to hold tween presets. */
UCLASS()
class TYPETWEEN_API UTweenPreset : public UDataAsset {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Tween")
	FTweenSettings Settings;
};

USTRUCT(BlueprintType)
struct TYPETWEEN_API FTweenConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Tween", meta = (
		DisplayName = "Preset (Optional)",
		ToolTip = "Assign a shared preset asset to reuse settings across actors. Leave empty to configure inline below."
		))
	UTweenPreset* Preset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Tween", meta = (
		EditCondition = "Preset == nullptr",
		EditConditionHides
		))
	FTweenSettings Settings;

	UPROPERTY(EditAnywhere, Category = "Tween", meta = (
		DisplayName = "Overrides",
		EditCondition = "Preset != nullptr", EditConditionHides,
		ToolTip = "Check a field to override that value from the preset."
		))
	FTweenOverrides Overrides;

	FTweenSettings Resolve() const {
		if (!Preset) return Settings;

		/* Copy preset settings and apply overrides */
		/* Code duplication necessary due to Unreal's property system */
		/* If you have any suggestions on how to make this cleaner, please make an issue/PR! */
		FTweenSettings Out = Preset->Settings;
		if (Overrides.bDuration)     Out.Duration = Overrides.Duration;
		if (Overrides.bEase)         Out.Ease = Overrides.Ease;
		if (Overrides.bRepeatCount)  Out.RepeatCount = Overrides.RepeatCount;
		if (Overrides.bLoopMode)     Out.LoopMode = Overrides.LoopMode;
		if (Overrides.bStartDelay)   Out.StartDelay = Overrides.StartDelay;
		if (Overrides.bReverseDelay) Out.ReverseDelay = Overrides.ReverseDelay;
		if (Overrides.bRepeatDelay)  Out.RepeatDelay = Overrides.RepeatDelay;
		if (Overrides.bEndDelay)     Out.EndDelay = Overrides.EndDelay;
		return Out;
	}
};