// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TriggerComponentBase.h"
#include "Components/BoxComponent.h"
#include "TriggerComponentBox.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MPSTARTER_API UTriggerComponentBox : public UTriggerComponentBase
{
	GENERATED_BODY()

public: 
	UTriggerComponentBox();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// @brief Delegate Callback when a collision overlap event begins
	/// @param OverlappedComponent Overlapped component
	/// @param OtherActor Overlapped Actor
	/// @param OtherComponent Signature matching only
	/// @param OtherBodyIndex Signature matching only
	/// @param bFromSweep Signature matching only
	/// @param SweepResult Signature matching only
	UFUNCTION()
	virtual void OverlapTriggerBegin(UPrimitiveComponent *OverlappedComponent, 
						AActor *OtherActor, 
						UPrimitiveComponent *OtherComponent, 
						int32 OtherBodyIndex, 
						bool bFromSweep, 
						const FHitResult &SweepResult) override;

	/// @brief Delegate Callback when a collision overlap event ends
	/// @param OverlappedComponent Overlapped component
	/// @param OtherActor Overlapped Actor
	/// @param OtherComponent Signature matching only
	/// @param OtherBodyIndex Signature matching only
	/// @param bFromSweep Signature matching only
	/// @param SweepResult Signature matching only
	UFUNCTION()
	virtual void OverlapTriggerEnd(UPrimitiveComponent* OverlappedComponent, 
						AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, 
						int32 OtherBodyIndex) override;
	
	/// @brief Retrieve BoxComponent subobject
	FORCEINLINE class UBoxComponent* GetBoxComponent() const { return ShapeComponent; }

protected:
	virtual void BeginPlay();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ShapeComponent;
};
