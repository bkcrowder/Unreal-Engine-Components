#include "Mover.h"

// Sets default values for this component's properties
UMover::UMover()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UMover::BeginPlay()
{
	Super::BeginPlay();
	LocationOrigin = GetOwner()->GetActorLocation();
}


// Called every frame
void UMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Move(GetOwner()->GetActorLocation(), DeltaTime);
}

bool UMover::IsOneTimeMover()
{
	return OneTimeMove;
}

void UMover::SetActivation(bool GoingToMove)
{
	// One time mover has already activated, so disallow setting
	if (Activate && OneTimeMove)
	{
		return;
	}

	Activate = GoingToMove;
}

void UMover::Move(FVector CurrentLocation, float DeltaTime)
{
	// No need to try to move once a one-shot is done
	if (IsOneTimeMoveAndDone())
	{
		return; 
	}
	
	FVector TargetLocation = LocationOrigin;
	
	if (Activate)
	{
		TargetLocation += MoveOffset;
	}

	float Speed = MoveOffset.Length() / MoveTime;

	// Next location to move to in sequence
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);
	GetOwner()->SetActorLocation(NewLocation);

	OneTimeMoveAndDone(CurrentLocation);
}

bool UMover::IsOneTimeMoveAndDone()
{
	return OneTimeMove && HasCompletedMove;
}

void UMover::OneTimeMoveAndDone(FVector CurrentLocation)
{
	if (!OneTimeMove){ return; }

	HasCompletedMove = LocationOrigin + MoveOffset == CurrentLocation;

	// Only enable physics if the one shot mover is done
	if (EnablePhysicsOnMove && IsOneTimeMoveAndDone())
	{
		UPrimitiveComponent *Component = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
		if (Component == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Unable to enable physics on owner: %s"), *GetOwner()->GetActorNameOrLabel());
			return;
		}

		Component->SetSimulatePhysics(true);
		Component->RecreatePhysicsState();
	}
}