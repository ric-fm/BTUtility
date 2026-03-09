// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "Considerations/BTUUtilityConsideration_BlueprintBase.h"


float UBTUUtilityConsideration_BlueprintBase::GetInputValue(const FBTUUtilityContext& Context) const
{
	return ReceiveGetInputValue(Context);
}

void UBTUUtilityConsideration_BlueprintBase::GetDescriptionValues(TArray<FString>& OutValues) const
{
	return ReceiveGetDescriptionValues(OutValues);
}
