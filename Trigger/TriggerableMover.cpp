#include "TriggerableMover.h"

/*
	TODO:
	- Fix rotation velocity as it's still a bit fast and inverted from location velocity (loc = higher -> slow; rot = higher -> faster)
	- Delegates where TODOs are flagged
	- Permit a rotation to loop forever
		- Need to flag the Quat rotation on the stage
		- Disallow any other rotations to occur when FQuat is flagged
		- Continue rotating on those axes indefinitely
*/

// Sets default values for this component's properties
UTriggerableMover::UTriggerableMover()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTriggerableMover::BeginPlay()
{
	Super::BeginPlay();

	OriginLocation = CurrentLocationTarget = PreviousLocationTarget = GetOwner()->GetActorLocation();
	OriginRotation = GetOwner()->GetActorQuat();

	CurrentRotationTarget = PreviousRotationTarget = OriginRotation;

	OriginSequenceStage = FSequenceStage(
		FStageLocation(FVector::Zero(), OriginLocationReturnVelocity), 
		FStageRotation(FVector::Zero(), OriginRotationReturnVelocity));
	AddStageToSequence(OriginSequenceStage);
}

// Called every frame
void UTriggerableMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bActive)
	{
		MoveAndRotate(DeltaTime, bIsReversing);
	}
}

#pragma region Sequence Setting
void UTriggerableMover::AppendSequenceStages(TArray<FSequenceStage> const &SequenceStages)
{
	Sequence.Append(SequenceStages);
}

void UTriggerableMover::AddStageToSequence(FSequenceStage const &SequenceStage)
{
	Sequence.Add(SequenceStage);
}

void UTriggerableMover::SetSequence(TArray<FSequenceStage> const &SequenceStages)
{
	Sequence.Empty();
	Sequence.Add(OriginSequenceStage);
	AppendSequenceStages(SequenceStages);
}
#pragma endregion

void UTriggerableMover::Activate_Implementation()
{
	bActive = true;

	// TODO: Event Dispatcher OnActivated
}

void UTriggerableMover::Deactivate_Implementation()
{
	bActive = false;

	// TODO: Event Dispatcher OnDeactivated
}

void UTriggerableMover::MoveAndRotate(const float DeltaTime, bool bReverse)
{
	// Sequence is empty or this is in an untouched state
	if (Sequence.Num() == 0 || (!bHasTriggered && !bIsReversing) || (bHasCompleted && !bLoopForever))
	{
		return;
	}

	// Current Actor State
	FVector CurrentLocation = GetOwner()->GetActorLocation();
	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	// Update the stage and completion if we've reached stage destination
	UpdateStages(CurrentLocation, CurrentRotation, bReverse);

	// Current stage in sequence
	FSequenceStage CurrentStage = Sequence[StageIndex];

	// Only move if we have a non-zero FVector
	Move(DeltaTime, CurrentLocation, CurrentStage.Location, bReverse);
	Rotate(DeltaTime, CurrentRotation, CurrentStage.Rotation, bReverse);
}

void UTriggerableMover::Move(const float DeltaTime, const FVector &CurrentLocation, const FStageLocation& StageLocation, bool bReverse)
{

	// Early return if location is done OR we're in reverse and location is not allowed to reverse
	if ((CurrentLocationTarget - CurrentLocation).IsNearlyZero() || (bReverse && !StageLocation.bIsReversible))
	{
		return;
	}

	// Movement Speed
	float DirectionSpeed = bReverse && StageLocation.bIsReversible ? StageLocation.ReverseVelocity : StageLocation.ForwardVelocity;
	float Speed = StageLocation.Offset.Size() / DirectionSpeed;

	FVector InterpLocation = FMath::VInterpConstantTo(CurrentLocation, CurrentLocationTarget, DeltaTime, Speed);
	GetOwner()->SetActorLocation(InterpLocation);
}

