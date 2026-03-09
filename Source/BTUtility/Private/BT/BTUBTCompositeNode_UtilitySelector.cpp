// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "BT/BTUBTCompositeNode_UtilitySelector.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BT/BTUBTDecorator_Utility.h"

UBTUBTCompositeNode_UtilitySelector::UBTUBTCompositeNode_UtilitySelector(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Utility Selector";
	
	UtilityLockKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTUBTCompositeNode_UtilitySelector, UtilityLockKey));

	INIT_COMPOSITE_NODE_NOTIFY_FLAGS();
}

void UBTUBTCompositeNode_UtilitySelector::BeginDestroy()
{
	// Clear references to allow GC to collect the decorators
	CachedUtilities.Empty();

	Super::BeginDestroy();
}

uint16 UBTUBTCompositeNode_UtilitySelector::GetInstanceMemorySize() const
{
	// Take into account the number of elements in SnapshotData for the Memory size
	return sizeof(FBTUNodeMemory_UtilitySelector) + GetChildrenNum() * sizeof(FBTUUtilityScoredCandidate);
}

void UBTUBTCompositeNode_UtilitySelector::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                        EBTMemoryInit::Type InitType) const
{
	Super::InitializeMemory(OwnerComp, NodeMemory, InitType);

	if (InitType == EBTMemoryInit::Initialize)
	{
		// Ensure utility decorators are cached before first evaluation
		InitUtilities();

		FBTUNodeMemory_UtilitySelector* Memory = CastInstanceNodeMemory<FBTUNodeMemory_UtilitySelector>(NodeMemory);

		const int8 NumCandidates = CachedUtilities.Num();

		// Setup snapshot metadata
		Memory->NumCandidates = NumCandidates;
		Memory->ExecutionScore = 0.0f;
		
		// Initialize with the lowest possible value to force immediate evaluation on first tick
		Memory->LastEvaluationTime = TNumericLimits<double>::Lowest();

		/**
		* Map the SnapshotData pointer to the extra memory allocated at the end of the 
		* FNodeMemory_UtilitySelector struct. This ensures the snapshot is contiguous 
		* with the rest of the node's memory, improving cache locality and avoiding heap allocations.
		*/
		Memory->SnapshotData = reinterpret_cast<FBTUUtilityScoredCandidate*>(NodeMemory + sizeof(FBTUNodeMemory_UtilitySelector));
		
		// Initialize the buffer with default values to ensure data consistency
		for (uint8 ChildIndex = 0; ChildIndex < NumCandidates; ++ChildIndex)
		{
			Memory->SnapshotData[ChildIndex] = FBTUUtilityScoredCandidate(ChildIndex, 0.0f);
		}
	}
}

int32 UBTUBTCompositeNode_UtilitySelector::GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild,
                                                            EBTNodeResult::Type LastResult) const
{
	// If child succeeded the Utility Selector scope is considered complete 
	if (Children.IsValidIndex(PrevChild) && LastResult == EBTNodeResult::Succeeded)
		return BTSpecialChild::ReturnToParent;

	FBTUNodeMemory_UtilitySelector* Memory = GetNodeMemory<FBTUNodeMemory_UtilitySelector>(SearchData);
	
	// FailedMarginTime: Threshold to differentiate between a fresh evaluation and a rapid failure of a previously selected task.
	const double TimeElapsedSinceLastEvaluation = GetWorld()->GetTimeSeconds() - Memory->LastEvaluationTime;
	constexpr double FailedMarginTime = 0.2;

	if (TimeElapsedSinceLastEvaluation > FailedMarginTime)
	{
		// Fresh Evaluation: Triggered if enough time has passed since the last score calculation.
		// This handles initial startup and cases where a task was aborted (e.g., by TimeLimit).
		TArray<FBTUUtilityScoredCandidate> Candidates;
		EvaluateUtilities(SearchData.OwnerComp, Memory, Candidates);
		
		if (ensure(Candidates.Num() == Memory->NumCandidates))
		{
			Memory->SaveSnapshot(Candidates);
		}
		
		if ((Candidates.IsValidIndex(0) && Candidates[0].IsValid()))
		{
			Memory->ExecutionScore = Candidates[0].Score;
			return Candidates[0].Index;
		}
		
		return BTSpecialChild::ReturnToParent;
	}
	
	if (PrevChild == BTSpecialChild::NotInitialized)
	{
		// Forced Re-entry: If NotInitialized, it means we arrived here via ProcessUtilityTick's RequestExecution.
		// We reuse the existing snapshot already calculated during the tick to avoid redundant calculations.
		const TArrayView<const FBTUUtilityScoredCandidate> Candidates = Memory->GetSnapshot();
		if ((Candidates.IsValidIndex(0) && Candidates[0].IsValid()))
		{
			Memory->ExecutionScore = Candidates[0].Score;
			return Candidates[0].Index;
		}
		
		return BTSpecialChild::ReturnToParent;
	}
	
	if (PrevChild >= 0 && LastResult == EBTNodeResult::Failed)
	{
		// Fallback/Sequence Logic: If a child failed almost instantly (within the margin), 
		// we iterate through the ranked snapshot to find the next best available candidate.
		const TArrayView<const FBTUUtilityScoredCandidate> Candidates = Memory->GetSnapshot();

		int32 CurrentChildIndex = Candidates.IndexOfByPredicate(
			[Index=PrevChild](const FBTUUtilityScoredCandidate& A)
			{
				return A.Index == Index;
			});

		const int32 NextCandidateIndex = CurrentChildIndex + 1;
		if ((Candidates.IsValidIndex(NextCandidateIndex) && Candidates[NextCandidateIndex].IsValid()))
		{
			Memory->ExecutionScore = Candidates[NextCandidateIndex].Score;
			return Candidates[NextCandidateIndex].Index;
		}
		
		return BTSpecialChild::ReturnToParent;
	}
	
	// Default fallback: No valid candidates found
	return BTSpecialChild::ReturnToParent;
}

