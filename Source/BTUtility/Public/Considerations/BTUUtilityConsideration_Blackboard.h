// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "Core/BTUUtilityConsideration.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTUUtilityConsideration_Blackboard.generated.h"


/**
 * @class UBTUUtilityConsideration_Blackboard
 * 
 * A consideration that retrieves numerical data (Float or Int) from the Blackboard
 * and processes it through a normalization interval and response curve.
 */
UCLASS(DisplayName="Blackboard")
class BTUTILITY_API UBTUUtilityConsideration_Blackboard : public UBTUUtilityConsideration
{
	GENERATED_BODY()
	
	UBTUUtilityConsideration_Blackboard();
	
#pragma region UUtilityConsideration
public:
	virtual float GetInputValue(const FBTUUtilityContext& Context) const override;
	
	virtual void GetDescriptionValues(TArray<FString>& OutValues) const override;
#pragma endregion
	
protected:
	
	/** The Blackboard key to be evaluated using the Curve. */
	UPROPERTY(EditAnywhere, Category = "Default")
	FBlackboardKeySelector BlackboardKey;
	
	/** Mapping Range to normalize the BlackboardKey value. */
	UPROPERTY(EditAnywhere, Category = "Default")
	FFloatInterval MappingRange = FFloatInterval(0.0f, 1.0f);
};
