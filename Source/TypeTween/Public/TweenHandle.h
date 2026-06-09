// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "TweenControl.h"
#include "TweenTraits.h"

namespace TypeTween {
	namespace Detail {

		/* Type-erased handle */
		template<template<typename> typename PtrType>
		class TweenHandle {
		public:
			TweenHandle() = default;

			explicit TweenHandle(PtrType<ITweenControl> InTween)
				: TweenPtr(MoveTemp(InTween)) {
			}

			/** Access control methods via arrow operator. */
			ITweenControl* operator->() const { return TweenPtr.Get(); }
			ITweenControl& operator*() const { return *TweenPtr; }
			bool IsValid() const { return TweenPtr.IsValid(); }
			explicit operator bool() const { return IsValid(); }

			/** Release the handle, potentially allowing the tween to be destroyed. */
			void Reset() { TweenPtr.Reset(); }

		protected:
			PtrType<ITweenControl> TweenPtr;
		};

		/* Typed handle */
		template<typename TweenT, template<typename> typename PtrType>
		class TypedTweenHandle : public TweenHandle<PtrType> {
			using BaseType = TweenHandle<PtrType>;
		public:
			TypedTweenHandle() = default;

			/* Construct from typed shared pointer. */
			explicit TypedTweenHandle(PtrType<TweenT> InTween)
				//: FTweenHandle(StaticCastSharedPtr<ITweenControl>(InTween))
				: BaseType(PtrType<ITweenControl>(InTween))
				, TypedPtr(MoveTemp(InTween)) {
			}

			/** Full typed access via arrow operator. */
			const TweenT* operator->() const { return TypedPtr.Get(); }
			const TweenT& operator*() const { return *TypedPtr; }
			const TweenT* Get() const { return TypedPtr.Get(); }
			TweenT* operator->() { return TypedPtr.Get(); }
			TweenT& operator*() { return *TypedPtr; }
			TweenT* Get() { return TypedPtr.Get(); }
			bool IsValid() const { return TypedPtr.IsValid(); }
			explicit operator bool() const { return IsValid(); }

			void Reset() {
				TypedPtr.Reset();
				BaseType::Reset();
			}

		private:
			PtrType<TweenT> TypedPtr;
		};
	}


	/* aliases */
	using FTweenHandle = Detail::TweenHandle<TSharedPtr>;
	using FTweenWeakHandle = Detail::TweenHandle<TWeakPtr>;

	template<typename T>
	using TTweenHandle = Detail::TypedTweenHandle<ITween<T>, TSharedPtr>;
	template<typename T>
	using TTweenWeakHandle = Detail::TypedTweenHandle<ITween<T>, TWeakPtr>;

}

