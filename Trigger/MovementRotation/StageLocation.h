#pragma once

#include "Stage.h"
#include "StageLocation.generated.h"

class UObject;

USTRUCT(BlueprintType)
struct FStageLocation : public FStage
{
	GENERATED_BODY();

	FStageLocation() : Super(){};

	FStageLocation(FVector StageOffset, float FwdVelocity, bool bStageIsReversible = false, float RevVelocity = 1.0) : Super(FwdVelocity, bStageIsReversible, RevVelocity)
	{
		Offset = StageOffset;
	}

#pragma region Location
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage")
	/// @brief Offset for shifting the actor
	FVector Offset = FVector::Zero();

#pragma endregion
};