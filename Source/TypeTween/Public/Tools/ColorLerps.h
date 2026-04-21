#pragma once
#include "CoreMinimal.h"
#include "Math/Color.h"

UENUM(BlueprintType)
enum class EColorLerpMode : uint8 {
	/** Naive byte lerp in gamma-encoded (sRGB) space.
	 *  Artist-friendly; matches Photoshop / most paint tools. */
	sRGB UMETA(DisplayName = "sRGB (naive)"),

	/** Lerp directly in linear-light space.
	 *  Physically correct; Unreal Engine default for FLinearColor. */
	Linear UMETA(DisplayName = "Linear (physically correct)"),

	/** Lerp through HSV hue / saturation / value space.
	 *  Travels the shortest arc on the colour wheel. */
	HSV UMETA(DisplayName = "HSV (hue rotation)"),

	/** Lerp in Oklab, a perceptually-uniform colour space.
	 *  Best perceived quality; no unexpected hue or brightness shifts. */
	Oklab UMETA(DisplayName = "Oklab (perceptually uniform)"),
};

/* Equations/functions from Claude.ai */
namespace TypeTween::Detail::ColorLerp {
	FORCEINLINE float LinearToSRGB(float x) {
		x = FMath::Clamp(x, 0.f, 1.f);
		return x <= 0.0031308f
			? x * 12.92f
			: 1.055f * FMath::Pow(x, 1.f / 2.4f) - 0.055f;
	}

	FORCEINLINE float SRGBToLinear(float x) {
		x = FMath::Clamp(x, 0.f, 1.f);
		return x <= 0.04045f
			? x / 12.92f
			: FMath::Pow((x + 0.055f) / 1.055f, 2.4f);
	}

	inline FLinearColor LerpSRGB(const FLinearColor& A, const FLinearColor& B, float T) {
		const FLinearColor EA(LinearToSRGB(A.R), LinearToSRGB(A.G), LinearToSRGB(A.B), A.A);
		const FLinearColor EB(LinearToSRGB(B.R), LinearToSRGB(B.G), LinearToSRGB(B.B), B.A);

		const FLinearColor EResult = FMath::Lerp(EA, EB, T);

		return FLinearColor(
			SRGBToLinear(EResult.R),
			SRGBToLinear(EResult.G),
			SRGBToLinear(EResult.B),
			EResult.A);
	}

	inline FLinearColor LerpLinear(const FLinearColor& A, const FLinearColor& B, float T) {
		return FMath::Lerp(A, B, T);
	}

	inline FLinearColor LerpHSV(const FLinearColor& A, const FLinearColor& B, float T) {
		const FLinearColor HA = A.LinearRGBToHSV();
		const FLinearColor HB = B.LinearRGBToHSV();

		float DeltaH = HB.R - HA.R;
		if (DeltaH > 180.f) DeltaH -= 360.f;
		if (DeltaH < -180.f) DeltaH += 360.f;

		FLinearColor HResult;
		HResult.R = FMath::Fmod(HA.R + DeltaH * T + 360.f, 360.f);
		HResult.G = FMath::Lerp(HA.G, HB.G, T);
		HResult.B = FMath::Lerp(HA.B, HB.B, T);
		HResult.A = FMath::Lerp(A.A, B.A, T);

		FLinearColor Result = HResult.HSVToLinearRGB();
		Result.A = HResult.A;
		return Result;
	}

	struct FOklab { float L, a, b; };

	inline FOklab LinearRGBToOklab(const FLinearColor& C) {
		const float l = 0.4122214708f * C.R + 0.5363325363f * C.G + 0.0514459929f * C.B;
		const float m = 0.2119034982f * C.R + 0.6806995451f * C.G + 0.1073969566f * C.B;
		const float s = 0.0883024619f * C.R + 0.2817188376f * C.G + 0.6299787005f * C.B;

		const float l_ = FMath::Pow(FMath::Max(l, 0.f), 1.f / 3.f);
		const float m_ = FMath::Pow(FMath::Max(m, 0.f), 1.f / 3.f);
		const float s_ = FMath::Pow(FMath::Max(s, 0.f), 1.f / 3.f);

		return FOklab{
			0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_,
			1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_,
			0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_
		};
	}

	inline FLinearColor OklabToLinearRGB(const FOklab& Lab) {
		const float l_ = Lab.L + 0.3963377774f * Lab.a + 0.2158037573f * Lab.b;
		const float m_ = Lab.L - 0.1055613458f * Lab.a - 0.0638541728f * Lab.b;
		const float s_ = Lab.L - 0.0894841775f * Lab.a - 1.2914855480f * Lab.b;

		const float l = l_ * l_ * l_;
		const float m = m_ * m_ * m_;
		const float s = s_ * s_ * s_;

		return FLinearColor(
			+4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
			-1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
			-0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s);
	}

	inline FLinearColor LerpOklab(const FLinearColor& A, const FLinearColor& B, float T) {
		const FOklab OA = LinearRGBToOklab(A);
		const FOklab OB = LinearRGBToOklab(B);

		const FOklab OLerp{
			FMath::Lerp(OA.L, OB.L, T),
			FMath::Lerp(OA.a, OB.a, T),
			FMath::Lerp(OA.b, OB.b, T)
		};

		FLinearColor Result = OklabToLinearRGB(OLerp);
		Result.A = FMath::Lerp(A.A, B.A, T);
		return Result;
	}

	inline FLinearColor Lerp(
		const FLinearColor& A,
		const FLinearColor& B,
		float                T,
		EColorLerpMode       Mode) {
		switch (Mode) {
		case EColorLerpMode::sRGB: return LerpSRGB(A, B, T);
		case EColorLerpMode::HSV: return LerpHSV(A, B, T);
		case EColorLerpMode::Oklab: return LerpOklab(A, B, T);
		case EColorLerpMode::Linear: /* default */
		default: return LerpLinear(A, B, T);
		}
	}
}