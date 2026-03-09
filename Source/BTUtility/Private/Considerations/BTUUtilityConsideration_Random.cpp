// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "Considerations/BTUUtilityConsideration_Random.h"

float UBTUUtilityConsideration_Random::Evaluate(const FBTUUtilityContext& Context) const
{
	return FMath::RandRange(Range.Min, Range.Max);
}

void UBTUUtilityConsideration_Random::GetDescriptionValues(TArray<FString>& OutValues) const
{
	Super::GetDescriptionValues(OutValues);
	
	OutValues.Add(FString::Printf(TEXT("Range: [%.2f, %.2f]"), Range.Min, Range.Max));
}
