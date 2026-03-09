// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "BT/BTUBTService_UtilityTick.h"

#include "BT/BTUBTCompositeNode_UtilitySelector.h"

UBTUBTService_UtilityTick::UBTUBTService_UtilityTick(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = TEXT("Utility Tick");
	
	Interval = 1.0f;
	RandomDeviation = 0.0f;
	bRestartTimerOnEachActivation = true;
	
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

void UBTUBTService_UtilityTick::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const UBTUBTCompositeNode_UtilitySelector* UtilitySelector = GetUtilitySelectorNode();
	if (!UtilitySelector)
		return;
	
	const double CurrentTime = GetWorld()->GetTimeSeconds();
	
	// TickPrecisionMargin absorbs frame-rate fluctuations (jitter) to prevent premature
	// evaluations when the service tick nearly aligns with a recent evaluation.
	constexpr float TickPrecisionMargin = 0.1f;

	/**
	 * Dynamic Rescheduling:
	 * If the selector recently evaluated (e.g. during a transition or fallback), 
	 * we postpone the service tick to keep the desired Interval rhythm.
	 * TODO: Currently ignoring RandomDeviation for rescheduling logic.
	 */
	const float TimeSinceLastEval = static_cast<float>(CurrentTime - UtilitySelector->GetLastEvaluationTime(OwnerComp));

	if (TimeSinceLastEval < (Interval - TickPrecisionMargin))
	{
		const float NextTick = FMath::Max(TickPrecisionMargin, Interval - TimeSinceLastEval);
		SetNextTickTime(NodeMemory, NextTick);
		return;
	}

	// Trigger the actual utility evaluation and potential branch switch
	UtilitySelector->ProcessUtilityTick(OwnerComp, DeltaSeconds);

	/**
	 * Post-Tick Realignment:
	 * We re-calculate the next tick based on the (potentially updated) evaluation timestamp
	 * to ensure the monitoring rhythm stays consistent with the actual logic execution.
	 */
	const float PostTickTimeSinceEval = static_cast<float>(CurrentTime - UtilitySelector->GetLastEvaluationTime(OwnerComp));
	const float NextTick = FMath::Max(TickPrecisionMargin, Interval - PostTickTimeSinceEval);
    
	SetNextTickTime(NodeMemory, NextTick);
}

const UBTUBTCompositeNode_UtilitySelector* UBTUBTService_UtilityTick::GetUtilitySelectorNode()
{
	// Lazy Initialization: We cache the parent selector on the first access.
	if (!CachedUtilitySelector)
	{
		CachedUtilitySelector = Cast<UBTUBTCompositeNode_UtilitySelector>(GetMyNode());
		
		// Safety check for misconfigured Behavior Trees.
		ensureMsgf(CachedUtilitySelector, TEXT("UBTService_UtilityTick is attached to %s, but it only supports UBTCompositeNode_UtilitySelector!"), 
			*GetNameSafe(GetMyNode()));
	}
	
	return CachedUtilitySelector.Get();
}
