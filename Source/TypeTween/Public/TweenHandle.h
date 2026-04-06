#pragma once
#include "TweenControl.h"

namespace TypeTween {

	// Forward declarations
	template<typename T, typename... Args>
	class ITween;

	template<typename TweenT = ITween<void>>
	class TweenHandle;

	// -----------------------------------------------------------------------
	// Type-erased handle - can control any tween but cannot access typed methods
	// -----------------------------------------------------------------------
	class FTweenHandle {
	public:
		FTweenHandle() = default;

		explicit FTweenHandle(TSharedPtr<ITweenControl> InTween)
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
		TSharedPtr<ITweenControl> TweenPtr;
	};

	// -----------------------------------------------------------------------
	// Typed handle - full access to all tween methods including type-specific
	// (e.g., ITween<float>, ITween<FVector>)
	// -----------------------------------------------------------------------
	template<typename TweenT>
	class TTweenHandle : public FTweenHandle {
	public:
		TTweenHandle() = default;

		/** Construct from typed shared pointer. */
		explicit TTweenHandle(TSharedPtr<TweenT> InTween)
			: FTweenHandle(StaticCastSharedPtr<ITweenControl>(InTween))
			, TypedPtr(MoveTemp(InTween)) {
		}

		/** Full typed access via arrow operator. */
		TweenT* operator->() const { return TypedPtr.Get(); }
		TweenT& operator*() const { return *TypedPtr; }
		TweenT* Get() const { return TypedPtr.Get(); }
		bool IsValid() const { return TypedPtr.IsValid(); }
		explicit operator bool() const { return IsValid(); }

		operator FTweenHandle() const {
			return FTweenHandle(TypedPtr);
		}

		void Reset() {
			TypedPtr.Reset();
			FTweenHandle::Reset();
		}

	private:
		TSharedPtr<TweenT> TypedPtr;
	};

	// -----------------------------------------------------------------------
	// Convenience type alias for cleaner syntax:
	// TweenHandle<float> instead of TTweenHandle<ITween<float>>
	// -----------------------------------------------------------------------
	template<typename T>
	using TweenHandle = TTweenHandle<ITween<T>>;
}
