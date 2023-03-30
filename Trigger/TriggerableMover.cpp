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

	OriginLocation = CurrentStageLocationTarget = CompletedStageLocation = GetOwner()->GetActorLocation();
	OriginRotation = CurrentStageRotationTarget = CompletedStageRotation = GetOwner()->GetActorRotation();

	// We have an inital sequence, so set targets
	if (Sequence.Num() > 0)
	{
		CurrentStageLocationTarget = GetStageTargetLocation(Sequence[MovementIndex]->MovementOffset);
		CurrentStageRotationTarget = GetStageTargetRotation(Sequence[MovementIndex]->RotationOffset);
	}
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
void UTriggerableMover::AddToSequence(TArray<FMovementRotationStage *> const MovementAndRotation)
{
	if (MovementAndRotation.IsEmpty())
	{
		return;
	}

	Sequence.Append(MovementAndRotation);
}

void UTriggerableMover::AddToSequence(FMovementRotationStage * const MovementAndRotation)
{
	if (MovementAndRotation)
	{
		Sequence.Add(MovementAndRotation);
	}
}

void UTriggerableMover::SetSequence(TArray<FMovementRotationStage *> const MovementAndRotation)
{
	
	if (MovementAndRotation.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Movement and Rotation sequence did not get set! You need to specify the replacement when setting the sequence!"));
	}
	else
	{
		Sequence.Empty();
		Sequence = MovementAndRotation;
	}
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

void UTriggerableMover::MoveAndRotate(float DeltaTime, bool bReverse)
{
	// Sequence is empty or this is in an untouched state
	if (Sequence.Num() == 0 || (!bHasTriggered && !bIsReversing))
	{
		return;
	}

	// Already completed sequence, so loop if selected, or early return
	if (bHasCompleted)
	{
		if (bLoopForever)
		{
			Loop();
		}

		return;
	}

	// Current Actor State
	FVector CurrentLocation = GetOwner()->GetActorLocation();
	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	// Update the stage and completion if we've reached stage destination
	UpdateStageAndCompletion(CurrentLocation, CurrentRotation, bReverse);
	if (bHasCompleted)
	{
		return;
	}

	// Current stage in sequence
	FMovementRotationStage *CurrentStage = Sequence[MovementIndex];
	if (!CurrentStage)
	{
		return;
	}

	Move(DeltaTime, CurrentLocation, CurrentStage, bReverse);
	Rotate(DeltaTime, CurrentRotation, CurrentStage, bReverse);
	
}

void UTriggerableMover::Move(float DeltaTime, FVector CurrentLocation, FMovementRotationStage* CurrentStage, bool bReverse)
{
	// Target location + necessary offset from GetStageTargetLocation
	CurrentStageLocationTarget = GetStageTargetLocation(CurrentStage->MovementOffset, bReverse);

	// Movement Speed
	float DirectionSpeed = bReverse ? CurrentStage->MoveReverseSpeed : CurrentStage->MoveForwardSpeed;
	float Speed = CurrentStage->MovementOffset.Length() / DirectionSpeed;

	FVector InterpLocation = FMath::VInterpConstantTo(CurrentLocation, CurrentStageLocationTarget, DeltaTime, Speed);
	GetOwner()->SetActorLocation(InterpLocation);
}

void UTriggerableMover::Rotate(float Deltatime, FRotator CurrentRotation, FMovementRotationStage* CurrentStage, bool bReverse)
{
	CurrentStageRotationTarget = GetStageTargetRotation(CurrentStage->RotationOffset, bReverse);
}

FVector UTriggerableMover::GetStageTargetLocation(FVector StageLocation, bool bReverse)
{
	if (bReverse)
	{
		return CurrentStageLocationTarget -= StageLocation;
	}

	return CurrentStageLocationTarget += StageLocation;
}

FRotator UTriggerableMover::GetStageTargetRotation(FRotator StageRotation, bool bReverse)
{
    if (bReverse)
	{
		return CurrentStageRotationTarget -= StageRotation;
	}

	return CurrentStageRotationTarget += StageRotation;
}

void UTriggerableMover::UpdateStageAndCompletion(FVector CurrentLocation, FRotator CurrentRotation, bool bReverse)
{
	// We've reached our destination, so increment/decrement
	if (CurrentStageLocationTarget == CurrentLocation &&
		CurrentStageRotationTarget == CurrentRotation)
	{
		MovementIndex = bReverse ? MovementIndex - 1 : MovementIndex + 1;

		// Ensure we don't move outside of the Array boundaries
		MovementIndex = FMath::Clamp(MovementIndex, 0, Sequence.Num() - 1);

		// TODO: Event Dispatcher on StageUpdated
	}

	bHasCompleted = bReverse ? MovementIndex == 0 : MovementIndex == Sequence.Num() - 1;
	// TODO: Event Dispatcher on bHasCompleted if bHasCompleted == true
}

void UTriggerableMover::Trigger_Implementation()
{
	// Don't trigger if: No sequence to trigger or already triggered
	if (Sequence.Num() == 0 || bHasTriggered)
	{
		return;
	}

	bHasTriggered = true;
	bIsReversing = false;

	// TODO: Event Dispatcher for OnTriggered
}

void UTriggerableMover::Reverse_Implementation()
{
	// Don't reverse if: No sequence or already reversing
	if (Sequence.Num() == 0 || bIsReversing)
	{
		return;
	}

	bHasTriggered = false;
	bIsReversing = true;

	// TODO: Event Dispatcher for OnReversed
}

void UTriggerableMover::Loop()
{
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