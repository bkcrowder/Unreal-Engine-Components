#include "TriggerableMover.h"

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
	OriginRotation = CurrentRotationTarget = PreviousRotationTarget = GetOwner()->GetActorRotation();

	// Set up a basic origin stage
	FLocationStage OriginStageLocation = FLocationStage(FVector(0, 0, 0), OriginLocationReturnVelocity);
	FRotationStage OriginStageRotation = FRotationStage(FVector(0, 0, 0), OriginRotationReturnVelocity);
	
	OriginSequenceStage = FSequenceStage(OriginStageLocation, OriginStageRotation);
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

void UTriggerableMover::Move(const float DeltaTime, const FVector CurrentLocation, const FLocationStage& StageLocation, bool bReverse)
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

void UTriggerableMover::Rotate(const float DeltaTime, const FRotator CurrentRotation, const FRotationStage& StageRotation, bool bReverse)
{
	// Early return if rotation is done OR we're in reverse and rotation is not allowed to reverse
	if ((CurrentRotationTarget - CurrentRotation).IsNearlyZero() || (bReverse && !StageRotation.bIsReversible))
	{
		return;
	}

	// Retrieve the correction direction based on if the sequence is reversing and the stage rotation is allowed to reverse; multiply by deltatime
	FVector DirectionVelocity = bReverse && StageRotation.bIsReversible ? StageRotation.ReverseVelocity : StageRotation.ForwardVelocity;
	float Speed = StageRotation.RotOffset.Euler().Size() / DirectionVelocity.Size();
	FRotator InterpRotation = FMath::RInterpConstantTo(CurrentRotation, CurrentRotationTarget, DeltaTime, Speed);

	RotationRemaining += -1 * (InterpRotation - CurrentRotation);

	GetOwner()->SetActorRotation(InterpRotation);
}

void UTriggerableMover::UpdateStages(const FVector CurrentLocation, const FRotator CurrentRotation, bool bReverse)
{
	// We've reached our destination, so increment/decrement
	if ((CurrentLocationTarget == CurrentLocation) && (CurrentRotationTarget == CurrentRotation))
	{
		int32 Direction = bReverse ? -1 : 1;
		StageIndex += Direction;

		// Ensure we don't move outside of the Array boundaries
		StageIndex = FMath::Clamp(StageIndex, 0, Sequence.Num() - 1);

		// Check the rotation overflow still needed and return without changing stages
		if (!RotationRemaining.IsZero())
		{
			// Add the next step in the rotation, broken up into 180 degree capped chunks
			CurrentRotationTarget += TrackRotationStep(RotationRemaining);
			return;
		}

		// TODO: Event Dispatcher on bHasCompleted if bHasCompleted == true and we reside at the stage location/rotation
		bHasCompleted = bReverse ? StageIndex == 0 : StageIndex == Sequence.Num() - 1;

		if (bHasCompleted)
		{
			Loop();
			return;
		}

		SetStageTargets(CurrentLocation, CurrentRotation, Direction);
	}
}

void UTriggerableMover::SetStageTargets(const FVector CurrentLocation, const FRotator CurrentRotation, const int32 Direction)
{
	if (Direction != 1 && Direction != -1)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Index is now %i. Direction is %i"), StageIndex, Direction);

	PreviousLocationTarget = CurrentLocationTarget;
	PreviousRotationTarget = CurrentRotationTarget;

	FRotationStage SequenceRotation = Sequence[StageIndex].Rotation;

	// Since FRotationStage a struct, we cannot use the Blueprint Setter in order to assign these as blueprint values change
	RotationRemaining = Sequence[StageIndex].Rotation.UpdateRotationFromVector();
	UE_LOG(LogTemp, Warning, TEXT("Rotation Track: %s"), *RotationRemaining.ToCompactString());

	// Set the targets
	if (StageIndex == 0)
	{
		// Origin return
		CurrentLocationTarget = OriginLocation;
		CurrentRotationTarget = OriginRotation;
	}
	else
	{
		CurrentLocationTarget = CurrentLocation + (Sequence[StageIndex].Location.Offset * Direction);
		CurrentRotationTarget = UKismetMathLibrary::ComposeRotators(CurrentRotation, RotationRemaining * Direction);
	}

	// TODO: Event Dispatcher on StageUpdated
}

FRotator UTriggerableMover::TrackRotationStep(const FRotator RotationToStep)
{
	return FRotator(
		TrackRotationForce(RotationToStep.Pitch),
		TrackRotationForce(RotationToStep.Yaw),
		TrackRotationForce(RotationToStep.Roll)
	);
}

float UTriggerableMover::TrackRotationForce(const double TrackedForce)
{
    if (TrackedForce == 0)
	{
		return 0.0f;
	}

	// Step rotations in 180 degrees so rotations won't immediately read 0 == 360
	double Modulus = std::fmod(TrackedForce, RotationStep);

	return Modulus == 0.0 ? TrackedForce - RotationStep : Modulus;
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
		CurrentLocationTarget = PreviousLocationTarget;
		CurrentRotationTarget = PreviousRotationTarget;
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
		CurrentLocationTarget = PreviousLocationTarget;
		CurrentRotationTarget = PreviousRotationTarget;
	}

	bHasTriggered = false;
	bIsReversing = true;
	bHasCompleted = false;
	
	if (StageIndex == 0)
	{
		return;
	}

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