bool UBTUBTCompositeNode_UtilitySelector::CanAbortSelf() const
{
	/**
	 * Allows the node to abort its current execution branch. 
	 * This is essential for Utility Selectors to remain reactive, permitting 
	 * internal decorators or services to trigger a re-evaluation of candidates.
	 */
	return true;
}

bool UBTUBTCompositeNode_UtilitySelector::CanAbortLowerPriority() const
{
	/**
	 * Disables standard priority-based aborts for lower nodes.
	 * In a Utility system, execution flow is driven by dynamic scores rather than 
	 * static left-to-right priority, so lower priority aborts are managed 
	 * via the Utility Tick service.
	 */
	return false;
}

FString UBTUBTCompositeNode_UtilitySelector::GetStaticDescription() const
{
	// Display the score threshold used for branch switching
	FString Desc = FString::Printf(TEXT("Threshold: %.2f"), ScoreThreshold);

	// If utility locking is enabled, show the associated Blackboard key
	if (bCheckLockUtility)
	{
		Desc.Append(TEXT("\n"));
		Desc.Appendf(TEXT("Lock: %s"), *UtilityLockKey.SelectedKeyName.ToString());
	}

	return Desc;
}

void UBTUBTCompositeNode_UtilitySelector::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);
	
	const FBTUNodeMemory_UtilitySelector* Memory = CastInstanceNodeMemory<FBTUNodeMemory_UtilitySelector>(NodeMemory);
	if (Memory && Memory->ExecutionScore > 0.0f)
	{
		Values.Add(FString::Printf(TEXT("Score: %.2f"), Memory->ExecutionScore));
	}
}

void UBTUBTCompositeNode_UtilitySelector::InitUtilities() const
{
	/**
	 * Scans children for Utility Decorators and caches them.
	 * This is performed during InitializeMemory because it's the safest point 
	 * where all node instances are fully resolved and stable in memory.
	 */
	if (bUtilitiesInitialized)
		return;
	
	// Initialize array size to match children count
	CachedUtilities.Reserve(Children.Num());

	for (const FBTCompositeChild& Child : Children)
	{
		UBTUBTDecorator_Utility* FoundUtility = nullptr;

		for (UBTDecorator* Decorator : Child.Decorators)
		{
			if (UBTUBTDecorator_Utility* Utility = Cast<UBTUBTDecorator_Utility>(Decorator))
			{
				FoundUtility = Utility;
				break;
			}
		}

		CachedUtilities.Emplace(FoundUtility);
	}

	bUtilitiesInitialized = true;
}

double UBTUBTCompositeNode_UtilitySelector::GetLastEvaluationTime(const UBehaviorTreeComponent& OwnerComp) const
{
	FBTUNodeMemory_UtilitySelector* Memory = CastInstanceNodeMemory<FBTUNodeMemory_UtilitySelector>(OwnerComp.GetNodeMemory(this, OwnerComp.GetActiveInstanceIdx()));
	return Memory->LastEvaluationTime;
}

