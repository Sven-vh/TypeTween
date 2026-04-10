#include "Blueprints/TweenAsyncBase.h"

void UTweenAsyncBase::ActivateCommon(TypeTween::ITweenControl& Tween) {
	//TODO: maybe set the settings?

	auto& CB = Tween.GetCallbacks();

	TWeakObjectPtr<UTweenAsyncBase> WeakThis(this);
	CB.OnStart([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnStart.Broadcast();        });
	CB.OnCycleBegin([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnCycleBegin.Broadcast();   });
	CB.OnForwardEnd([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnForwardEnd.Broadcast();   });
	CB.OnReverseBegin([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnReverseBegin.Broadcast(); });
	CB.OnCycleEnd([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnCycleEnd.Broadcast();     });
	CB.OnRepeat([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnRepeat.Broadcast();       });
	CB.OnComplete([WeakThis]() { if (auto* P = WeakThis.Get()) P->OnComplete.Broadcast();     });

	OnCreated.Broadcast();
}
