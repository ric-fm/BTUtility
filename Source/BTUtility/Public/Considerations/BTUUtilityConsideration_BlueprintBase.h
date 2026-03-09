// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Core/BTUUtilityConsideration.h"
#include "BTUUtilityConsideration_BlueprintBase.generated.h"


/**
 * @class UBTUUtilityConsideration_BlueprintBase
 * 
 * Extends the Utility Consideration system to Blueprints.
 * Designers can implement custom logic by overriding 'GetInputValue'.
 * The result is automatically clamped and passed through the ResponseCurve defined in the base class.
 */
UCLASS(Abstract, Blueprintable)
class BTUTILITY_API UBTUUtilityConsideration_BlueprintBase : public UBTUUtilityConsideration
{
	GENERATED_BODY()
	
protected:
#pragma region UBTUUtilityConsideration
	virtual float GetInputValue(const FBTUUtilityContext& Context) const override;
	
	virtual void GetDescriptionValues(TArray<FString>& OutValues) const override;
#pragma endregion
	
	/** Event to implement the custom logic in Blueprints. Should return a value that represents the consideration's state (e.g., current/max health). */
	UFUNCTION(BlueprintImplementableEvent, DisplayName ="GetInputValue")
	float ReceiveGetInputValue(const FBTUUtilityContext& Context) const;
	
	/**
	 * Override this in Blueprints to add custom lines to the node's description in the Behavior Tree.
	 * @param OutDescription Array of strings. Each entry will be a new line starting with "    - ".
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Utility", DisplayName = "Get Description Values")
	void ReceiveGetDescriptionValues(TArray<FString>& OutDescription) const;
};
