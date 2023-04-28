#pragma once

#include "Stage.generated.h"

class UObject;

USTRUCT(BlueprintType)
struct FStage
{
	GENERATED_BODY();

	FStage(){};

	FStage(float FwdVelocity, bool bStageIsReversible = false, float RevVelocity = 1.0)
	{
		ForwardVelocity = FwdVelocity;
		bIsReversible = bStageIsReversible;
		ReverseVelocity = RevVelocity;
	};

#pragma region Rotation
	/// @brief Whether or not this stage is reversible
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage")
	bool bIsReversible = true;

	/// @brief How quickly to update the location during a forward trigger
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage")
	float ForwardVelocity = 1.0;

	/// @brief How quickly to update the location when reversing
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Stage", meta = (EditCondition = bIsReversible))
	float ReverseVelocity = 1.0;
#pragma endregion

	UPROPERTY()
	UObject *SafeObjectPointer;
};