// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BTUUtilityAITypes.generated.h"

class UBlackboardComponent;
class AAIController;


/**
 * @struct FBTUUtilityContext
 * 
 * Provides the necessary agent data and world state to Utility Considerations.
 */
USTRUCT(BlueprintType)
struct FBTUUtilityContext
{
	GENERATED_BODY()
	
	/** The AI Controller evaluating the utility. */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAIController> Controller;

	/** Current world time, used to normalize time-dependent considerations (e.g., cooldowns). */
	double TimeStamp;
	
	FBTUUtilityContext() = default;
	FBTUUtilityContext(AAIController* InController, double InTimeStamp) : Controller(InController), TimeStamp(InTimeStamp) {};
	
	bool IsValid() const { return Controller != nullptr && Controller->GetPawn() != nullptr; }
};

/**
 * @struct FBTUUtilityScoredCandidate
 * 
 * Represents a potential child node and its calculated utility score.
 * This structure is used for ranking and snapshotting candidate utility.
 */
struct FBTUUtilityScoredCandidate
{
	// Index of the child node in the Composite's Children array.
	// Uses int8 to align with FBTCompositeMemory's internal child indexing.
	int8 Index = INDEX_NONE;

	// The calculated utility score for this candidate.
	float Score = 0.0f;

	FBTUUtilityScoredCandidate() = default;

	FBTUUtilityScoredCandidate(int8 InIndex, float InScore) : Index(InIndex), Score(InScore)
	{
	}

	bool IsValid() const { return Index != INDEX_NONE && Score > 0.0f; }
};

/**
 * @enum EBTUUtilityIntegrationMode
 * 
 * Defines how multiple considerations are combined into a single final score.
 */
UENUM(BlueprintType)
enum class EBTUUtilityIntegrationMode : uint8
{
	/** 
	 * [Balanced]
	 * A flexible mode where all considerations contribute to a common score. 
	 * Low scores in one area are naturally offset by high scores in others, 
	 * resulting in a more stable and "forgiving" final value.
	 */
	Average UMETA(DisplayName = "Average"),

	/** 
	 * [Sensitive]
	 * A strict mode where the final score is highly sensitive to the lowest values. 
	 * Even if most scores are high, a single low score will significantly pull 
	 * down the entire result, making this mode ideal for high-stakes conditions.
	 */
	Multiplicative UMETA(DisplayName = "Multiplicative")
};
