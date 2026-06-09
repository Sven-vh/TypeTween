// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once
#include "TweenControl.h"
#include "TweenTraits.h"

namespace TypeTween {
	namespace Detail {

		template<typename T>
		FORCEINLINE T* PtrGet(TSharedPtr<T>& Ptr) { return Ptr.Get(); }
		template<typename T>
		FORCEINLINE const T* PtrGet(const TSharedPtr<T>& Ptr) { return Ptr.Get(); }

		template<typename T>
		FORCEINLINE T* PtrGet(TWeakPtr<T>& Ptr) { return Ptr.Pin().Get(); }
		template<typename T>
		FORCEINLINE const T* PtrGet(const TWeakPtr<T>& Ptr) { return Ptr.Pin().Get(); }


		/* Type-erased handle */
		template<template<typename> typename PtrType>
		class TweenHandle {
		public:
			TweenHandle() = default;

			explicit TweenHandle(PtrType<ITweenControl> InTween)
				: TweenPtr(MoveTemp(InTween)) {
			}

			/** Access control methods via arrow operator. */
			const ITweenControl* operator->() const { return PtrGet(TweenPtr); }
			const ITweenControl& operator*() const { return *PtrGet(TweenPtr); }
			const ITweenControl* Get() const { return PtrGet(TweenPtr); }

			ITweenControl* operator->() { return PtrGet(TweenPtr); }
			ITweenControl& operator*() { return *PtrGet(TweenPtr); }
			ITweenControl* Get() { return PtrGet(TweenPtr); }

			bool IsValid() const { return TweenPtr.IsValid(); }
			explicit operator bool() const { return IsValid(); }

			const PtrType<ITweenControl> GetPtr() const { return TweenPtr; }

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
				: BaseType(PtrType<ITweenControl>(InTween))
				, TypedPtr(MoveTemp(InTween)) {
			}

			/** Full typed access via arrow operator. */
			const TweenT* operator->() const { return PtrGet(TypedPtr); }
			const TweenT& operator*() const { return *PtrGet(TypedPtr); }
			const TweenT* Get() const { return PtrGet(TypedPtr); }

			TweenT* operator->() { return PtrGet(TypedPtr); }
			TweenT& operator*() { return *PtrGet(TypedPtr); }
			TweenT* Get() { return PtrGet(TypedPtr); }

			const PtrType<TweenT>& GetTypedPtr() const { return TypedPtr; }

			bool IsValid() const { return TypedPtr.IsValid(); }
			explicit operator bool() const { return IsValid(); }

			TypedTweenHandle<TweenT, TSharedPtr> ToShared() {
				return TypedTweenHandle<TweenT, TSharedPtr>(TypedPtr.Pin());
			}

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

