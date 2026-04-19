#include "Blueprints/TweenAsyncBase.h"

void UTweenAsyncBaseSimple::ActivateSimple(TypeTween::ITweenControl& Tween) {
	auto& CB = Tween.GetCallbacks();

	/* Only connect OnComplete */
	TWeakObjectPtr<UTweenAsyncBaseSimple> WeakThis(this);
	CB.OnComplete([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnComplete.Broadcast();     });
}

void UTweenAsyncBase::ActivateAdvanced(TypeTween::ITweenControl& Tween) {
	auto& CB = Tween.GetCallbacks();

	/* Connect all callbacks */
	TWeakObjectPtr<UTweenAsyncBase> WeakThis(this);
	CB.OnStart([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnStart.Broadcast();        });
	CB.OnCycleBegin([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnCycleBegin.Broadcast();   });
	CB.OnForwardEnd([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnForwardEnd.Broadcast();   });
	CB.OnReverseBegin([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnReverseBegin.Broadcast(); });
	CB.OnCycleEnd([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnCycleEnd.Broadcast();     });
	CB.OnRepeat([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnRepeat.Broadcast();       });

	ActivateSimple(Tween);
}

void UTweenAsyncBaseSimple::OnTweenComplete() {
	SetReadyToDestroy();
}
