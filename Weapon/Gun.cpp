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
	UE_LOG(LogTemp, Warning, TEXT("Fire ze missles!"));
}
