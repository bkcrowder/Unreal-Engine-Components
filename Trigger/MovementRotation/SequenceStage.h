#pragma once

#include "LocationStage.h"
#include "RotationStage.h"
#include "SequenceStage.generated.h"

class UObject;

USTRUCT(BlueprintType)
struct FSequenceStage
{
	GENERATED_BODY();

	FSequenceStage(){};

	FSequenceStage(FLocationStage StageLocation, FRotationStage StageRotation, bool bStageIsReversible = false)
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
	FLocationStage Location;

	/// @brief Where to move the vector in relation to its last location
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Movement")
	FRotationStage Rotation;

	UPROPERTY()
	UObject *SafeObjectPointer;
};