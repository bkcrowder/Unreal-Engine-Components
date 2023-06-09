#include "EIPlayerBinding.h"
#include "Gun.h"

// Constructor
AEIPlayerBinding::AEIPlayerBinding()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEIPlayerBinding::BeginPlay()
{
	Super::BeginPlay();

	Gun = GetWorld()->SpawnActor<AGun>(GunClass);

	// Only clear the original weapon if we have a bone name to clear
	if (!WeaponBoneName.IsNone())
	{
		GetMesh()->HideBoneByName(WeaponBoneName, EPhysBodyOp::PBO_None);	
	}

	// Attach the component to the mesh and assign the gun owner
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponSocketName);
	Gun->SetOwner(GetOwner());
}

// Called every frame
void AEIPlayerBinding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEIPlayerBinding::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	// Local player enhanced input subsystem
	UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	// Adding Input Context Mapping
	EISubsystem->AddMappingContext(GroundMovementInputContext, GroundMovementContextPriority);

	UEnhancedInputComponent *PlayerEIComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	BindActions(PlayerEIComponent);
}

void AEIPlayerBinding::BindActions(UEnhancedInputComponent* PlayerEIComponent)
{
	// Move and Look
	PlayerEIComponent->BindAction(InputMove, ETriggerEvent::Triggered, this, &AEIPlayerBinding::MoveAround);
	PlayerEIComponent->BindAction(InputLook, ETriggerEvent::Triggered, this, &AEIPlayerBinding::Look);

	// Jump
	PlayerEIComponent->BindAction(InputJump, ETriggerEvent::Triggered, this, &AEIPlayerBinding::Jump);
	PlayerEIComponent->BindAction(InputJump, ETriggerEvent::Completed, this, &AEIPlayerBinding::Jump);

	// Crouch
	PlayerEIComponent->BindAction(InputCrouch, ETriggerEvent::Triggered, this, &AEIPlayerBinding::CrouchStart);
	PlayerEIComponent->BindAction(InputCrouch, ETriggerEvent::Completed, this, &AEIPlayerBinding::CrouchStop);
	PlayerEIComponent->BindAction(InputCrouch, ETriggerEvent::Canceled, this, &AEIPlayerBinding::CrouchStop);

	// Weapons
	PlayerEIComponent->BindAction(PrimaryFire, ETriggerEvent::Triggered, this, &AEIPlayerBinding::PrimaryFire);
}

void AEIPlayerBinding::Look(const FInputActionInstance& Instance)
{
	FVector2D Looking = Instance.GetValue().Get<FVector2D>();
	AddControllerYawInput(-Looking.X);
	AddControllerPitchInput(Looking.Y);
}

void AEIPlayerBinding::MoveAround(const FInputActionInstance& Instance)
{
	FVector2D Looking = Instance.GetValue().Get<FVector2D>();
	AddMovementInput(GetActorRightVector(), Looking.X);
	AddMovementInput(GetActorForwardVector(), Looking.Y);
}

void AEIPlayerBinding::JumpStart(const FInputActionInstance& Instance)
{
	Jump();
}

void AEIPlayerBinding::JumpStop(const FInputActionInstance& Instance)
{
	StopJumping();
}

void AEIPlayerBinding::CrouchStart(const FInputActionInstance& Instance)
{

}

void EIPlayerBinding::CrouchStop(const FInputActionInstance& Instance)
{

}

void EIPlayerBinding::PrimaryFire(const FInputActionInstance& Instance)
{
	if (Gun)
	{
		Gun->PullTrigger();
	}
}

