// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "Considerations/BTUUtilityConsideration_Distance.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTUUtilityConsideration_Distance::UBTUUtilityConsideration_Distance()
{
	// TODO: Support FVector keys
	SourceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTUUtilityConsideration_Distance, SourceKey), AActor::StaticClass());
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTUUtilityConsideration_Distance, TargetKey), AActor::StaticClass());
}

float UBTUUtilityConsideration_Distance::GetInputValue(const FBTUUtilityContext& Context) const
{
	const AActor* SourceActor = Cast<AActor>(Context.Controller->GetBlackboardComponent()->GetValueAsObject(SourceKey.SelectedKeyName));
	const AActor* TargetActor = Cast<AActor>(Context.Controller->GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	
	// If we can't measure, we treat it as "out of range" (1.0 after normalization)
	if (!SourceActor || !TargetActor)
		return 1.0f;
	
	// TODO: Add settings to evaluate Distance, Distance2D, DistanceZ...
	const float Distance = FVector::Distance(SourceActor->GetActorLocation(), TargetActor->GetActorLocation());
	
	// Normalize distance
	return DistanceRange.GetRangePct(Distance);
}

void UBTUUtilityConsideration_Distance::GetDescriptionValues(TArray<FString>& OutValues) const
{
	Super::GetDescriptionValues(OutValues);
	
	OutValues.Add(FString::Printf(TEXT("Source: %s"), *SourceKey.SelectedKeyName.ToString()));
	OutValues.Add(FString::Printf(TEXT("Target: %s"), *TargetKey.SelectedKeyName.ToString()));
	OutValues.Add(FString::Printf(TEXT("Distance: [%.2f, %.2f]"), DistanceRange.Min, DistanceRange.Max));
}
