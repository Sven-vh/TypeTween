#pragma once
#include "TweenTypes.h"
#include "Math/UnrealMathUtility.h"

/* Formulas from https://easings.net/ */
namespace TypeTween::Detail {
	inline float ApplyEase(float t, ETweenEase Ease) {
		switch (Ease) {
		case ETweenEase::Linear:     return t;

		case ETweenEase::InSine:     return 1.f - FMath::Cos(t * UE_PI / 2.f);
		case ETweenEase::OutSine:    return FMath::Sin(t * UE_PI / 2.f);
		case ETweenEase::InOutSine:  return -(FMath::Cos(UE_PI * t) - 1.f) / 2.f;

		case ETweenEase::InQuad:     return t * t;
		case ETweenEase::OutQuad:    return 1.f - (1.f - t) * (1.f - t);
		case ETweenEase::InOutQuad:  return t < 0.5f ? 2.f * t * t : 1.f - FMath::Pow(-2.f * t + 2.f, 2.f) / 2.f;

		case ETweenEase::InCubic:    return t * t * t;
		case ETweenEase::OutCubic:   return 1.f - FMath::Pow(1.f - t, 3.f);
		case ETweenEase::InOutCubic: return t < 0.5f ? 4.f * t * t * t : 1.f - FMath::Pow(-2.f * t + 2.f, 3.f) / 2.f;

		case ETweenEase::InQuart:    return t * t * t * t;
		case ETweenEase::OutQuart:   return 1.f - FMath::Pow(1.f - t, 4.f);
		case ETweenEase::InOutQuart: return t < 0.5f ? 8.f * t * t * t * t : 1.f - FMath::Pow(-2.f * t + 2.f, 4.f) / 2.f;

		case ETweenEase::InQuint:    return t * t * t * t * t;
		case ETweenEase::OutQuint:   return 1.f - FMath::Pow(1.f - t, 5.f);
		case ETweenEase::InOutQuint: return t < 0.5f ? 16.f * t * t * t * t * t : 1.f - FMath::Pow(-2.f * t + 2.f, 5.f) / 2.f;

		case ETweenEase::InExpo:     return FMath::IsNearlyZero(t) ? 0.f : FMath::Pow(2.f, 10.f * t - 10.f);
		case ETweenEase::OutExpo:    return FMath::IsNearlyEqual(t, 1.f) ? 1.f : 1.f - FMath::Pow(2.f, -10.f * t);
		case ETweenEase::InOutExpo:
			if (FMath::IsNearlyZero(t))      return 0.f;
			if (FMath::IsNearlyEqual(t, 1.f)) return 1.f;
			return t < 0.5f
				? FMath::Pow(2.f, 20.f * t - 10.f) / 2.f
				: (2.f - FMath::Pow(2.f, -20.f * t + 10.f)) / 2.f;

		case ETweenEase::InCirc:     return 1.f - FMath::Sqrt(1.f - t * t);
		case ETweenEase::OutCirc:    return FMath::Sqrt(1.f - FMath::Pow(t - 1.f, 2.f));
		case ETweenEase::InOutCirc:
			return t < 0.5f
				? (1.f - FMath::Sqrt(1.f - FMath::Pow(2.f * t, 2.f))) / 2.f
				: (FMath::Sqrt(1.f - FMath::Pow(-2.f * t + 2.f, 2.f)) + 1.f) / 2.f;

		case ETweenEase::InBack:
		{
			constexpr float C1 = 1.70158f, C3 = C1 + 1.f;
			return C3 * t * t * t - C1 * t * t;
		}
		case ETweenEase::OutBack:
		{
			constexpr float C1 = 1.70158f, C3 = C1 + 1.f;
			const float T = t - 1.f;
			return 1.f + C3 * T * T * T + C1 * T * T;
		}
		case ETweenEase::InOutBack:
		{
			constexpr float C2 = 1.70158f * 1.525f;
			return t < 0.5f
				? FMath::Pow(2.f * t, 2.f) * ((C2 + 1.f) * 2.f * t - C2) / 2.f
				: (FMath::Pow(2.f * t - 2.f, 2.f) * ((C2 + 1.f) * (2.f * t - 2.f) + C2) + 2.f) / 2.f;
		}

		case ETweenEase::InElastic:
			if (FMath::IsNearlyZero(t) || FMath::IsNearlyEqual(t, 1.f)) return t;
			return -FMath::Pow(2.f, 10.f * t - 10.f) * FMath::Sin((t * 10.f - 10.75f) * (2.f * UE_PI) / 3.f);
		case ETweenEase::OutElastic:
			if (FMath::IsNearlyZero(t) || FMath::IsNearlyEqual(t, 1.f)) return t;
			return FMath::Pow(2.f, -10.f * t) * FMath::Sin((t * 10.f - 0.75f) * (2.f * UE_PI) / 3.f) + 1.f;
		case ETweenEase::InOutElastic:
			if (FMath::IsNearlyZero(t) || FMath::IsNearlyEqual(t, 1.f)) return t;
			return t < 0.5f
				? -(FMath::Pow(2.f, 20.f * t - 10.f) * FMath::Sin((20.f * t - 11.125f) * (2.f * UE_PI) / 4.5f)) / 2.f
				: (FMath::Pow(2.f, -20.f * t + 10.f) * FMath::Sin((20.f * t - 11.125f) * (2.f * UE_PI) / 4.5f)) / 2.f + 1.f;

		case ETweenEase::OutBounce:
		{
			if (t < 1.f / 2.75f)        return 7.5625f * t * t;
			if (t < 2.f / 2.75f) { const float f = t - 1.5f / 2.75f; return 7.5625f * f * f + 0.75f; }
			if (t < 2.5f / 2.75f) { const float f = t - 2.25f / 2.75f; return 7.5625f * f * f + 0.9375f; }
			/* else */ { const float f = t - 2.625f / 2.75f; return 7.5625f * f * f + 0.984375f; }
		}
		case ETweenEase::InBounce:    return 1.f - ApplyEase(1.f - t, ETweenEase::OutBounce);
		case ETweenEase::InOutBounce:
			return t < 0.5f
				? (1.f - ApplyEase(1.f - 2.f * t, ETweenEase::OutBounce)) / 2.f
				: (1.f + ApplyEase(2.f * t - 1.f, ETweenEase::OutBounce)) / 2.f;

		default: return t;
		}
	}
}