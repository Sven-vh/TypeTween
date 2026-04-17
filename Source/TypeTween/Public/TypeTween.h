#pragma once
#include "TweenTypes.h"
#include "TweenEasing.h"
#include "TweenBase.h"
#include "Specializations/ITween.h"
#include "TweenHandle.h"
#include "TweenSubsystem.h"
#include "TweenTraits.h"
#include "Specializations/TweenLerps.h"
#include "Specializations/TweenTransform.h"

namespace TypeTween {

	/* Tween a user provided value, user must make sure value does not get invalidated during the tween's lifetime */
	template <typename T>
		requires ((!TIsConst<T>::Value) && Traits::THasITween<T>)
	inline ITween<T>& Tween(T& Value, const UObject* WorldContext) {
		UTweenSubsystem* Sub = UTweenSubsystem::Get(WorldContext);
		checkf(Sub, TEXT("UTweenSubsystem not found."));

		return Sub->RegisterTween(MakeShared<ITween<T>>(&Value));
	}

	template <typename T>
		requires ((!TIsConst<T>::Value) && Traits::THasITween<T>)
	inline ITween<T>& Tween(T* Value, const UObject* WorldContext) {
		UTweenSubsystem* Sub = UTweenSubsystem::Get(WorldContext);
		checkf(Sub, TEXT("UTweenSubsystem not found."));

		return Sub->RegisterTween(MakeShared<ITween<T>>(Value));
	}

	/* Tween an internal value owned by the tween, useful for fire-and-forget or when the value is only needed in the update callback. */
	template <typename T>
		requires (Traits::THasITween<T> && !std::is_void_v<T>)
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
	template <typename T, typename Decayed = typename TDecay<T>::Type>
		requires Traits::THasITween<Decayed>
	inline ITween<Decayed>& Tween(T&& InitialValue, const UObject* WorldContext) {
		static_assert(std::is_same_v<Decayed, typename TDecay<T>::Type>, "Do not specify Decayed manually.");

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

	/* Void tween template overload - allows explicit Tween<void>(WorldContext) syntax */
	template <typename T>
		requires std::is_void_v<T>
	inline ITween<void>& Tween(const UObject* WorldContext) {
		return Tween(WorldContext);  // Delegate to non-template version
	}

	/* Tween an Actor (or any class derived from AActor). */
	template <typename T>
		requires std::is_base_of_v<AActor, T>
	inline ITween<AActor>& Tween(T* Actor) {
		UTweenSubsystem* Sub = UTweenSubsystem::Get(Actor);
		checkf(Sub, TEXT("UTweenSubsystem not found."));

		return Sub->RegisterTween(MakeShared<ITween<AActor>>(Actor));
	}
}