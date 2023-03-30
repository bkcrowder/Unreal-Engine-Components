#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovementRotationStage.h"
#include "ITriggerable.h"
#include "TriggerableMover.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MPSTARTER_API UTriggerableMover : public UActorComponent, public IITriggerable
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTriggerableMover();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/// @brief  Activates the mover, starting it from its current point in the sequence
	void Activate_Implementation();

	/// @brief Deactivates the mover, stopping it in place
	void Deactivate_Implementation();

	/// @brief Trigger the event
	void Trigger_Implementation() override;

	/// @brief Reverses the trigger event
	void Reverse_Implementation() override;

#pragma region Getters
	bool IsActive_Implementation() const override { return bActive; }

	/// @brief Triggerable is in a triggered state
	/// @return Whether or not the triggerable is in a triggered state
	bool IsTriggered_Implementation() const override { return bHasTriggered; };

	/// @brief Triggerable is currently reversing
	/// @return Whether or not the triggerable is in a reversing state
	bool IsReversing_Implementation() const override { return bIsReversing; };

	/// @brief Triggerable is completed with its triggering process (manipulation)
	/// @return Whether or not the triggerable is considered "finished" with its process
	bool IsDone_Implementation() const override { return bHasCompleted; };

	/// @brief Retrieve the sequence of movements and rotations this mover will perform
	/// @return Returns an array of movement and rotation sequences in the order it was created
	TArray<FMovementRotationStage*> GetSequence() const { return Sequence; }
#pragma endregion

	/// @brief Add a series of movements and rotations to the end of the existing sequence
	/// @param MovementAndRotation Array of movements and rotations
	void AddToSequence(TArray<FMovementRotationStage *> const MovementAndRotation);

	/// @brief Add a movement and rotation to the end of the existing sequence
	/// @param MovementAndRotation Struct of the movement and rotation
	void AddToSequence(FMovementRotationStage * const MovementAndRotation);

	/// @brief Sets the sequence to the supplied movement and rotation array
	/// @param MovementAndRotation Array of movements and rotations
	void SetSequence(TArray<FMovementRotationStage *> const MovementAndRotation);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	/// @brief Whether or not this mover is active. If not active, it will not move
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Triggerable", meta=(AllowPrivateAccess = "true"))
	bool bActive = true;

	/// @brief Forces the triggerable to ignore the reversible flag in every stage in the sequence. Every stage will be reversible.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Triggerable", meta=(AllowPrivateAccess = "true"))
	bool bForceReverseSequence = false;

	/// @brief Whether or not to loop this sequence forever
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Triggerable", meta=(AllowPrivateAccess = "true"))
	bool bLoopForever = false;

	/// @brief Whether or not this triggerable has been triggered
	bool bHasTriggered = false;

	/// @brief Determines whether or not the movement/rotation is currently reversing
	bool bIsReversing = false;

	/// @brief Whether or not this triggerable has completed a cycle
	bool bHasCompleted = false;

	/// @brief Current index of the stage of movement/rotation
	int32 MovementIndex = 0;

	/// @brief Sequence of movements and rotations via FVector and FRotator
	TArray<FMovementRotationStage*> Sequence;

#pragma region Location Tracking
	/// @brief Original World Location of Mover
	FVector OriginLocation;

	/// @brief Original World Rotation of Mover
	FRotator OriginRotation;

	/// @brief Target Location for the current stage
	FVector CurrentStageLocationTarget;

	/// @brief Target Rotation for the current stage
	FRotator CurrentStageRotationTarget;

	/// @brief Current World location of the previously completed stage.
	FVector CompletedStageLocation;

	/// @brief Current World rotation of the previously completed stage.
	FRotator CompletedStageRotation;

#pragma endregion

	/// @brief Performs the movement and rotation based on the sequence
	/// @param Reverse Whether or not to reverse the sequence
	void MoveAndRotate(float DeltaTime, bool Reverse = false);

	/// @brief Loop the movement and rotation, flipping the trigger/reverse values
	void Loop();

	/// @brief Perform the movement actions
	void Move(float DeltaTime, FVector CurrentLocation, FMovementRotationStage* CurrentStage, bool bReverse);

	/// @brief Perform the rotation action
	void Rotate(float DeltaTime, FRotator CurrentRotation, FMovementRotationStage* CurrentStage, bool bReverse);

	/// @brief Update the current sequence stage and completed stage tracking
	/// @param CurrentLocation Current Location of the actor
	/// @param CurrentRotation Current Rotation of the actor
	/// @param bReverse Whether or not we are reversing which determines movement index update
	void UpdateStageAndCompletion(FVector CurrentLocation, FRotator CurrentRotation, bool bReverse = false);

	/// @brief Retrieve the target location for movement
	/// @param StageLocation Vector offset to move for the supplied sequence stage
	/// @param Reverse Whether or not we are reversing
	/// @return Vector of the target to move to
	FVector GetStageTargetLocation(FVector StageLocation, bool bReverse = false);
	
	/// @brief Retrieve the target rotation
	/// @param StageRotation Rotation offset to rotate for the supplied sequence stage
	/// @param Reverse Whether or not we are reversing
	/// @return Rotation of the target to rotate to
	FRotator GetStageTargetRotation(FRotator StageRotation, bool bReverse = false);
};