void UTriggerableMover::Rotate(const float DeltaTime, const FRotator &CurrentRotation, const FStageRotation& StageRotation, bool bReverse)
{
	bool bReversePermitted = bForceReverseSequence || StageRotation.bIsReversible;

	if (RotationRemaining.IsZero() || (bReverse && !bReversePermitted))
	{
		return;
	}

	// Retrieve the correction direction based on if the sequence is reversing and the stage rotation is allowed to reverse
	float DirectionSpeed = bReverse && StageRotation.bIsReversible ? StageRotation.ReverseVelocity : StageRotation.ForwardVelocity;
	float Speed = StageRotation.Offset.Size() / (DirectionSpeed / DeltaTime);

	FQuat CurrentQuat = CurrentRotation.Quaternion();	
	FQuat InterpQuat = FMath::QInterpConstantTo(CurrentQuat, CurrentRotationTarget, DeltaTime, Speed);
	
	// Apply InterpQuat (Current + interval to target) first, and then the inverse of current to get delta
	FQuat DeltaQuat = InterpQuat * CurrentQuat.Inverse();

	// Soley compare to zero as values can be positive or negative depending on user input
	FVector Delta = DeltaQuat.Euler();
	RotationRemaining.X = RotationRemaining.X == 0.0 ? 0.0 : RotationRemaining.X - Delta.X;
	RotationRemaining.Y = RotationRemaining.Y == 0.0 ? 0.0 : RotationRemaining.Y - Delta.Y;
	RotationRemaining.Z = RotationRemaining.Z == 0.0 ? 0.0 : RotationRemaining.Z - Delta.Z;

	// Next step will land in the negative, so zero out and use the difference as the remaining interpolated rotation
	if (RotationRemaining.IsNearlyZero())
	{
		RotationRemaining = FVector::Zero();
		GetOwner()->SetActorRotation(CurrentRotationTarget, ETeleportType::TeleportPhysics);
	}

	// TODO: World Rotation appears to flip once it reaches 360 degrees
	// GetOwner()->SetActorRotation(InterpQuat)
	GetOwner()->AddActorWorldRotation(DeltaQuat, false, nullptr, ETeleportType::TeleportPhysics);
}

void UTriggerableMover::UpdateStages(const FVector &CurrentLocation, const FRotator &CurrentRotation, bool bReverse)
{
	int32 Direction = bReverse ? -1 : 1;

	// Check the rotation overflow still needed and return without changing stages
	if (CurrentRotationTarget.Equals(CurrentRotation.Quaternion()) && !RotationRemaining.IsZero())
	{
		// Add the next step in the rotation, broken up into 180 degree capped chunks only if necessary
		FQuat Step = TrackRotationStep(RotationRemaining, RotationStep, RotationTolerance) * Direction;
		CurrentRotationTarget = CurrentRotation.Quaternion() * Step * Direction;
	}

	// We've reached our destination and no rotation remaining, so increment/decrement stage
	if ((CurrentLocationTarget - CurrentLocation).IsNearlyZero() && RotationRemaining.IsZero())
	{
		// Update index and ensure we don't move outside of the Array boundaries
		StageIndex += Direction;
		StageIndex = FMath::Clamp(StageIndex, 0, Sequence.Num() - 1);

		// TODO: Event Dispatcher on bHasCompleted if bHasCompleted == true and we reside at the stage location/rotation
		bHasCompleted = bReverse ? StageIndex == 0 : StageIndex == Sequence.Num() - 1;

		if (bHasCompleted)
		{
			Loop();
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("--- Setting Targets ---"));
		SetStageTargets(CurrentLocation, CurrentRotation, Direction);
	}
}

void UTriggerableMover::SetStageTargets(const FVector &CurrentLocation, const FRotator &CurrentRotation, const int32 Direction)
{
	if (Direction != 1 && Direction != -1)
	{
		return;
	}

	PreviousLocationTarget = CurrentLocationTarget;
	PreviousRotationTarget = CurrentRotationTarget;

	RotationRemaining = Sequence[StageIndex].Rotation.ToVector();

	// Need to modify -- Axes values should never be higher than the
	RotationRemaining.X = RotationRemaining.X == 0.0 ? 0.0 : RotationRemaining.X;
	RotationRemaining.Y = RotationRemaining.Y == 0.0 ? 0.0 : RotationRemaining.Y;
	RotationRemaining.Z = RotationRemaining.Z == 0.0 ? 0.0 : RotationRemaining.Z;

	// Set the targets
	if (StageIndex == 0)
	{
		// Origin return
		CurrentLocationTarget = OriginLocation;
		CurrentRotationTarget = OriginRotation;
		UE_LOG(LogTemp, Warning, TEXT("-- Starting Origin Target: %s"), *CurrentRotationTarget.Rotator().ToCompactString());
	}
	else
	{
		CurrentLocationTarget = CurrentLocation + (Sequence[StageIndex].Location.Offset * Direction);

		FQuat Step = TrackRotationStep(RotationRemaining, RotationStep, RotationTolerance);
		CurrentRotationTarget *= Step * Direction;
		
		//CurrentRotationTarget = UKismetMathLibrary::Quat_MakeFromEuler(RotationRemaining) * Direction;
	}

	// TODO: Event Dispatcher on StageUpdated
}

