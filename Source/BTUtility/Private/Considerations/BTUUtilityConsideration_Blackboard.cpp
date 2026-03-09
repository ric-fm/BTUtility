// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "Considerations/BTUUtilityConsideration_Blackboard.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"

UBTUUtilityConsideration_Blackboard::UBTUUtilityConsideration_Blackboard()
{
	BlackboardKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTUUtilityConsideration_Blackboard, BlackboardKey));
	BlackboardKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTUUtilityConsideration_Blackboard, BlackboardKey));
}

float UBTUUtilityConsideration_Blackboard::GetInputValue(const FBTUUtilityContext& Context) const
{
	return BlackboardKey.SelectedKeyType == UBlackboardKeyType_Float::StaticClass()
		       ? Context.Controller->GetBlackboardComponent()->GetValueAsFloat(BlackboardKey.SelectedKeyName)
		       : Context.Controller->GetBlackboardComponent()->GetValueAsInt(BlackboardKey.SelectedKeyName);
}

void UBTUUtilityConsideration_Blackboard::GetDescriptionValues(TArray<FString>& OutValues) const
{
	Super::GetDescriptionValues(OutValues);
	
	OutValues.Add(FString::Printf(TEXT("Key: %s"), *BlackboardKey.SelectedKeyName.ToString()));
	OutValues.Add(FString::Printf(TEXT("Mapping Range: [%.2f, %.2f]"), MappingRange.Min, MappingRange.Max));
}
