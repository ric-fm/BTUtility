// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "BT/BTUBTService_LockUtility.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTUBTService_LockUtility::UBTUBTService_LockUtility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Lock Utility";
	
	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTUBTService_LockUtility, BlackboardKey));
	
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

void UBTUBTService_LockUtility::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BlackboardKey.SelectedKeyName, true);
}

void UBTUBTService_LockUtility::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BlackboardKey.SelectedKeyName, false);
}

FString UBTUBTService_LockUtility::GetStaticDescription() const
{
	return FString::Printf(TEXT("Key: %s"), *BlackboardKey.SelectedKeyName.ToString());
}
