// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Core/BTUUtilityConsideration.h"
#include "BTUUtilityConsideration_Random.generated.h"

/**
 * @class UBTUUtilityConsideration_Random
 * 
 * Injects controlled randomness into the utility calculation.
 * Useful for breaking ties between behaviors or creating less predictable AI 
 * by adding "noise" to the scoring process.
 */
UCLASS(DisplayName="Random")
class BTUTILITY_API UBTUUtilityConsideration_Random : public UBTUUtilityConsideration
{
	GENERATED_BODY()
	
#pragma region UUtilityConsideration
public:
	virtual float Evaluate(const FBTUUtilityContext& Context) const override;
	
	virtual void GetDescriptionValues(TArray<FString>& OutValues) const override;
#pragma endregion
	
protected:
	
	/** The bounds for the random utility value. */
	UPROPERTY(EditAnywhere, Category = "Default", meta=(UIMin=0.0f, ClampMin=0.0f, UIMax=1.0f, ClampMax=1.0f))
	FFloatInterval Range = FFloatInterval(0.f, 1.f);
};
