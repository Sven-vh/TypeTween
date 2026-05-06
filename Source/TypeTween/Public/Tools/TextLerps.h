#pragma once
#include "CoreMinimal.h"
#include "Algo/Reverse.h"

UENUM(BlueprintType)
enum class ETextLerpMode : uint8 {
	/** Reveals destination left-to-right. Source is ignored entirely. */
	Reveal			UMETA(DisplayName = "Reveal"),
	/** Deletes source left-to-right, then types destination. Passes through empty at T=0.5. */
	DeleteAndType	UMETA(DisplayName = "Delete & Type"),
	/** Characters show random glyphs, then resolve into destination over time. */
	Scramble		UMETA(DisplayName = "Scramble (glitch)"),
	/** Minimal edit operations via Levenshtein. Best for similar strings. */
	EditDistance	UMETA(DisplayName = "Edit Distance (Levenshtein)"),
	/** Interpolates Unicode code points between source and destination characters. */
	CharCode		UMETA(DisplayName = "Char Code (alphabet slide)"),
};

UENUM(BlueprintType)
enum class ETextGlyphSet : uint8 {
	Alphanumeric	UMETA(DisplayName = "Alphanumeric (A-Z, a-z, 0-9)"),
	Uppercase		UMETA(DisplayName = "Uppercase (A-Z)"),
	Lowercase		UMETA(DisplayName = "Lowercase (a-z)"),
	Symbols			UMETA(DisplayName = "Symbols (!@#$...)"),
	Custom			UMETA(DisplayName = "Custom"),
};

/* Equations/functions from Claude.ai */
namespace TypeTween::Detail::TextLerp {
	// --- Helpers -------------------------------------------------------------

	inline FString GetGlyphs(ETextGlyphSet GlyphSet, const FString& Custom) {
		switch (GlyphSet) {
		case ETextGlyphSet::Uppercase:		return TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		case ETextGlyphSet::Lowercase:		return TEXT("abcdefghijklmnopqrstuvwxyz");
		case ETextGlyphSet::Symbols:		return TEXT("!@#$%^&*()_+-=[]{}|;:,.<>?");
		case ETextGlyphSet::Custom:			return Custom.IsEmpty() ? TEXT("?") : Custom;
		case ETextGlyphSet::Alphanumeric:
		default:							return TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
		}
	}

	FORCEINLINE TCHAR RandomGlyph(const FString& Glyphs) {
		if (Glyphs.IsEmpty()) return TEXT('?');
		return Glyphs[FMath::RandRange(0, Glyphs.Len() - 1)];
	}

	// --- Mode Implementations -------------------------------------------------

	/** Ignores From. Reveals To left-to-right as T goes 0->1. */
	inline FString LerpReveal(const FString& From, const FString& To, float T) {
		return To.Left(FMath::RoundToInt(T * To.Len()));
	}

	/** Phase 1 (T 0->0.5): delete From left-to-right.
	 *  Phase 2 (T 0.5->1): type To left-to-right.
	 *  String passes through empty at exactly T=0.5. */
	inline FString LerpDeleteAndType(const FString& From, const FString& To, float T) {
		if (T <= 0.5f) {
			const int32 Keep = FMath::RoundToInt((1.f - T * 2.f) * From.Len());
			return From.Left(Keep);
		} else {
			const int32 Show = FMath::RoundToInt((T * 2.f - 1.f) * To.Len());
			return To.Left(Show);
		}
	}

	/** Length lerps from From.Len() to To.Len().
	 *  Each char at index i resolves (stops scrambling) when T > i / To.Len(),
	 *  creating a natural left-to-right resolution without needing a stagger param. */
	inline FString LerpScramble(const FString& From, const FString& To, float T, const FString& Glyphs) {
		if (To.IsEmpty()) return FString();

		const int32 ToLen = To.Len();
		const int32 Len = FMath::RoundToInt(FMath::Lerp((float)From.Len(), (float)ToLen, T));

		FString Result;
		Result.Reserve(Len);

		for (int32 i = 0; i < Len; ++i) {
			// Char i locks in once T has passed its proportional threshold
			const float Threshold = (float)i / (float)ToLen;
			if (T >= Threshold && i < ToLen)
				Result.AppendChar(To[i]);
			else
				Result.AppendChar(RandomGlyph(Glyphs));
		}

		return Result;
	}

	/** Each character position slides through Unicode code points from
	 *  its source value to its destination value. Length lerps too. */
	inline FString LerpCharCode(const FString& From, const FString& To, float T) {
		const int32 FromLen = From.Len();
		const int32 ToLen = To.Len();
		const int32 Len = FMath::RoundToInt(FMath::Lerp((float)FromLen, (float)ToLen, T));

		FString Result;
		Result.Reserve(Len);

		for (int32 i = 0; i < Len; ++i) {
			// When a position doesn't exist in one string, clamp to that string's last char
			const int32 SrcCode = (i < FromLen) ? (int32)From[i] : (int32)To[FMath::Min(i, ToLen - 1)];
			const int32 DstCode = (i < ToLen) ? (int32)To[i] : (int32)From[FMath::Min(i, FromLen - 1)];
			const int32 Code = FMath::RoundToInt(FMath::Lerp((float)SrcCode, (float)DstCode, T));

			// Clamp to printable ASCII — extend range if Unicode support is needed
			Result.AppendChar((TCHAR)FMath::Clamp(Code, 32, 126));
		}

		return Result;
	}

