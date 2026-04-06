#pragma once
#include "TweenTypes.h"
#include "TweenEasing.h"
#include "TweenBase.h"
#include "ITween.h"
#include "TweenHandle.h"
#include "TweenSubsystem.h"

namespace TypeTween {
	/* Tween a user provided value, user must make sure value does not get invalidated during the tween's lifetime */
	template <typename T>
		requires (!TIsConst<T>::Value)
	inline ITween<T>& Tween(T& Value, const UObject* WorldContext) {
		UTweenSubsystem* Sub = UTweenSubsystem::Get(WorldContext);
		checkf(Sub, TEXT("Tweening::tween - UTweenSubsystem not found. Is WorldContext valid?"));
		return Sub->RegisterTween(MakeShared<ITween<T>>(&Value));
	}

	/* Tween an internal value owned by the tween, useful for fire-and-forget or when the value is only needed in the update callback. */
	template <typename T>
	inline ITween<T>& Tween(const UObject* WorldContext) {
		UTweenSubsystem* Sub = UTweenSubsystem::Get(WorldContext);
		checkf(Sub, TEXT("UTweenSubsystem not found."));
		return Sub->RegisterTween(
			StaticCastSharedRef<ITween<T>>(
				MakeShared<ITweenOwned<T>>()
			)
		);
	}

	/* Type-deducing overload, if you provide an initial value, the tween can deduce the type and you don't have to specify it. Also sets the start value to provided value by default. */
	template <typename T>
	inline ITween<typename TDecay<T>::Type>& Tween(T&& InitialValue, const UObject* WorldContext) {
		using Decayed = typename TDecay<T>::Type;

		UTweenSubsystem* Sub = UTweenSubsystem::Get(WorldContext);
		checkf(Sub, TEXT("UTweenSubsystem not found."));
		return Sub->RegisterTween(
			StaticCastSharedRef<ITween<Decayed>>(
				MakeShared<ITweenOwned<Decayed>>(
					Forward<T>(InitialValue)
				)
			)
		);
	}

	/* Tween with no value, useful for just using the timing and callbacks. */
	inline ITween<void>& Tween(const UObject* WorldContext) {
		UTweenSubsystem* Sub = UTweenSubsystem::Get(WorldContext);
		checkf(Sub, TEXT("UTweenSubsystem not found."));
		return Sub->RegisterTween(MakeShared<ITween<void>>());
	}
}