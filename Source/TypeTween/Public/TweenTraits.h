#pragma once
#include <concepts>

namespace TypeTween {

	/* Forward Declarations */
	template<typename T, typename... Args>
	class ITween;


	namespace Detail {
		template<typename T>
		class TweenBase;
	}

	/* Specialize this struct for custom types */
	template<typename T>
	struct Lerper;

	/* Free function public API */
	template<typename T>
	static T Lerp(const T& A, const T& B, float F) {
		return Lerper<T>::Lerp(A, B, F);
	}

	/* Concept checks */
	namespace Traits {
		/* Requires a type to have +, -, and * operators */
		template<typename T>
		concept TIsLerpable = requires(T A, T B, float F) {
			{ A + (B - A) * F } -> std::convertible_to<T>;
		};

		template<typename T>
		concept THasUnrealLerp = TCustomLerp<T>::Value;

		template<typename T>
		concept THasLerper = requires(T A, T B, float F) {
			{ Lerper<T>::Lerp(A, B, F) } -> std::convertible_to<T>;
		};

		template<typename T>
		concept THasITween = requires {
			typename ITween<T>;
		};
	}
}