	// --- Levenshtein ----------------------------------------------------------

	struct FEditOp {
		enum class EType : uint8 { Sub, Ins, Del };
		EType Type;
		int32 Pos; // position relative to the original From string
		TCHAR Ch;  // target character for Sub and Ins
	};

	/** Computes the minimum edit path from From to To.
	 *  Returns only the non-keep operations in forward order.
	 *  Should be called once on tween start and cached — O(M*N) time and space. */
	inline TArray<FEditOp> ComputeEditOps(const FString& From, const FString& To) {
		const int32 M = From.Len();
		const int32 N = To.Len();

		// Flat DP table: (M+1) x (N+1)
		// Op encoding: 0=origin, 1=keep, 2=sub, 3=del, 4=ins
		struct FCell { int32 Cost; uint8 Op; };
		TArray<FCell> DP;
		DP.SetNum((M + 1) * (N + 1));

		auto At = [&](int32 i, int32 j) -> FCell& { return DP[i * (N + 1) + j]; };

		At(0, 0) = { 0, 0 };
		for (int32 i = 1; i <= M; ++i) At(i, 0) = { i, 3 }; // pure deletes
		for (int32 j = 1; j <= N; ++j) At(0, j) = { j, 4 }; // pure inserts

		for (int32 i = 1; i <= M; ++i) {
			for (int32 j = 1; j <= N; ++j) {
				const bool  bSame = From[i - 1] == To[j - 1];
				const int32 DiagCost = At(i - 1, j - 1).Cost + (bSame ? 0 : 1);
				const int32 DelCost = At(i - 1, j).Cost + 1;
				const int32 InsCost = At(i, j - 1).Cost + 1;

				if (bSame && DiagCost <= DelCost && DiagCost <= InsCost)
					At(i, j) = { DiagCost, 1 }; // keep
				else if (DiagCost <= DelCost && DiagCost <= InsCost)
					At(i, j) = { DiagCost, 2 }; // sub
				else if (DelCost <= InsCost)
					At(i, j) = { DelCost,  3 }; // del
				else
					At(i, j) = { InsCost,  4 }; // ins
			}
		}

		// Traceback from (M,N) -> (0,0), collect ops in reverse, then flip
		TArray<FEditOp> Ops;
		int32 i = M, j = N;
		while (i > 0 || j > 0) {
			switch (At(i, j).Op) {
			case 1: --i; --j; break; // keep — no op recorded
			case 2: { FEditOp Op; Op.Type = FEditOp::EType::Sub; Op.Pos = i - 1; Op.Ch = To[j - 1]; Ops.Add(Op); --i; --j; break; }
			case 3: { FEditOp Op; Op.Type = FEditOp::EType::Del; Op.Pos = i - 1; Op.Ch = TEXT('\0'); Ops.Add(Op); --i;      break; }
			case 4: { FEditOp Op; Op.Type = FEditOp::EType::Ins; Op.Pos = i;     Op.Ch = To[j - 1]; Ops.Add(Op);      --j; break; }
			default: i = 0; j = 0; break; // malformed table, bail
			}
		}

		Algo::Reverse(Ops);
		return Ops;
	}

	/** Applies the first StepCount edit operations to From, tracking an offset
	 *  so positions stay valid after prior insertions and deletions. */
	inline FString ApplyEditOps(const FString& From, const TArray<FEditOp>& Ops, int32 StepCount) {
		FString Result = From;
		int32   Offset = 0;
		const int32 Steps = FMath::Min(StepCount, Ops.Num());

		for (int32 k = 0; k < Steps; ++k) {
			const FEditOp& Op = Ops[k];
			const int32    Pos = Op.Pos + Offset;

			switch (Op.Type) {
			case FEditOp::EType::Sub:
				if (Pos >= 0 && Pos < Result.Len())
					Result[Pos] = Op.Ch;
				break;

			case FEditOp::EType::Ins:
				if (Pos >= 0 && Pos <= Result.Len())
					Result.InsertAt(Pos, Op.Ch);
				++Offset;
				break;

			case FEditOp::EType::Del:
				if (Pos >= 0 && Pos < Result.Len())
					Result.RemoveAt(Pos, 1, false);
				--Offset;
				break;
			}
		}

		return Result;
	}

	inline FString LerpEditDistance(const FString& From, const FString& To, float T, const TArray<FEditOp>& CachedOps) {
		if (CachedOps.IsEmpty()) return To;
		const int32 Step = FMath::RoundToInt(T * CachedOps.Num());
		return ApplyEditOps(From, CachedOps, Step);
	}

	// --- Dispatch --------------------------------------------------------------

	inline FString Lerp(
		const FString& From,
		const FString& To,
		float						T,
		ETextLerpMode				Mode,
		const FString& Glyphs,
		const TArray<FEditOp>& CachedOps) {
		switch (Mode) {
		case ETextLerpMode::Reveal:        return LerpReveal(From, To, T);
		case ETextLerpMode::DeleteAndType: return LerpDeleteAndType(From, To, T);
		case ETextLerpMode::Scramble:      return LerpScramble(From, To, T, Glyphs);
		case ETextLerpMode::CharCode:      return LerpCharCode(From, To, T);
		case ETextLerpMode::EditDistance:  return LerpEditDistance(From, To, T, CachedOps);
		default:                           return LerpScramble(From, To, T, Glyphs);
		}
	}

} // namespace TypeTween::Detail::TextLerp