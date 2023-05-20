// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "ITrigger.h"
#include "ITriggerable.h"
#include "TriggerComponentBase.generated.h"

/*
	Abstract Trigger Component base class
	Inherits from Primitive and implements IITrigger interface

	Utilized to create proximity-based trigger components of varying shapes
	For things that are interactable such as levers, doors, etc., denote those a "triggerables" instead
*/
UCLASS(Abstract, Blueprintable)
class MPSTARTER_API UTriggerComponentBase : public UPrimitiveComponent, public IITrigger
{
	GENERATED_BODY()

public:	
	// ctor
	UTriggerComponentBase();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddTriggerable(IITriggerable* Triggerable);

	/// @brief Executes the triggerables associated with this trigger
	void Trigger_Implementation() const override;

	/// @brief Determines whether or not the number of valid actors is greater than or equal to number of actors needed
	/// @return If the number of valid actors is greater than or equal to number of actors needed
	bool CanTrigger_Implementation() const override { return ActorsValid.Num() >= NumberOfActorsNeeded; };

	/// @brief Delegate Callback when a collision overlap event begins
	/// @param OverlappedComponent Overlapped component
	/// @param OtherActor Overlapped Actor
	/// @param OtherComponent Signature matching only
	/// @param OtherBodyIndex Signature matching only
	/// @param bFromSweep Signature matching only
	/// @param SweepResult Signature matching only
	virtual void OverlapTriggerBegin(UPrimitiveComponent *OverlappedComponent, 
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
	virtual void OverlapTriggerEnd(UPrimitiveComponent* OverlappedComponent, 
						AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, 
						int32 OtherBodyIndex);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/// @brief Set of positive tags that need to be present on the colliding actor
	/// @remark This will evaluate if actor has ANY / ONE; not all
	UPROPERTY(EditAnywhere, Category = "Trigger")
	TSet<FName> AcceptableActorTags;
	
	/// @brief How many actors are needed to satisfy the trigger
	UPROPERTY(EditAnywhere, Category = "Trigger", Meta = (ClampMin="1"))
	int32 NumberOfActorsNeeded = 1;

	/// @brief The tag names to use for excluding the colliding actor
	/// @remark This will evaluate if an actor has ANY/ONE; not all
	UPROPERTY(EditAnywhere, Category = "Trigger")
	TSet<FName> ExclusionTags;

	/// @brief Whether or not to attach the actor to the current component (disables physics)
	/// @remark When using a player body, set this to false to prevent the player from attaching to the trigger
	UPROPERTY(EditAnywhere, Category = "Trigger")
	bool AttachActor = true;

	/*
		TODO: Future Me - Get rid of this code smell and implement Selectable Shape Component in editor to
				- Generate the corresponding Shape Component
				- Child it to the trigger component
				- Destroy old shape component and generate new one when editor selection changes (OnSelectChanged non-blueprint delegate?)
	/// @brief Trigger Shape to use
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (GetOptions = "GetShapeOptions"))
	UShapeComponent* TriggerShape;

	/// @brief Trigger Shape selection in Editor
	/// @return Array of Trigger Shape Selectables
	UFUNCTION(CallInEditor)
	TArray<UShapeComponent*> GetShapeOptions() {
		FName Label = "Trigger Area";
		UBoxComponent *Box = CreateDefaultSubobject<UBoxComponent>(Label);
		USphereComponent *Sphere = CreateDefaultSubobject<USphereComponent>(Label);
		UCapsuleComponent *Capsule = CreateDefaultSubobject<UCapsuleComponent>(Label);
		UShapeComponent *Custom = CreateDefaultSubobject<UShapeComponent>(Label);
		
		return {
			Box,
			Sphere,
			Capsule,
			Custom
		};
	}
	*/

	/// @brief Collision actors who have acceptable tags and not yet acted on
	TSet<AActor*> ActorsValid;

	/// @brief  Array of triggerables to execute
	TArray<IITriggerable*> Triggerables;

	/// @brief Validates the supplied actor. If valid, append to valid actors hashset and trigger events
	/// @param Actor 
	void ValidateActor(AActor *Actor);

	/// @brief Attaches the actor to the root component and deactivates physics
	/// @param Actor Actor to attach
	void AttachActorToTrigger(AActor *Actor);

	/// @brief Determines if the provided actor has accepted tag(s) and does not have excluded tag(s)
	/// @param Actor Overlapped Actor
	/// @return True if the actor contains the matching tag and no exclusion tag; false if not
	bool IsAcceptableActor(AActor *Actor) const;

	/// @brief Checks for initial overlapping actors
	void CheckInitialOverlap(UPrimitiveComponent* Component);
};
