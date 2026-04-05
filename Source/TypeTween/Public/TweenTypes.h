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
