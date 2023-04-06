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

		RotOffset = FRotator(Offset.X, Offset.Y, Offset.Z);
	}

	/// @brief Updates the rotation offset using the current FVector Offset
	/// @return Updated Rotation Offset
	FRotator UpdateRotationFromVector()
	{
		return RotOffset = FRotator(Offset.Y, Offset.Z, Offset.X);
	}

	/// @brief Updates the rotation based on the supplied FVector
	/// @param NewOffset New Offset series transposed from X, Y, Z to Pitch, Yaw, Roll
	/// @return Updated Rotation Offset
	FRotator UpdateRotationFromVector(FVector NewOffset)
	{
		return RotOffset = FRotator(NewOffset.Y, NewOffset.Z, NewOffset.X);
	}

#pragma region Rotation
	/// @brief Vector represented as a rotator. Enables us better accuracy in taking supplied offset values than a direct rotator
	FRotator RotOffset = FRotator(0.0, 0.0, 0.0);

	/// @brief Pitch, Yaw, and Roll velocity when rotating "forward"
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage")
	FVector ForwardVelocity;


	/// @brief Pitch, Yaw, and Roll velocity when rotating "backward"
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage", meta = (EditCondition = bIsReversible))
	FVector ReverseVelocity;
#pragma endregion
};