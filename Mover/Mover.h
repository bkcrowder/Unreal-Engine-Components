#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Mover.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRYPTRAIDER_API UMover : public UActorComponent
{
	GENERATED_BODY()

public:
	/// @brief Where to move the actor to
	UPROPERTY(EditAnywhere)
	FVector MoveOffset;

	/// @brief Time taken to move actor from current location to the move offset
	UPROPERTY(EditAnywhere)
	float MoveTime = 4.0;

	/// @brief Whether or not to enable physics once the actor has reached the move offset
	UPROPERTY(EditAnywhere)
	bool EnablePhysicsOnMove = false;

	/// @brief If the mover is a fire-and-done or if it should be able to return to its origin
	UPROPERTY(EditAnywhere)
	bool OneTimeMove = false;

	// ctor
	UMover();

	/// @brief Checks to see if this mover only moves a single time
	/// @return Whether or not this mover only moves on time 
	bool IsOneTimeMover();

	/// @brief Determines if this is a one time mover and it has completed moving
	/// @return Whether or not the one time mover has already finished moving
	bool IsOneTimeMoveAndDone();

	/// @brief Determines whether or not to activate the mover
	/// @param ShouldMove 
	void SetActivation(bool ShouldMove);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/// @brief Moves the mover from its current location to the origin + offset
	/// @param CurrentLocation Current Vector of the Mover Location
	/// @param DeltaTime Time difference between frame changes
	void Move(FVector CurrentLocation, float DeltaTime);
	
	/// @brief Sets the move completion for one time movers
	/// @remarks Compares the Origin + Offset location with the current location.
	/// @param CurrentLocation Current Vector of the Mover Location
	void OneTimeMoveAndDone(FVector CurrentLocation);

private:
	/// @brief Whether or not the mover is currently active
	bool Activate = false;

	/// @brief Whether or not the mover's current location is the desired offset location
	bool HasCompletedMove = false;

	/// @brief Where the mover started from
	FVector LocationOrigin;
};
