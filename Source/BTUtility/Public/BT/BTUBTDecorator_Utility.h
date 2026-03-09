// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Core/BTUUtilityEvaluator.h"
#include "BTUBTDecorator_Utility.generated.h"


struct FBTUNodeMemory_Utility
{
	/** The last calculated utility score, stored for debugging and visualization. */
	float LastScore;
};

/**
 * @class UBTUBTDecorator_Utility
 *	@see FBTUUtilityEvaluator
 *	@see UBTUBTCompositeNode_UtilitySelector
 *	@see UBTUUtilityConsideration
 *	
 * Utility Decorator: Evaluates a set of considerations to produce a score [0, 1].
 * This score can be used to drive dynamic decision-making within the Behavior Tree.
 */
UCLASS(DisplayName="Utility", Category="Utility")
class BTUTILITY_API UBTUBTDecorator_Utility : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	
	UBTUBTDecorator_Utility(const FObjectInitializer& ObjectInitializer);
	
#pragma region UBTDecorator	
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
#pragma endregion
	
	/** Calculate and return the utility for this node. Saves the last score in Memory */
	float Evaluate(UBehaviorTreeComponent& OwnerComp) const;
	
protected:
	
	/**  Utility Evaluator settings */
	UPROPERTY(EditAnywhere, Category="Utility", meta=(ShowOnlyInnerProperties))
	FBTUUtilityEvaluator Evaluator;
};
