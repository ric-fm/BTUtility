// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "Core/BTUUtilityEvaluator.h"
#include "Core/BTUUtilityConsideration.h"


float FBTUUtilityEvaluator::Evaluate(const FBTUUtilityContext& Context) const
{
	if (!Context.IsValid())
		return 0.0f;
	
	switch (IntegrationMode)
	{
	case EBTUUtilityIntegrationMode::Average:
		return CalculateAverage(Context);
	case EBTUUtilityIntegrationMode::Multiplicative:
		return CalculateMultiplicative(Context);
	}
	
	return 0.0f;
}

FString FBTUUtilityEvaluator::GetStaticDescription() const
{
	FString ConsiderationsDesc = FString::Printf(TEXT("IntegrationMode: %s\nConsiderations:\n"), *UEnum::GetDisplayValueAsText(IntegrationMode).ToString());
	
	for (const UBTUUtilityConsideration* Consideration : Considerations)
	{
		if (Consideration)
		{
			ConsiderationsDesc.Appendf(TEXT("%s\n"), *Consideration->GetDescription());
		}
	}
	
	return ConsiderationsDesc;
}

float FBTUUtilityEvaluator::CalculateAverage(const FBTUUtilityContext& Context) const
{
	// If no considerations are present, we assume the behavior is fully viable by default.
	if (Considerations.IsEmpty())
		return 1.0f;
    
	float Sum = 0.0f;
	int32 ValidConsiderations = 0;
    
	for (const UBTUUtilityConsideration* Consideration : Considerations)
	{
		// Skip consideration if not assigned. Just in case
		if (!Consideration)
			continue;

		// Scores are normalized [0,1]
		const float RawScore = FMath::Clamp(Consideration->Evaluate(Context), 0.0f, 1.0f);
		Sum += RawScore;
		ValidConsiderations++;
	}

	return (ValidConsiderations > 0) ? (Sum / static_cast<float>(ValidConsiderations)) : 0.0f;
}

float FBTUUtilityEvaluator::CalculateMultiplicative(const FBTUUtilityContext& Context) const
{
	// If no considerations are present, we assume the behavior is fully viable by default.
	const int32 NumConsiderations = Considerations.Num();
	if (NumConsiderations == 0)
		return 1.0f;

	/**
	 * The Modification Factor prevents 'probability collapse' or score decay.
	 * In multiplicative systems, increasing the number of considerations (N) 
	 * mathematically biases the result toward zero. This factor normalizes 
	 * the impact of each criteria relative to N, ensuring that complex behaviors 
	 * with many requirements can remain competitive against simpler ones.
	 * 
	 *	@ref https://www.gdcvault.com/play/1021848/Building-a-Better-Centaur-AI
	 */
	const float ModificationFactor = 1.0f - (1.0f / static_cast<float>(NumConsiderations));
	float CombinedScore = 1.0f;

	for (const UBTUUtilityConsideration* Consideration : Considerations)
	{
		if (!Consideration)
			continue;
       
		// Scores are normalized [0,1] to ensure predictable behavior across different criteria.
		// Clamp the socre just in case
		const float RawScore = FMath::Clamp(Consideration->Evaluate(Context), 0.0f, 1.0f);

		// Any absolute failure (0.0) invalidates the entire action immediately.
		if (FMath::IsNearlyZero(RawScore))
			return 0.0f;

		// Applying the compensation logic
		const float MakeUpValue = (1.0f - RawScore) * ModificationFactor;
		const float CompensatedScore = RawScore + (MakeUpValue * RawScore);

		CombinedScore *= CompensatedScore;
	}

	return CombinedScore;
}
