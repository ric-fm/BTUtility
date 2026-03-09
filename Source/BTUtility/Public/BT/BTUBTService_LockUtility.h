// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTUBTService_LockUtility.generated.h"

/**
 * @class UBTUBTService_LockUtility
 * 
 * A service that manages a boolean "lock" in the Blackboard while a branch is active.
 * Use Case: Prevents the Utility Evaluator from switching to a different task 
 * by setting a flag that other considerations can use to lower their priority 
 * or that decorators can use to block execution.
 */
UCLASS(DisplayName = "Lock Utility", Category = "Utility", HideCategories=(Service))
class BTUTILITY_API UBTUBTService_LockUtility : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTUBTService_LockUtility(const FObjectInitializer& ObjectInitializer);
	
#pragma region UBTService
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
#pragma endregion
	
protected:
	
	/** Boolean key in the Blackboard that represents the locked state. */
	UPROPERTY(EditAnywhere, Category = Utility)
	FBlackboardKeySelector BlackboardKey;
};
