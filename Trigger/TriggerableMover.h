#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MovementRotation/SequenceStage.h"
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
	TArray<FSequenceStage> GetSequence() const { return Sequence; }
#pragma endregion

	/// @brief Add a series of movements and rotations to the end of the existing sequence
	/// @param MovementAndRotation Array of movements and rotations
	UFUNCTION(BlueprintCallable, Category = "Triggerable Sequence", meta = (DisplayName = "Append Triggerable Sequence", CompactNodeTitle = "APPTRIGSEQ", ArrayParam = "SequenceArray"))
	void AppendSequenceStages(TArray<FSequenceStage> const &SequenceStages);

	/// @brief Add a movement and rotation to the end of the existing sequence
	/// @param MovementAndRotation Struct of the movement and rotation
	UFUNCTION(BlueprintCallable, Category = "Triggerable Sequence", meta = (DisplayName = "Add Triggerable Sequence Stage", CompactNodeTitle = "ADDTRIGSEQ"))
	void AddStageToSequence(FSequenceStage const &SequenceStage);

	/// @brief Sets the sequence to the supplied movement and rotation array
	/// @param MovementAndRotation Array of movements and rotations
	UFUNCTION(BlueprintCallable, Category = "Triggerable Sequence", meta = (DisplayName = "Set Triggerable Sequence", CompactNodeTitle = "SETTRIGSEQ", ArrayParam = "SequenceArray"))
	void SetSequence(TArray<FSequenceStage> const &SequenceStages);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

#pragma region Members
	/// @brief Whether or not this mover is active. If not active, it will not move
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Triggerable", meta=(AllowPrivateAccess = "true"))
	bool bActive = true;

	// TODO: Remove
	/// @brief Forces the triggerable to ignore the reversible flag in every stage in the sequence. Every stage will be reversible.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Triggerable", meta=(AllowPrivateAccess = "true"))
	bool bForceReverseSequence = false;

	/// @brief Whether or not to loop this sequence forever
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Triggerable", meta=(AllowPrivateAccess = "true"))
	bool bLoopForever = false;

	/// @brief Origin Location Return (Reverse) Speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Triggerable | Origin Sequence Stage", meta=(AllowPrivateAccess = "true"))
	float OriginLocationReturnVelocity = 1.0;

	/// @brief Origin Rotation Return (Reverse) Speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Triggerable | Origin Sequence Stage", meta=(AllowPrivateAccess = "true"))
	FVector OriginRotationReturnVelocity = FVector(1, 1, 1);

	/// @brief Whether or not this triggerable has been triggered
	bool bHasTriggered = false;

	/// @brief Determines whether or not the movement/rotation is currently reversing
	bool bIsReversing = false;

	/// @brief Whether or not this triggerable has completed a cycle
	bool bHasCompleted = false;

	/// @brief Current index of the stage of movement/rotation
	int32 StageIndex = 0;

	/// @brief Sequence of movements and rotations via FVector and FRotator
	TArray<FSequenceStage> Sequence;

	/// @brief Origin sequence stage. Gets created on begin play.
	FSequenceStage OriginSequenceStage;

	/// @brief How much to step rotation evaluations in order to keep rotating to 360 degrees and beyond
	const double RotationStep = 180.0;

#pragma region Location Tracking
	/// @brief Original World Location of Mover
	FVector OriginLocation;

	/// @brief Original World Rotation of Mover
	FRotator OriginRotation;

	/// @brief Target Location for the current stage
	FVector CurrentLocationTarget;

	/// @brief Target Rotation for the current stage
	FRotator CurrentRotationTarget;

	/// @brief Current amount of rotation applied in the current stage
	FRotator RotationRemaining = FRotator(0,0,0);

	/// @brief Target Location for the current stage
	FVector PreviousLocationTarget;

	/// @brief Target Rotation for the current stage
	FRotator PreviousRotationTarget;

#pragma endregion
#pragma endregion

	/// @brief Loop the movement and rotation, flipping the trigger/reverse values
	void Loop();

	/// @brief Perform the movement actions
	void Move(const float DeltaTime, const FVector CurrentLocation, const FLocationStage& CurrentStage, bool bReverse);

	/// @brief Perform the rotation action
	void Rotate(const float DeltaTime, const FRotator CurrentRotation, const FRotationStage& CurrentStage, bool bReverse);

	/// @brief Performs the movement and rotation based on the sequence
	/// @param Reverse Whether or not to reverse the sequence
	void MoveAndRotate(const float DeltaTime, bool Reverse = false);

	/// @brief Update the current sequence stage and completed stage tracking
	/// @param CurrentLocation Current Location of the actor
	/// @param CurrentRotation Current Rotation of the actor
	/// @param bReverse Whether or not we are reversing which determines movement index update
	void UpdateStages(const FVector CurrentLocation, const FRotator CurrentRotation, bool bReverse = false);

	/// @brief Updates the stage targets for Location and Rotation
	/// @param CurrentLocation Current Location of the actor
	/// @param CurrentRotation Current Rotation of the actor
	/// @param Direction Direction to traverse stages represented as 1 or -1
	void SetStageTargets(const FVector CurrentLocation, const FRotator CurrentRotation, const int32 Direction);

	/// @brief Tracks the force supplied to the rotation, measured in 180 degree intervals
	/// @param force Yaw, Pitch, or Roll of the force to track
	/// @return The remaining force target for the supplied yaw, pitch, or roll
	float TrackRotationForce(const double TrackedForce);

	/// @brief Tracks the current rotation by intervals of 180 degrees and updates the rotation target
	FRotator TrackRotationStep(const FRotator CurrentRotation);
};
