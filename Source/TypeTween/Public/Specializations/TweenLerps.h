// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
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

