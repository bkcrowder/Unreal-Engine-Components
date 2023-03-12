#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Grabber.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRYPTRAIDER_API UGrabber : public USceneComponent
{
	GENERATED_BODY()

public:	

	/// @brief Tag to apply that indicates an object/actor has been "grabbed"
	UPROPERTY(EditAnywhere)
	FName GrabbedTag = "Grabbed";

	/// @brief Maximum distance in the actor's forward vector that they can grab the actor
	UPROPERTY(EditAnywhere)
	float MaxGrabDistance = 400.0;

	/// @brief Channel sweep radius to detect collisions on specified channel
	UPROPERTY(EditAnywhere)
	float GrabRadius = 100.0;

	/// @brief How far away the grabbed object will be held while being carried
	UPROPERTY(EditAnywhere)
	float HoldDistance = 100.0;

	// ctor
	UGrabber();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/// @brief Grab the actor, apply the grabbed tag, and detach from that actor's owner
	UFUNCTION(BlueprintCallable)
	void Grab();

	/// @brief Release the actor and remove the grabbed tag
	UFUNCTION(BlueprintCallable)
	void Release();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	/// @brief Whether or not the current grabber actor is holding something
	bool IsCarryingSomething = false;

	/// @brief Physics handle for 
	UPhysicsHandleComponent* PhysicsHandle;

	/// @brief Sets the grabbed actor's location while it is being carried
	void Carry();

	/// @brief Determines of the actor in the hit result is within reach and is detected on the trace channel
	/// @param OutHit The hit result 
	/// @return Whether or not the hit result is within reach and detected on the trace channel
	bool GetGrabbableInReach(FHitResult &OutHit) const;

	/// @brief Retrieves the Physics Handle component attached to this actor
	/// @return UPhysicsHandleComponent Physics Handle on success or nullptr on failure
	UPhysicsHandleComponent* GetPhysicsHandle() const;

	/// @brief Toggles the designated grabbed tag
	/// @param Actor Actor to toggle the grabbed tag on
	/// @param Add Add the tag if true or remove the tag if false
	void ToggleGrabbedTag(AActor *Actor, bool Add);

	/// @brief Rotates the an actor to face another actor based on retrieving forward vector from world rotation
	/// @remark Does not currently work as intended... Though some results are hilarious...
	/// @param ActorToFace Actor that the second actor should turn towards
	/// @param SecondActor Actor performing the rotation to face the ActorToFace
	void RotateActorToFaceAnotherActor(AActor* ActorToFace, AActor* SecondActor);
};