void UTriggerableMover::AdjustRemainingRotation(const FVector &ReductionAmount)
{
	RotationRemaining.X = FMath::Max(RotationRemaining.X - ReductionAmount.X, 0.0);
	RotationRemaining.Y = FMath::Max(RotationRemaining.Y - ReductionAmount.Y, 0.0);
	RotationRemaining.Z = FMath::Max(RotationRemaining.Z - ReductionAmount.Z, 0.0);
}

FQuat UTriggerableMover::TrackRotationStep(const FVector &RemainingRotation, const int32 StepInterval, const double Tolerance)
{
	double Pitch = TrackRotationForce(RemainingRotation.Y, StepInterval, Tolerance);
	double Yaw = TrackRotationForce(RemainingRotation.Z, StepInterval, Tolerance);
	double Roll = TrackRotationForce(RemainingRotation.X, StepInterval, Tolerance);

	UE_LOG(LogTemp, Warning, TEXT("Pitch, Yaw, and Roll for stage %i: %f, %f, %f"), StageIndex, Pitch, Yaw, Roll);

	return FRotator(Pitch, Yaw, Roll).Quaternion();
}

float UTriggerableMover::TrackRotationForce(const double TrackedForce, const int32 StepInterval, const double Tolerance)
{
	// Between zero and rotation tolerance range
    if (FMath::Abs(TrackedForce) <= Tolerance)
	{
		return Tolerance;
	}

	// Tracked value is same as step interval, so this is last step
	if (FMath::Abs(TrackedForce - StepInterval) <= Tolerance)
	{
		return TrackedForce;
	}

	// Check if remainder of mod is non-zero from 180 degree step. If zero, we know interval is factor of step, so subtract
	double Modulus = std::fmod(TrackedForce, StepInterval);
	double Interval = FMath::Min(TrackedForce - StepInterval, StepInterval);
	
	return Modulus == 0.0 ? Interval : Modulus;
}

void UTriggerableMover::Trigger_Implementation()
{
	// Ignore if there is no sequence to trigger or already triggered
	if (Sequence.Num() == 0 || bHasTriggered || !bActive)
	{
		return;
	}

	// We are coming from a reverse state, so target index should be the next one
	if (bIsReversing)
	{
		StageIndex = FMath::Clamp(StageIndex++, 0, Sequence.Num() - 1);

		// Movment should be
		CurrentLocationTarget = Sequence[StageIndex].Location.Offset;

		// Calculate the traveled rotation and set the target to the inverse of that travel
		RotationRemaining = Sequence[StageIndex].Rotation.ToVector() - RotationRemaining;
		CurrentRotationTarget = UKismetMathLibrary::Quat_MakeFromEuler(-RotationRemaining);
	}

	bHasTriggered = true;
	bIsReversing = false;
	bHasCompleted = false;

	// TODO: Event Dispatcher for OnTriggered
}

void UTriggerableMover::Reverse_Implementation()
{

	// Ignore if we there is no sequence or already reversing
	if (Sequence.Num() == 0 || bIsReversing || !bActive)
	{
		return;
	}

	if (bHasTriggered)
	{
		StageIndex = FMath::Clamp(StageIndex--, 0, Sequence.Num() - 1);
		CurrentLocationTarget = Sequence[StageIndex].Location.Offset;

		// Calculate the traveled rotation and set the target to the inverse of that travel
		RotationRemaining = Sequence[StageIndex].Rotation.ToVector() - RotationRemaining;
		CurrentRotationTarget = UKismetMathLibrary::Quat_MakeFromEuler(-RotationRemaining);
	}

	RotationRemaining = Sequence[StageIndex].Rotation.ToVector() - RotationRemaining;

	bHasTriggered = false;
	bIsReversing = true;
	bHasCompleted = false;

	// TODO: Event Dispatcher for OnReversed
}

void UTriggerableMover::Loop()
{
	if (!bLoopForever)
	{
		return;
	}
	
	bHasCompleted = false;

	// TODO: Event Dispatcher for OnLooped

	if (bIsReversing)
	{
		Trigger_Implementation();
	}
	else if (bHasTriggered)
	{
		Reverse_Implementation();
	}
}