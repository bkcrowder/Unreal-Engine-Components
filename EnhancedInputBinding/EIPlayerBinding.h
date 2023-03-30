#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EIPlayerBinding.generated.h"

class AGun;

UCLASS()
class SIMPLESHOOTER_API AEIPlayerBinding : public ACharacter
{
	GENERATED_BODY()

public:
	/// @brief Enhanced Input Action Context
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputMappingContext* GroundMovementInputContext;

	/// @brief Context priority over other action contexts
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input|Movement")
	int32 GroundMovementContextPriority = 0;

	/// @brief Input action for moving
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input|Movement")
	UInputAction* InputMove;
	
	/// @brief Input action for looking
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input|Movement")
	UInputAction* InputLook;

	/// @brief Input action for jumping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input|Movement")
	UInputAction* InputJump;

	/// @brief Input action for crouching
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input|Movement")
	UInputAction* InputCrouch;

	/// @brief Input action for Primary Fire
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input|Weapons")
	UInputAction* PrimaryFire;

	/// @brief Gun or Weapon to set as actively equipped
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AGun> GunClass;

	/// @brief Bone name to clear original weapon. Leave empty if no default weapon bone is present in skeleton.
	UPROPERTY(EditDefaultsOnly, Category="Weapon Bone")
	FName WeaponBoneName = NAME_None;

	/// @brief Socket to attach the new weapon to
	UPROPERTY(EditDefaultsOnly, Category="Weapon Bone")
	FName WeaponSocketName = "WeaponSocket";

	/// @brief Stored pointer to reference the weapon
	AGun* Gun;

	// Sets default values for this character's properties
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

	/// @brief Trigger primary fire
	/// @param Instance Action instance containing values
	void PrimaryFire(const FInputActionInstance &Instance);
};
