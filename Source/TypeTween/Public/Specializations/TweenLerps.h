#pragma once
#include "TweenTraits.h"
#include "Math/UnrealMathUtility.h"

template<typename T>
	requires TypeTween::Traits::THasUnrealLerp<T>
struct TypeTween::Lerper<T> {
	static T Lerp(const T& A, const T& B, float Alpha) {
		return FMath::Lerp(A, B, Alpha);
	}
};

template<typename T>
	requires TypeTween::Traits::TIsLerpable<T> && (!TypeTween::Traits::THasUnrealLerp<T>)
struct TypeTween::Lerper<T> {
	static T Lerp(const T& A, const T& B, float Alpha) {
		return A + (B - A) * Alpha;
	}
};
