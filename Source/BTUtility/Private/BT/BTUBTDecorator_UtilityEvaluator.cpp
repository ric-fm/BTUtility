// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "BT/BTUBTDecorator_UtilityEvaluator.h"

UBTUBTDecorator_UtilityEvaluator::UBTUBTDecorator_UtilityEvaluator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Utility Evaluator";
	
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
}

uint16 UBTUBTDecorator_UtilityEvaluator::GetInstanceMemorySize() const
{
	return sizeof(FBTUNodeMemory_UtilityEvaluator);
}

void UBTUBTDecorator_UtilityEvaluator::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryInit::Type InitType) const
{
	if (InitType == EBTMemoryInit::Initialize)
	{
		FBTUNodeMemory_UtilityEvaluator* Memory = CastInstanceNodeMemory<FBTUNodeMemory_UtilityEvaluator>(OwnerComp.GetNodeMemory(this, OwnerComp.GetActiveInstanceIdx()));
		Memory->LastEvalTime = TNumericLimits<double>::Lowest();
		Memory->LastScore = 0.0f;
		Memory->bLastRawResult = false;
	}
}

bool UBTUBTDecorator_UtilityEvaluator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	return CalcConditionImpl(OwnerComp, NodeMemory);
}

void UBTUBTDecorator_UtilityEvaluator::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Initial evaluation upon entering the branch. This synchronizes the state before the first TickNode call.
	FBTUNodeMemory_UtilityEvaluator* Memory = CastInstanceNodeMemory<FBTUNodeMemory_UtilityEvaluator>(NodeMemory);
	Memory->bLastRawResult = CalcConditionImpl(OwnerComp, NodeMemory);
	Memory->LastEvalTime = GetWorld()->GetTimeSeconds();
}

void UBTUBTDecorator_UtilityEvaluator::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTUNodeMemory_UtilityEvaluator* MyMemory = CastInstanceNodeMemory<FBTUNodeMemory_UtilityEvaluator>(NodeMemory);
	const double CurrentTime = GetWorld()->GetTimeSeconds();

	// Throttling logic: We only re-evaluate considerations if the defined EvaluationInterval has passed.
	if (CurrentTime - MyMemory->LastEvalTime >= EvaluationInterval)
	{
		const bool bCurrentResult = CalcConditionImpl(OwnerComp, NodeMemory);
		MyMemory->LastEvalTime = CurrentTime;

		// Request evaluation if the condition changed.
		if (bCurrentResult != MyMemory->bLastRawResult)
		{
			MyMemory->bLastRawResult = bCurrentResult;
			OwnerComp.RequestExecution(this);
		}
	}
}

FString UBTUBTDecorator_UtilityEvaluator::GetStaticDescription() const
{
	return FString::Printf(TEXT("%sMinScore: %.2f\nEvaluation Interval: %.2f s"), *Evaluator.GetStaticDescription(), MinScore, EvaluationInterval);
}

void UBTUBTDecorator_UtilityEvaluator::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);
	
	if (const FBTUNodeMemory_UtilityEvaluator* Memory = CastInstanceNodeMemory<FBTUNodeMemory_UtilityEvaluator>(NodeMemory))
	{
		Values.Add(FString::Printf(TEXT("Score: %.2f"), Memory->LastScore));
		
		if (Verbosity == EBTDescriptionVerbosity::Basic)
		{
			Values.Add(FString::Printf(TEXT("LastResult: %s"), Memory->bLastRawResult ? TEXT("true") : TEXT("false")));
		}
	}
}

bool UBTUBTDecorator_UtilityEvaluator::CalcConditionImpl(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	// Prepare evaluation context.
	FBTUUtilityContext Context(OwnerComp.GetAIOwner(), GetWorld()->GetTimeSeconds());
	
	const float Score = Evaluator.Evaluate(Context);
	
	// Store score in node memory for debugging.
	FBTUNodeMemory_UtilityEvaluator* Memory = CastInstanceNodeMemory<FBTUNodeMemory_UtilityEvaluator>(OwnerComp.GetNodeMemory(this, OwnerComp.GetActiveInstanceIdx()));
	Memory->LastScore = Score;
	
	return Score >= MinScore;
}
