#pragma once

#include "Stage.h"
#include "StageRotation.generated.h"

class UObject;

USTRUCT(BlueprintType)
struct FStageRotation : public FStage
{
	GENERATED_BODY();

	FStageRotation() : Super(){};

	FStageRotation(FVector StageOffset, float StageForwardVelocity, bool bStageIsReversible = false, float StageReverseVelocity = 1.0) : Super(StageForwardVelocity, bStageIsReversible, StageReverseVelocity)
	{
		Offset = StageOffset;
	};

	FStageRotation(double Pitch, double Yaw, double Roll, float StageForwardVelocity = 1.0, bool bStageIsReversible = false, float StageReverseVelocity = 1.0) : Super(StageForwardVelocity, bStageIsReversible, StageReverseVelocity)
	{
		PitchOffset = Pitch;
		YawOffset = Yaw;
		RollOffset = Roll;

		Offset = FVector(RollOffset, PitchOffset, YawOffset);
	};

#pragma region Rotation
	/// @brief Offset for shifting the actor
	FVector Offset = FVector::Zero();

	/// @brief Offset for the actor's pitch
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage")
	float PitchOffset = 0.0;
	
	/// @brief Offset for the actor's yaw
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage")
	float YawOffset = 0.0;

	/// @brief Offset to roll the actor
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage")
	float RollOffset = 0.0;
#pragma endregion

public:
	FVector ToVector()
	{
		Offset = FVector(RollOffset, PitchOffset, YawOffset);
		return Offset;
	}
};