void UBTUBTCompositeNode_UtilitySelector::ProcessUtilityTick(UBehaviorTreeComponent& OwnerComp,
                                                          float DeltaSeconds) const
{
	uint8* RawMemory = OwnerComp.GetNodeMemory(this, OwnerComp.GetActiveInstanceIdx());
	FBTUNodeMemory_UtilitySelector* Memory = reinterpret_cast<FBTUNodeMemory_UtilitySelector*>(RawMemory);

	// Utility Locking: Check if the AI is currently forced to stay in the active branch
	// TODO: Improve the lock system, I think a simple BlackboardKey is not enough. Maybe GameplayTags and GameplayTagQuery is a better approach.
	if (bCheckLockUtility && OwnerComp.GetBlackboardComponent()->GetValueAsBool(UtilityLockKey.SelectedKeyName))
		return;
	
	TArray<FBTUUtilityScoredCandidate> Candidates;
	EvaluateUtilities(OwnerComp, Memory, Candidates);

	// Find where our currently executing child stands in the new scores
	const int32 CurrentChildIndexInResult = Candidates.IndexOfByPredicate(
		[Index = Memory->CurrentChild](const FBTUUtilityScoredCandidate& A)
		{
			return A.Index == Index;
		});

	if (CurrentChildIndexInResult == INDEX_NONE)
		return;
	
	/** 
	 * Since 'Candidates' are filtered and sorted by score, the optimal choice is always at index 0,
	 * we trigger a transition only if the best candidate is not the current one and the utility gain
	 * exceeds the ScoreThreshold.
	 */
	// TODO: Add Dynamic Threshold / Intertialisation?
	if (CurrentChildIndexInResult != 0 && (Candidates[0].Score - Candidates[CurrentChildIndexInResult].Score) >= ScoreThreshold)
	{
		// Save the current evaluation so GetNextChildHandler can use it immediately
		Memory->SaveSnapshot(Candidates);
        
		/** 
		 * To ensure a clean transition and reset decorators, we request execution from the Parent node.
		 * This forces the tree to flow back through GetNextChildHandler, maintaining internal engine consistency.
		 */
		const UBTCompositeNode* Parent = GetParentNode();
		const int32 ParentInstanceIdx = OwnerComp.FindInstanceContainingNode(Parent);
		const int32 ThisChildIndexInParent = Parent->GetChildIndex(*this);
		OwnerComp.RequestExecution(Parent, ParentInstanceIdx, this, ThisChildIndexInParent, EBTNodeResult::Aborted);
	}
}

bool UBTUBTCompositeNode_UtilitySelector::CanExecuteChild(UBehaviorTreeComponent& OwnerComp, int32 ChildIndex) const
{
	// We could use DoDecoratorsAllowExecution but has side effects for BT debugger
	// return DoDecoratorsAllowExecution(OwnerComp, OwnerComp.GetActiveInstanceIdx(), ChildIdx);

	if (!Children.IsValidIndex(ChildIndex))
		return false;

	for (UBTDecorator* Decorator : Children[ChildIndex].Decorators)
	{
		if (!Decorator || !Decorator->WrappedCanExecute(OwnerComp, OwnerComp.GetNodeMemory(Decorator, OwnerComp.GetActiveInstanceIdx())))
		{
			return false;
		}
	}

	// All decorators passed
	return true;
}

void UBTUBTCompositeNode_UtilitySelector::EvaluateUtilities(UBehaviorTreeComponent& OwnerComp,
                                                         FBTUNodeMemory_UtilitySelector* Memory,
                                                         TArray<FBTUUtilityScoredCandidate>& OutResult) const
{
	const double CurrentTime = GetWorld()->GetTimeSeconds();

	// Cache the evaluation time to manage tick scheduling
	Memory->LastEvaluationTime = CurrentTime;

	FBTUUtilityContext Context(OwnerComp.GetAIOwner(), GetWorld()->GetTimeSeconds());

	// Reset result array while preserving its capacity based on child count
	OutResult.Reset(CachedUtilities.Num());

	// Evaluate candidates
	for (int32 ChildIndex = 0; ChildIndex < CachedUtilities.Num(); ++ChildIndex)
	{
		const float Score = EvaluateCandidate(OwnerComp, ChildIndex);
		OutResult.Emplace(ChildIndex, Score);
	}

	/**
	 * Ranking: Sort candidates by score in descending order.
	 * In case of a tie, the candidate with the lower index (left-most in the BT) 
	 * takes priority to ensure deterministic behavior.
	 */
	OutResult.Sort([](const FBTUUtilityScoredCandidate& A, const FBTUUtilityScoredCandidate& B)
	{
		if (FMath::IsNearlyEqual(A.Score, B.Score))
		{
			return A.Index < B.Index;
		}
		
		return A.Score > B.Score;
	});
}

float UBTUBTCompositeNode_UtilitySelector::EvaluateCandidate(UBehaviorTreeComponent& OwnerComp, int32 ChildIndex) const
{
	// Ensure both cached utilities and children arrays are consistent
	if (!ensure(CachedUtilities.IsValidIndex(ChildIndex) && Children.IsValidIndex(ChildIndex)))
		return 0.0f;

	// A child without a Utility Decorator is considered to have zero utility
	const UBTUBTDecorator_Utility* Utility = CachedUtilities[ChildIndex];
	if (!Utility)
		return 0.0f;

	// If any decorator fails its condition, the candidate is disqualified and returns a score of 0.
	for (UBTDecorator* Decorator : Children[ChildIndex].Decorators)
	{
		if (!Decorator->WrappedCanExecute(OwnerComp, OwnerComp.GetNodeMemory(Decorator, OwnerComp.GetActiveInstanceIdx())))
		{
			return 0.0f;
		}
	}

	// Calculate final utility score using the specific Utility Decorator logic
	return Utility->Evaluate(OwnerComp);
}
