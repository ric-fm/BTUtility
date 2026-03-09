// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Core/BTUUtilityConsideration.h"
#include "BTUUtilityConsideration_Distance.generated.h"

/**
 * @class UBTUUtilityConsideration_Distance
 * 
 * Measures the Euclidean distance between the AI Pawn and a Target Actor stored in the Blackboard.
 * The result is fed into the curve pipeline to determine utility based on proximity.
 */
UCLASS(DisplayName="Distance")
class BTUTILITY_API UBTUUtilityConsideration_Distance : public UBTUUtilityConsideration
{
	GENERATED_BODY()
	
	UBTUUtilityConsideration_Distance();
	
#pragma region UUtilityConsideration
public:
	virtual float GetInputValue(const FBTUUtilityContext& Context) const override;
	
	virtual void GetDescriptionValues(TArray<FString>& OutValues) const override;
#pragma endregion
	
protected:
	
	/** Blackboard key containing the Actor to measure distance from. */
	UPROPERTY(EditAnywhere, Category = "Default")
	FBlackboardKeySelector SourceKey;
	
	/** Blackboard key containing the Actor to measure distance to. */
	UPROPERTY(EditAnywhere, Category = "Default")
	FBlackboardKeySelector TargetKey;
	
	/** The distance range [Min, Max] used to normalize the raw input. */
	UPROPERTY(EditAnywhere, Category = "Default")
	FFloatInterval DistanceRange = FFloatInterval(0.0f, 1000.0f);
};
