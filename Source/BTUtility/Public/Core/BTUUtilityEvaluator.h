// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "BTUUtilityAITypes.h"

#include "BTUUtilityEvaluator.generated.h"


class UBTUUtilityConsideration;

/**
 * @struct FBTUUtilityEvaluator
 *	@see UBTUUtilityConsideration
 *	
 * Core engine for Utility AI evaluation.
 * Encapsulates the logic for integrating multiple considerations into a single score.
 */
USTRUCT(BlueprintType)
struct BTUTILITY_API FBTUUtilityEvaluator
{
	GENERATED_BODY()
	
public:
	/** Calculate and return the utility for this node*/
	float Evaluate(const FBTUUtilityContext& Context) const;
	
	/** Returns a formatted string describing the evaluator's configuration for the BT editor. */
	FString GetStaticDescription() const;

protected:
	
	/** Combines considerations using a standard mean. */
	float CalculateAverage(const FBTUUtilityContext& Context) const;
	
	/** Combines considerations using a sensitive product-based approach */
	float CalculateMultiplicative(const FBTUUtilityContext& Context) const;
	
	/** The logic used to integrate multiple consideration scores. */
	UPROPERTY(EditAnywhere, Category="Utility")
	EBTUUtilityIntegrationMode IntegrationMode = EBTUUtilityIntegrationMode::Average;
	
	/** List of individual considerations (Distance, Health, LOS, etc.) to be evaluated. */
	UPROPERTY(EditAnywhere, Category="Utility")
	TArray<TObjectPtr<UBTUUtilityConsideration>> Considerations;
};
