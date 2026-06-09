#include "Blueprints/TweenFunctionLibrary.h"
#include "TweenSubsystem.h"

void UTypeTweenLibrary::KillAllTweens(UObject* WorldContextObject) {
	UTweenSubsystem* Sub = UTweenSubsystem::Get(WorldContextObject);
	if (!ensureMsgf(Sub, TEXT("KillAllTweens: UTweenSubsystem not found!"))) {
		return;
	}
	//FIXME: this function CAN be called from within a loop over all tweens.
	Sub->KillAll();
}

void UTypeTweenLibrary::PauseAllTweens(UObject* WorldContextObject) {
	UTweenSubsystem* Sub = UTweenSubsystem::Get(WorldContextObject);
	if (!ensureMsgf(Sub, TEXT("PauseAllTweens: UTweenSubsystem not found!"))) {
		return;
	}
	Sub->PauseTweens();
}

void UTypeTweenLibrary::PauseTween(const FTweenHandle& TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("PauseTween: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	TweenHandle.Handle->Pause();
}

void UTypeTweenLibrary::ResumeTween(const FTweenHandle& TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("ResumeTween: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	TweenHandle.Handle->Resume();
}

void UTypeTweenLibrary::RestartTween(const FTweenHandle& TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("RestartTween: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	TweenHandle.Handle->Restart();
}

void UTypeTweenLibrary::FinishTween(const FTweenHandle& TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("FinishTween: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	TweenHandle.Handle->Finish();
}

void UTypeTweenLibrary::KillTween(const FTweenHandle& TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("KillTween: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	//TweenHandle.Handle->Kill();
}

bool UTypeTweenLibrary::IsDone(const FTweenHandle& TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("IsDone: Tween handle has no tween (nullptr)!"))) {
		return true;
	}
	return TweenHandle.Handle->IsDone();
}

bool UTypeTweenLibrary::IsPlaying(const FTweenHandle& TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("IsPlaying: Tween handle has no tween (nullptr)!"))) {
		return false;
	}
	return !TweenHandle.Handle->IsDone() && !TweenHandle.Handle->IsPaused();
}

bool UTypeTweenLibrary::IsPaused(const FTweenHandle& TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("IsPaused: Tween handle has no tween (nullptr)!"))) {
		return false;
	}
	return TweenHandle.Handle->IsPaused();
}

const FTweenSettings& UTypeTweenLibrary::GetSettings(const FTweenHandle& TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("GetSettings: Tween handle has no tween (nullptr)!"))) {
		static FTweenSettings DefaultSettings;
		return DefaultSettings;
	}
	return TweenHandle.Handle->GetSettings();
}

void UTypeTweenLibrary::SetSettings(UPARAM(ref)FTweenHandle& TweenHandle, const FTweenSettings& Settings) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("SetSettings: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	TweenHandle.Handle->GetSettings() = Settings;
}
