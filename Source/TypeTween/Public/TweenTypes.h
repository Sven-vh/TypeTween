#pragma once
#include "CoreMinimal.h"
#include "TweenTypes.generated.h"

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
	Loop      UMETA(DisplayName = "Loop (0->1, 0->1...)"),
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
		ToolTip = "How the tween loops back after reaching the end. Loop: jumps back to start. PingPong: reverses direction each cycle."
		))
	ETweenLoopMode LoopMode = ETweenLoopMode::Loop;

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
	ETweenLoopMode LoopMode = ETweenLoopMode::Loop;

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