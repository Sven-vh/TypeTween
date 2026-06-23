#include "Blueprints/TweenFunctionLibrary.h"
#include "TweenSubsystem.h"

void UTypeTweenLibrary::PauseTween(FTweenHandle TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("PauseTween: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	TweenHandle.Handle->Pause();
}

void UTypeTweenLibrary::ResumeTween(FTweenHandle TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("ResumeTween: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	TweenHandle.Handle->Resume();
}

void UTypeTweenLibrary::RestartTween(FTweenHandle TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("RestartTween: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	TweenHandle.Handle->Restart();
}

void UTypeTweenLibrary::FinishTween(FTweenHandle TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("FinishTween: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	TweenHandle.Handle->Finish();
}

void UTypeTweenLibrary::KillTween(FTweenHandle TweenHandle) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("KillTween: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	TweenHandle.Handle->Kill();
}

bool UTypeTweenLibrary::IsValid(const FTweenHandle& TweenHandle) {
	if (!TweenHandle.Handle) {
		return false;
	}
	return true;
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

void UTypeTweenLibrary::SetSettings(FTweenHandle TweenHandle, const FTweenSettings& Settings) {
	if (!ensureMsgf(TweenHandle.Handle, TEXT("SetSettings: Tween handle has no tween (nullptr)!"))) {
		return;
	}
	TweenHandle.Handle->GetSettings() = Settings;
}
