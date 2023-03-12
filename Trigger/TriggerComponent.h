#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Mover.h"
#include "TriggerComponent.generated.h"

/// @brief Collision trigger component class that activates / reverses mover(s) based on mover configuration(s)
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRYPTRAIDER_API UTriggerComponent : public UBoxComponent
{
	GENERATED_BODY()

public:	
	// ctor
	UTriggerComponent();

	/// @brief Sets the mover to trigger
	/// @param Mover UMover point to the Mover class to trigger
	UFUNCTION(BlueprintCallable)
	void AddMover(UMover *Mover);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// @brief Delegate Callback when a collision overlap event begins
	/// @param OverlappedComponent Overlapped component
	/// @param OtherActor Overlapped Actor
	/// @param OtherComponent Signature matching only
	/// @param OtherBodyIndex Signature matching only
	/// @param bFromSweep Signature matching only
	/// @param SweepResult Signature matching only
	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent *OverlappedComponent, 
						AActor *OtherActor, 
						UPrimitiveComponent *OtherComponent, 
						int32 OtherBodyIndex, 
						bool bFromSweep, 
						const FHitResult &SweepResult);

	/// @brief Delegate Callback when a collision overlap event ends
	/// @param OverlappedComponent Overlapped component
	/// @param OtherActor Overlapped Actor
	/// @param OtherComponent Signature matching only
	/// @param OtherBodyIndex Signature matching only
	/// @param bFromSweep Signature matching only
	/// @param SweepResult Signature matching only
	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, 
						AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, 
						int32 OtherBodyIndex);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	/// @brief Set of positive tags that need to be present
	/// @remark This will evaluate if actor has ANY / ONE; not all
	UPROPERTY(EditAnywhere)
	TSet<FName> AcceptableActorTags;
	
	/// @brief How many actors are needed to satisfy the trigger
	UPROPERTY(EditAnywhere, Meta = (ClampMin="1"))
	int32 NumberOfActorsNeeded = 1;

	/// @brief The tag names to use for excluding the colliding actor
	/// @remark This will evaluate if an actor has ANY/ONE; not all
	UPROPERTY(EditAnywhere)
	TSet<FName> ExclusionTags;

	/// @brief Whether or not to attach the actor to the current component (disables physics)
	/// @remark When using a player body, set this to false to prevent the player from attaching to the trigger
	UPROPERTY(EditAnywhere)
	bool AttachActor = true;

	TArray<UMover *> Movers;

	/// @brief Collision actors who have not yet been checked for acceptable tags
	TSet<AActor *> ActorsPending;

	/// @brief Collision actors who have acceptable tags and not yet acted on
	TSet<AActor *> ActorsValid;

	/// @brief Prevents continuous evaluation of collision actors who have already been validated and acted on
	TSet<AActor *> ActorsIgnored;

	/// @brief Determines if the provided actor contains the tag necessary to execute this trigger
	/// @param OtherActor Overlapped Actor
	/// @return True if the actor contains the matching tag; false if not
	bool IsAcceptableActor(AActor *OtherActor) const;

	/// @brief Triggers mover behavior based on requirement satisfaction
	/// @param ShouldMove Whether or not the mover should move to new position, or revert to origin
	void TriggerMovers(bool ShouldMove);
	
	/// @brief Sets the physics for the actor, attaches it to this trigger, and triggers mover behavior
	/// @param Actor 
	void TriggerValidOverlap(AActor *Actor);
	
	/// @brief Checks for validated actors that are not being ignored, triggers overlap, and then ignores the actor
	void CheckValidActors();

	/// @brief Checks pending actors. If they're valid, they are added to valid list. If not, they are ignored.
	void CheckPendingActors();

	/// @brief Removes pending actor and adds actor to ignore list. Leaves actor in valid list if already present.
	/// @param Actor 
	void IgnoreActor(AActor *Actor);

	/// @brief Clears the actor from all hash sets
	/// @param Actor 
	void ClearActor(AActor *Actor);
};

