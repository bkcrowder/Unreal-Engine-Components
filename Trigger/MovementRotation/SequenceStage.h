#pragma once

#include "StageLocation.h"
#include "StageRotation.h"
#include "SequenceStage.generated.h"

class UObject;

USTRUCT(BlueprintType)
struct FSequenceStage
{
	GENERATED_BODY();

	FSequenceStage(){};

	FSequenceStage(FStageLocation StageLocation, FStageRotation StageRotation, bool bStageIsReversible = false)
	{
		Location = StageLocation;
		Rotation = StageRotation;
		bIsReversible = bStageIsReversible;
	}

	/// @brief Whether or not this sequence stage is reversible
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable")
	bool bIsReversible = true;

	/// @brief Where to move the vector in relation to its last location
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Movement")
	FStageLocation Location;

	/// @brief Where to move the vector in relation to its last location
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Movement")
	FStageRotation Rotation;

	UPROPERTY()
	UObject *SafeObjectPointer;
};