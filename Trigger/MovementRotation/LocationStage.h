#pragma once

#include "BaseStage.h"
#include "LocationStage.generated.h"

class UObject;

USTRUCT(BlueprintType)
struct FLocationStage : public FBaseStage
{
	GENERATED_BODY();

	FLocationStage() : Super(){};

	FLocationStage(FVector StageOffset, float StageForwardVelocity, bool bStageIsReversible = false, float StageReverseVelocity = 1.0) : Super(StageOffset, bStageIsReversible)
	{
		ForwardVelocity = StageForwardVelocity;
		ReverseVelocity = StageReverseVelocity;
	}

#pragma region Location
	/// @brief How quickly to update the location during a forward trigger
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Movement")
	float ForwardVelocity = 1.0;

	/// @brief How quickly to update the location when reversing
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Movement", meta = (EditCondition = bIsReversible))
	float ReverseVelocity = 1.0;
#pragma endregion
};