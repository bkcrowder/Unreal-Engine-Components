#pragma once

//#include "CoreMinimal.h"
#include "MovementRotationStage.generated.h"

class UObject;

USTRUCT(BlueprintType)
struct FMovementRotationStage
{
	GENERATED_BODY();

	/// @brief Whether or not this stage is reversible
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable")
	bool bIsReversible = true;

#pragma region Location
	/// @brief Where to move the vector in relation to its last location
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Movement")
	FVector MovementOffset = FVector(0,0,0);

	/// @brief How quickly to update the location during a forward trigger
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Movement")
	float MoveForwardSpeed = 1.0;

	/// @brief Whether or not this stage is reversible
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Movement", meta = (EditCondition = "bIsReversible"))
	bool bReverseLocationOnReverse = true;

	/// @brief How quickly to update the location when reversing
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Movement", meta = (EditCondition = "bIsReversible && bReverseLocationOnReverse"))
	float MoveReverseSpeed = 1.0;
#pragma endregion

#pragma region Rotation
	/// @brief How to rotate the vector in relation it its current rotation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Rotation")
	FRotator RotationOffset = FRotator(0,0,0);

	/// @brief How quickly to update the rotation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Rotation")
	float RotationForwardSpeed = 1.0;

	/// @brief Whether or not this stage is reversible
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Rotation", meta = (EditCondition = "bIsReversible"))
	bool bReverseRotationOnReverse = true;

	/// @brief How quickly to update the rotation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Triggerable | Rotation", meta = (EditCondition = "bIsReversible && bReverseRotationOnReverse"))
	float RotationReverseSpeed = 1.0;
#pragma endregion

	UPROPERTY()
	UObject *SafeObjectPointer;
};