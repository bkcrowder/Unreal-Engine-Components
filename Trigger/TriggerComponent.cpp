#include "TriggerComponent.h"

// Sets default values for this component's properties
UTriggerComponent::UTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Ensure overlap events are set
	this->SetGenerateOverlapEvents(true);
	
	this->OnComponentBeginOverlap.AddDynamic(this, &UTriggerComponent::OverlapBegin);
	this->OnComponentEndOverlap.AddDynamic(this, &UTriggerComponent::OverlapEnd);

	// Get initial overlapping actors
	TArray<AActor *> InitialOverlappingActors;
	GetOverlappingActors(InitialOverlappingActors);
	ActorsPending = TSet<AActor *>(InitialOverlappingActors);
}

// Called every frame
void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckPendingActors();
	CheckValidActors();
}

void UTriggerComponent::AddMover(UMover *Mover)
{
	if (!Movers.Contains(Mover))
	{
		Movers.Add(Mover);
	}
}

void UTriggerComponent::TriggerMovers(bool ShouldMove)
{
	for (UMover* Mover : Movers)
	{
		Mover->SetActivation(ShouldMove);
	}
}

void UTriggerComponent::TriggerValidOverlap(AActor *Actor)
{
	UPrimitiveComponent *Component = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
	if (Component == nullptr)
	{
		return;
	}
	
	if (AttachActor)
	{
		Component->SetSimulatePhysics(false);
		Actor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	}

	// Move once the quantity has been satisfied
	if (Movers.Num() > 0)
	{
		TriggerMovers(ActorsValid.Num() >= NumberOfActorsNeeded);
	}
}

bool UTriggerComponent::IsAcceptableActor(AActor *OtherActor) const
{
	if (AcceptableActorTags.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, 
			TEXT("Tag for Trigger Key is not set! No actions will happen on overlap! %s"),
			*OtherActor->GetActorNameOrLabel());
		return false;
	}

	TArray<FName> &Tags = OtherActor->Tags;

	// Sift out exclusion tags
	for (FName Exclude : ExclusionTags)
	{
		if (Tags.Contains(Exclude))
		{
			return false;
		}
	}

	// Find at least one acceptable tag
	for (FName Acceptable : AcceptableActorTags)
	{
		if (Tags.Contains(Acceptable))
		{
			return true;
		}
	}

	return false;
}

void UTriggerComponent::CheckPendingActors()
{
	if (ActorsPending.Num() > 0)
	{
		for (AActor *Actor : ActorsPending)
		{
			if (IsAcceptableActor(Actor))
			{
				ActorsValid.Add(Actor);
			}
		}
	}
}

void UTriggerComponent::CheckValidActors()
{
	if (ActorsValid.Num() > 0)
	{
		for (AActor *Actor : ActorsValid)
		{
			if (ActorsIgnored.Contains(Actor))
			{
				continue;
			}

			TriggerValidOverlap(Actor);
			IgnoreActor(Actor);
		}
	}
}

void UTriggerComponent::IgnoreActor(AActor* Actor)
{
	ActorsPending.Remove(Actor);
	if (!ActorsIgnored.Contains(Actor))
	{
		ActorsIgnored.Add(Actor);
	}
}

void UTriggerComponent::ClearActor(AActor* Actor)
{
	ActorsValid.Remove(Actor);
	ActorsPending.Remove(Actor);
	ActorsIgnored.Remove(Actor);
}

void UTriggerComponent::OverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *Actor, UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	// Only add to pending if they are not already pending, already valid, or already ignored
	if (!ActorsPending.Contains(Actor) && 
		!ActorsValid.Contains(Actor) && 
		!ActorsIgnored.Contains(Actor))
	{
		ActorsPending.Add(Actor);
	}
}

void UTriggerComponent::OverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex)
{
	for (UMover * Mover : Movers)
	{
		// Only permit actor removal from non-one time movers and incomplete movers
		if (Mover->IsOneTimeMoveAndDone()){
			continue;
		}
		
		ClearActor(OtherActor);
		TriggerMovers(ActorsValid.Num() >= NumberOfActorsNeeded);
	}
}