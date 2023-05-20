// Fill out your copyright notice in the Description page of Project Settings.

#include "TriggerComponentBox.h"

UTriggerComponentBox::UTriggerComponentBox() : UTriggerComponentBase()
{
    PrimaryComponentTick.bCanEverTick = true;

    ShapeComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Area"));
    ShapeComponent->SetGenerateOverlapEvents(true);
    ShapeComponent->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
}

void UTriggerComponentBox::BeginPlay()
{
    Super::BeginPlay();

    CheckInitialOverlap(ShapeComponent);

    ShapeComponent->OnComponentBeginOverlap.AddDynamic(this, &UTriggerComponentBox::OverlapTriggerBegin);
	ShapeComponent->OnComponentEndOverlap.AddDynamic(this, &UTriggerComponentBox::OverlapTriggerEnd);
}

void UTriggerComponentBox::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTriggerComponentBox::OverlapTriggerBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OverlapTriggerBegin(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);
}

void UTriggerComponentBox::OverlapTriggerEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    Super::OverlapTriggerEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}