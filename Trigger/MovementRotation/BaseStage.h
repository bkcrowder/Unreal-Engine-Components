#pragma once

#include "BaseStage.generated.h"

class UObject;

USTRUCT(BlueprintType)
struct FBaseStage
{
	GENERATED_BODY();

	FBaseStage(){};

	FBaseStage(FVector StageOffset, bool bStageIsReversible = false)
	{
		Offset = StageOffset;
		bIsReversible = bStageIsReversible;
	}

#pragma region Location
	/// @brief Where to move the vector in relation to its last location
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage")
	FVector Offset = FVector(0,0,0);

	/// @brief Whether or not this stage is reversible
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage")
	bool bIsReversible = true;

#pragma endregion

	UPROPERTY()
	UObject *SafeObjectPointer;
};