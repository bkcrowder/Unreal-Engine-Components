#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ITriggerable.h"
#include "ITrigger.generated.h"

UINTERFACE(MinimalAPI)
class UITrigger : public UInterface
{
	GENERATED_BODY()
};

class MPSTARTER_API IITrigger
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/// @brief Attempt to Trigger the events associated with the trigger.
	/// @remark Will reverse triggerables that can if number of present valid actors is not met
	UFUNCTION(BlueprintNativeEvent, Category = "Trigger")
	void Trigger() const;

	/// @brief Determines whether or not associated events can be triggered
	/// @return Whether or not events can be triggered. False will also indicate reversible Triggerables will be reversed.
	UFUNCTION(BlueprintNativeEvent, Category = "Trigger")
	bool CanTrigger() const;
};
