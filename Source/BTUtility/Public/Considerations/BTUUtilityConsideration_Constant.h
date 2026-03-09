// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Core/BTUUtilityConsideration.h"
#include "BTUUtilityConsideration_Constant.generated.h"

/**
 * @class UBTUUtilityConsideration_Constant
 * 
 * Provides a fixed utility score. Mainly used for debugging, 
 * calibration, or as a baseline score for complex utility integrations.
 */
UCLASS(DisplayName="Constant")
class BTUTILITY_API UBTUUtilityConsideration_Constant : public UBTUUtilityConsideration
{
	GENERATED_BODY()
	
#pragma region UUtilityConsideration
public:
	UBTUUtilityConsideration_Constant()
	{
		bUseCurve = false;	
	}
	
	virtual float Evaluate(const FBTUUtilityContext& Context) const override
	{
		return Value;
	};
	
	virtual void GetDescriptionValues(TArray<FString>& OutValues) const override
	{
		OutValues.Add(FString::Printf(TEXT("Value: %.2f"), Value));
	}
#pragma endregion
	
protected:
	
	/** Static value to be returned. */
	UPROPERTY(EditAnywhere, Category="Default", meta=(UIMin=0.0f, ClampMin=0.0f, UIMax=1.0f, ClampMax=1.0f))
	float Value = 1.0f;
};
