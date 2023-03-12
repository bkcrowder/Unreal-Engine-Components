#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EIPlayerBinding.generated.h"

UCLASS()
class SIMPLESHOOTER_API AShooter : public ACharacter
{
	GENERATED_BODY()

public:
	/// @brief Enhanced Input Action Context
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputMappingContext* GroundMovementInputContext;

	/// @brief Context priority over other action contexts
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	int32 GroundMovementContextPriority = 0;

	/// @brief Input action for moving
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* InputMove;
	
	/// @brief Input action for looking
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* InputLook;

	/// @brief Input action for jumping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* InputJump;

	/// @brief Input action for crouching
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* InputCrouch;

	// ctor
	AEIPlayerBinding();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/// @brief Bind actions to the Player's Enhanced Input Component
	/// @param PlayerEIComponent Enhanced Input Component for the player (casted from Player Input)
	void BindActions(UEnhancedInputComponent* PlayerEIComponent);

	/// @brief Look Around
	/// @param Instance Action instance containing values
	void Look(const FInputActionInstance& Instance);

	/// @brief Move Around
	/// @param Instance Action instance containing values
	void MoveAround(const FInputActionInstance& Instance);

	/// @brief Start the jump
	/// @param Instance Action instance containing values
	void JumpStart(const FInputActionInstance& Instance);

	/// @brief End the jump
	/// @param Instance Action instance containing values
	void JumpStop(const FInputActionInstance& Instance);

	/// @brief Start the crouch
	/// @param Instance Action instance containing values
	void CrouchStart(const FInputActionInstance& Instance);

	/// @brief End the crouch
	/// @param Instance Action instance containing values
	void CrouchStop(const FInputActionInstance& Instance);
};
