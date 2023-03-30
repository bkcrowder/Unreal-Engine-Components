#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the components
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Attach mesh to root component
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGun::PullTrigger()
{
	if (AttachedMuzzleFlash)
	{
		if (AttachedMuzzleFlash->IsActive())
		{
			return;
		}

		AttachedMuzzleFlash->Activate();
	}
	else
	{
		if (MuzzleFlash)
		{
			AttachedMuzzleFlash = UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, MuzzleFlashSocket);
			AttachedMuzzleFlash->bAutoDestroy = false;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No muzzle flash set! Unable to generate particle component!"));
		}
	}
}

void AGun::ReleaseTrigger()
{
	if (AttachedMuzzleFlash)
	{
		AttachedMuzzleFlash->Deactivate();
	}
}
 