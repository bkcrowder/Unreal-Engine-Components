#include "Grabber.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	PhysicsHandle = GetPhysicsHandle();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(PhysicsHandle && IsCarryingSomething)
	{
		Carry();
	}	
}

void UGrabber::Grab()
{
	if (PhysicsHandle == nullptr || IsCarryingSomething)
	{
		return;
	}

	FHitResult Hit;
	bool HasHit = GetGrabbableInReach(Hit);

	if (HasHit)
	{
		UPrimitiveComponent *Component = Hit.GetComponent();
		FRotator GrabberRotation = GetComponentRotation();

		Component->WakeAllRigidBodies();
		Component->SetSimulatePhysics(true);
		AActor *GrabbedOwner = Component->GetOwner();
		GrabbedOwner->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			Component,
			NAME_None,
			Hit.ImpactPoint,
			GetComponentRotation());

		IsCarryingSomething = true;

		// Add the corresponding tag
		ToggleGrabbedTag(GrabbedOwner, true);
	}
}

void UGrabber::Carry()
{
	UPrimitiveComponent *Component = PhysicsHandle->GetGrabbedComponent();
	if (Component)
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
}

void UGrabber::Release()
{
	UPrimitiveComponent *GrabbedComponent = PhysicsHandle->GetGrabbedComponent();
	if (GrabbedComponent && IsCarryingSomething)
	{
		GrabbedComponent->WakeAllRigidBodies();
		PhysicsHandle->ReleaseComponent();

		ToggleGrabbedTag(GrabbedComponent->GetOwner(), false);
		IsCarryingSomething = false;
	}
}

void UGrabber::ToggleGrabbedTag(AActor *Actor, bool Add)
{
	if (Actor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to toggle grabbed tags for actor as supplied actor is a null pointer!"));
		return;
	}

	if (GrabbedTag.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Grabbed tag is not set on grabbed %s! Cannot append tag!"), 
					*GetOwner()->GetActorNameOrLabel());
		return;
	}

	if (Add)
	{
		Actor->Tags.Add(GrabbedTag);
	}
	else
	{
		Actor->Tags.Remove(GrabbedTag);
	}
}

bool UGrabber::GetGrabbableInReach(FHitResult &OutHit) const
{
	UWorld *World = GetWorld();
	FVector Start = GetComponentLocation();
	FVector End = Start + (GetForwardVector() * MaxGrabDistance);	

	// Begin Sweep
	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	
	return World->SweepSingleByChannel(
		OutHit,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		Sphere
	);
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent *Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (Result == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber requires a UPhysicsHandle Component!"));
	}

	return Result;
}

void UGrabber::RotateActorToFaceAnotherActor(AActor* ActorToFace, AActor* SecondActor)
{
	FVector FaceForward = ActorToFace->GetActorForwardVector();
	FRotator RelativeSecond = UKismetMathLibrary::InverseTransformRotation(
		SecondActor->GetTransform(), 
		SecondActor->GetActorRotation());
	FVector SecondForward = UKismetMathLibrary::GetForwardVector(RelativeSecond);

	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(FaceForward, SecondForward);
	SecondActor->AddActorLocalRotation(LookAt);
}