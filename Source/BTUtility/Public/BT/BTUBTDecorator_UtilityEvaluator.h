// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Core/BTUUtilityEvaluator.h"
#include "BTUBTDecorator_UtilityEvaluator.generated.h"


struct FBTUNodeMemory_UtilityEvaluator
{
	/** World time of the last successful considerations evaluation. */
	double LastEvalTime;
	
	/** The last calculated utility score, stored for debugging and visualization. */
	float LastScore;
	
	/** Cached result of the last threshold check to evaluate execution requests. */
	bool bLastRawResult;
};

/**
 * @class UBTUBTDecorator_UtilityEvaluator
 *	@see FBTUUtilityEvaluator
 *	@see UBTUUtilityConsideration
 *	
 * A conditional gate that validates branch viability through utility scoring.
 * It compares the integrated result of multiple considerations against a MinScore threshold to trigger reactive aborts.
 */
UCLASS(DisplayName = "Utility Evaluator", Category="Utility")
class BTUTILITY_API UBTUBTDecorator_UtilityEvaluator : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTUBTDecorator_UtilityEvaluator(const FObjectInitializer& ObjectInitializer);
	
#pragma region UBTDecorator
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	virtual FString GetStaticDescription() const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
#pragma endregion
	
private:
	
	bool CalcConditionImpl(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;
	
protected:
	
	/**  Utility Evaluator settings */
	UPROPERTY(EditAnywhere, Category="Utility", meta=(ShowOnlyInnerProperties))
	FBTUUtilityEvaluator Evaluator;
	
	/** Minimum utility score [0.0 - 1.0] required to pass the condition. */
	UPROPERTY(EditAnywhere, Category = Utility)
	float MinScore = 0.0f;
	
	/** Frequency of utility re-evaluation. */
	UPROPERTY(EditAnywhere, Category = Utility, AdvancedDisplay, meta=(ForceUnits="s", UIMin=0.0f, ClampMin=0.0f))
	float EvaluationInterval = 0.2f;
};
