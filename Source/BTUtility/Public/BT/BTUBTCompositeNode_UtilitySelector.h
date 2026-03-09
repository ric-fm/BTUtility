// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "Core/BTUUtilityAITypes.h"

#include "BTUBTCompositeNode_UtilitySelector.generated.h"


class UBTUBTDecorator_Utility;

struct FBTUNodeMemory_UtilitySelector : public FBTCompositeMemory
{
	// Total candidates found in the BT Asset. Used to bound the SnapshotData buffer.
	uint8 NumCandidates;
	
	// The score that triggered the current execution
	float ExecutionScore; 

	// Timestamp of the last successful utility scoring to throttle evaluations.
	double LastEvaluationTime;

	// Inline buffer of ranked candidates
	FBTUUtilityScoredCandidate* SnapshotData;

	/**
	 * Copies scored results into the persistent node memory.
	 * @param Scores - The temporary array of calculated utility scores.
	 */
	void SaveSnapshot(const TArray<FBTUUtilityScoredCandidate>& Scores)
	{
		if (NumCandidates > 0 && NumCandidates == Scores.Num())
		{
			const SIZE_T NumBytes = NumCandidates * sizeof(FBTUUtilityScoredCandidate);
			FMemory::Memcpy(SnapshotData, Scores.GetData(), NumBytes);
		}
	}

	/**
	 * Returns a mutable view of the current ranked candidates snapshot.
	 * Used internally to update or sort candidate data during a search.
	 * * @return TArrayView of the current ranked candidates.
	 */
	TArrayView<FBTUUtilityScoredCandidate> GetSnapshot()
	{
		return TArrayView<FBTUUtilityScoredCandidate>(SnapshotData, NumCandidates);
	}

	/**
	 * Returns a read-only view of the current ranked candidates snapshot.
	 * Ideal for selecting the next best candidate without modifying scores.
	 * @return Const TArrayView of the current ranked candidates.
	 */
	TArrayView<const FBTUUtilityScoredCandidate> GetSnapshot() const
	{
		return TArrayView<const FBTUUtilityScoredCandidate>(SnapshotData, NumCandidates);
	}
};

/**
 * @class UBTUBTCompositeNode_UtilitySelector
 *	@see UBTUBTDecorator_Utility
 *	@see UBTUBTService_UtilityTick
 *
 * A composite node that selects children based on dynamic utility scores.
 * 
 * Execution Logic:
 *	- Scans and ranks all children by evaluating their UBTDecorator_Utility.
 *	- Each child should have exactly one UBTDecorator_Utility. If multiple are present, only the first one is considered.
 *	- If a child lacks a Utility Decorator, it is assigned a score of 0 and effectively ignored.
 *	- All decorators on a child are pre-evaluated; if any decorator fails, the utility score is 0.
 *	- In case of a score tie, priority is resolved Left-to-Right in the BT.
 *	
 * Flow Control:
 *	- If a child node fails or is aborted, the selector moves to the next best valid candidate.
 *	- If a child node succeeds, the UtilitySelector finishes with Success.
 *  - Supports reactive re-evaluation when paired with a UBTService_UtilityTick, 
 *    allowing branch overrides if a better candidate exceeds the current ScoreThreshold.
 */
UCLASS(DisplayName="Utility Selector", Category="Utility")
class BTUTILITY_API UBTUBTCompositeNode_UtilitySelector : public UBTCompositeNode
{
	GENERATED_BODY()

	friend class UBTUBTService_UtilityTick;

public:
	UBTUBTCompositeNode_UtilitySelector(const FObjectInitializer& ObjectInitializer);

#pragma region UBTCompositeNode
	virtual void BeginDestroy() override;

	virtual uint16 GetInstanceMemorySize() const override;
	
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                              EBTMemoryInit::Type InitType) const override;
	
	virtual int32 GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild,
	                                  EBTNodeResult::Type LastResult) const override;

	virtual bool CanAbortSelf() const override;
	virtual bool CanAbortLowerPriority() const override;

	virtual FString GetStaticDescription() const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
#pragma endregion

private:
	/** Checks once per asset if children have utility decorators and caches them. */
	void InitUtilities() const;

	/** Simple getter for the service to check throttle logic. Used by @ref UBTService_UtilityTick */
	double GetLastEvaluationTime(const UBehaviorTreeComponent& OwnerComp) const;

	/** Main logic for the periodic utility tick. */
	void ProcessUtilityTick(UBehaviorTreeComponent& OwnerComp, float DeltaSeconds) const;

	/** Validates if a branch's decorators allow execution */
	bool CanExecuteChild(UBehaviorTreeComponent& OwnerComp, int32 ChildIndex) const;

	/** Performs the full scoring and ranking pass. */
	void EvaluateUtilities(UBehaviorTreeComponent& OwnerComp, FBTUNodeMemory_UtilitySelector* Memory,
	                       TArray<FBTUUtilityScoredCandidate>& OutResult) const;

	/** Individual candidate evaluation logic. */
	float EvaluateCandidate(UBehaviorTreeComponent& OwnerComp, int32 ChildIndex) const;

protected:
	/**
	 * The minimum score difference required to trigger a branch switch.
	 * Higher values prevent "oscillation" between tasks with similar scores, 
	 * while lower values make the AI more reactive to small utility changes.
	 */
	UPROPERTY(EditAnywhere, Category = Utility, meta=(UIMin = 0.0f, ClampMin = 0.0f, UIMax = 1.0f, ClampMax = 1.0f))
	float ScoreThreshold = 0.1f;

	/**
	 * If enabled, the selector will check a Blackboard boolean before allowing any utility-based abortion.
	 * Useful for "locking" the AI into an action (e.g., an unbreakable attack animation).
	 */
	UPROPERTY(EditAnywhere, Category = Utility)
	bool bCheckLockUtility = false;

	/**
	 * Blackboard key used to lock utility evaluation. 
	 * When the boolean value is true, the current branch cannot be interrupted by the Utility Service.
	 */
	UPROPERTY(EditAnywhere, Category = Utility, meta=(EditCondition=bCheckLockUtility))
	FBlackboardKeySelector UtilityLockKey;

private:
	/** Cached list of utility decorators for the children of this node. */
	UPROPERTY(Transient)
	mutable TArray<TObjectPtr<UBTUBTDecorator_Utility>> CachedUtilities;
	
	/** Guard to ensure InitUtilities logic only runs once per Asset lifecycle. */
	mutable bool bUtilitiesInitialized;
};
