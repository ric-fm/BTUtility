// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "BT/BTUBTDecorator_Utility.h"

UBTUBTDecorator_Utility::UBTUBTDecorator_Utility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Utility";
	
	// Utility decorators don't use standard abort logic; they provide data to the Utility Selector.
	bAllowAbortChildNodes = false;
	bAllowAbortLowerPri = false;
	bAllowAbortNone = false;
	
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
}

float UBTUBTDecorator_Utility::Evaluate(UBehaviorTreeComponent& OwnerComp) const
{
	// Prepare evaluation context.
	FBTUUtilityContext Context(OwnerComp.GetAIOwner(), GetWorld()->GetTimeSeconds());
	
	// Calculate the Candidate score.
	const float Score = Evaluator.Evaluate(Context);
	
	// Store score in node memory for debugging.
	FBTUNodeMemory_Utility* Memory = CastInstanceNodeMemory<FBTUNodeMemory_Utility>(OwnerComp.GetNodeMemory(this, OwnerComp.GetActiveInstanceIdx()));
	Memory->LastScore = Score;
	
	return Score;
}

uint16 UBTUBTDecorator_Utility::GetInstanceMemorySize() const
{
	return sizeof(FBTUNodeMemory_Utility);
}

FString UBTUBTDecorator_Utility::GetStaticDescription() const
{
	return Evaluator.GetStaticDescription();
}

void UBTUBTDecorator_Utility::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);
	
	if (const FBTUNodeMemory_Utility* Memory = CastInstanceNodeMemory<FBTUNodeMemory_Utility>(NodeMemory))
	{
		Values.Add(FString::Printf(TEXT("Score: %.2f"), Memory->LastScore));
	}
}
