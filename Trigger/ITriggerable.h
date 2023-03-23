#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ITriggerable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UITriggerable : public UInterface
{
	GENERATED_BODY()
};

class MPSTARTER_API IITriggerable
{
	GENERATED_BODY()

public:
	
	/// @brief Trigger the event
	UFUNCTION(BlueprintNativeEvent, Category = "Triggerable")
	void Trigger();

	/// @brief Reverses the trigger event
	UFUNCTION(BlueprintNativeEvent, Category = "Triggerable")
	void Reverse();

	/// @brief Triggerable is in a triggered state
	/// @return Whether or not the triggerable is in a triggered state
	UFUNCTION(BlueprintNativeEvent, Category = "Triggerable")
	bool IsTriggered() const;

	/// @brief Check if the triggerable can be reversed
	/// @return Whether or not the triggerable can be reversed
	UFUNCTION(BlueprintNativeEvent, Category = "Triggerable")
	bool IsReversible() const;

	/// @brief Triggerable is completed with its triggering process (manipulation)
	/// @return Whether or not the triggerable is considered "finished" with its process
	UFUNCTION(BlueprintNativeEvent, Category = "Triggerable")
	bool IsDone() const;
};
