#pragma once

#include "BaseStage.h"
#include "RotationStage.generated.h"

class UObject;

USTRUCT(BlueprintType)
struct FRotationStage : public FBaseStage
{
	GENERATED_BODY();

	FRotationStage() : Super(){};

	FRotationStage(FVector StageOffset, FVector StageForwardVelocity, bool bStageIsReversible = false, FVector StageReverseVelocity = FVector(0,0,0)) : Super(StageOffset, bStageIsReversible)
	{
		ForwardVelocity = StageForwardVelocity;
		ReverseVelocity = StageReverseVelocity;
	}

#pragma region Rotation
	/// @brief Pitch, Yaw, and Roll velocity when rotating "forward"
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage")
	FVector ForwardVelocity;


	/// @brief Pitch, Yaw, and Roll velocity when rotating "backward"
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage", meta = (EditCondition = bIsReversible))
	FVector ReverseVelocity;
#pragma endregion
};