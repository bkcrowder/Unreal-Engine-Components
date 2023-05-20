#include "TriggerComponentBase.h"

// Sets default values for this component's properties
UTriggerComponentBase::UTriggerComponentBase()
{
}

// Called when the game starts
void UTriggerComponentBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UTriggerComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTriggerComponentBase::AddTriggerable(IITriggerable* Triggerable)
{
	if (!Triggerables.Contains(Triggerable))
	{
		Triggerables.Add(Triggerable);
	}
}

void UTriggerComponentBase::Trigger_Implementation() const
{
	if (Triggerables.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Triggerables are empty!"));
		return;
	}

    for (IITriggerable* Triggerable : Triggerables)
    {
        if (CanTrigger_Implementation())
        {
            Triggerable->Trigger();
        }
        else
        {
            Triggerable->Reverse();
        }
    }
}

void UTriggerComponentBase::ValidateActor(AActor* Actor)
{
    // Only add to pending if they are not already pending, already valid, or already ignored
	if (ActorsValid.Num() == 0)
	{
		return;
	}

	if (!ActorsValid.Contains(Actor))
	{
		if (IsAcceptableActor(Actor))
        {
            ActorsValid.Add(Actor);
            AttachActorToTrigger(Actor);
            Trigger_Implementation();
        }
    }
}

void UTriggerComponentBase::AttachActorToTrigger(AActor *Actor)
{
    if (!AttachActor)
    {
        return;
    }
    // Freeze component on the trigger if we're meant to attach actor(s)
    // TODO: Change behavior
    if (UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
    {
        Component->SetSimulatePhysics(false);
        Actor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
    }
}

bool UTriggerComponentBase::IsAcceptableActor(AActor *Actor) const
{
	if (AcceptableActorTags.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, 
			TEXT("Tag for Trigger Key is not set! No actions will happen on overlap! %s"),
			*Actor->GetActorNameOrLabel());
		return false;
	}

	TArray<FName> &Tags = Actor->Tags;

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

void UTriggerComponentBase::CheckInitialOverlap(UPrimitiveComponent* Component)
{
	// Get initial overlapping actors
	TArray<AActor *> InitialOverlappingActors;
	Component->GetOverlappingActors(InitialOverlappingActors);
	if (InitialOverlappingActors.Num() > 0)
	{
		for (AActor* Actor : InitialOverlappingActors)
		{
			ValidateActor(Actor);
		}
	}
}

#pragma region Delegates
void UTriggerComponentBase::OverlapTriggerBegin(UPrimitiveComponent *OverlappedComponent, AActor *Actor, UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling OverlapTriggerBegin"));
	ValidateActor(Actor);
}

void UTriggerComponentBase::OverlapTriggerEnd(UPrimitiveComponent *OverlappedComponent, AActor *Actor, UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex)
{
    if (ActorsValid.Num() > 0)
	{
		ActorsValid.Remove(Actor);
	}

	UE_LOG(LogTemp, Warning, TEXT("Calling Trigger_Implementation from OverlapTriggerEnd"));
	Trigger_Implementation();
}
#pragma endregion