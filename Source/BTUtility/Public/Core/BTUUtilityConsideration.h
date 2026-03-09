// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BTUUtilityAITypes.h"
#include "BTUUtilityResponseCurves.h"
#include "BTUUtilityConsideration.generated.h"

class AAIController;
class UBlackboardComponent;


/**
 * @class UBTUUtilityConsideration
 * @see FBTUUtilityEvaluator
 * 
 * Abstract base class for utility inputs.
 * A consideration represents a single atomic test or data point used to calculate 
 * an AI's desire to perform a specific action.
 * Subclasses must implement Evaluate to return a normalized score [0.0 - 1.0].
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, DisplayName = "Consideration")
class BTUTILITY_API UBTUUtilityConsideration : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Calculates the utility score for this specific consideration.
	 *	@param Context Structured data containing the Querier Context.
	 *	@return A normalized utility value, typically between 0.0 and 1.0.
	 */
	virtual float Evaluate(const FBTUUtilityContext& Context) const
	{
		const float NormalizedInput = FMath::Clamp(GetInputValue(Context), 0.0f, 1.0f);
		return bUseCurve ? ResponseCurve.Evaluate(NormalizedInput) : NormalizedInput;
	};

	/** Returns a relevant description of the consideration for debugging and visualization. */
	FString GetDescription() const
	{
		FString Desc = FString::Printf(TEXT(" * %s"), *GetFriendlyName());
			
		TArray<FString> Values;
		GetDescriptionValues(Values);
		for (const FString& Value : Values)
		{
			Desc.Appendf(TEXT("\n    - %s"), *Value);
		}
		
		return Desc;
	};

	/** Get the friendly name for the consideration class. Used for debugging and visualization. */
	FString GetFriendlyName() const
	{
		// Removing the 'UtilityConsideration_' prefix or similar for cleaner editor UI
		FString FriendlyName;
#if WITH_EDITOR
		FriendlyName = GetClass()->GetDisplayNameText().ToString();
#else
		FriendlyName = GetClass()->GetName();
		GetClass()->GetDisplayNameText()
		FriendlyName.RemoveFromStart(TEXT("BTUUtilityConsideration_"));
		FriendlyName.RemoveFromEnd(TEXT("_C"));
#endif
		
		return FriendlyName;
	}

protected:
	/** To be implemented by children (e.g., return distance to target). This value will be clamped and normalized by the ResponseCurve. */
	virtual float GetInputValue(const FBTUUtilityContext& Context) const { return 0.0f; };
	
	/**
	 * Gathers specific parameters and internal state of the consideration to be displayed in the editor.
	 * Derived classes should override this to add their own debug information.
	 * * @param OutValues Array of strings where each entry represents a descriptive line.
	 */
	virtual void GetDescriptionValues(TArray<FString>& OutValues) const
	{
		if (bUseCurve)
		{
			OutValues.Add(FString::Printf(TEXT("Response Curve: %s"), *ResponseCurve.GetDescription()));
		}
	};

	/** If true, the raw input value will be transformed using the Response Curve. Disable this for purely binary considerations (e.g. Line of Sight). */
	UPROPERTY(EditDefaultsOnly, Category="Default")
	bool bUseCurve = true;

	/** Mathematical function used to map the normalized input [0,1] to a utility score [0,1]. Hidden if bUseCurve is false to keep the Details Panel clean. */
	UPROPERTY(EditAnywhere, Category="Default", meta=(EditCondition = "bUseCurve", EditConditionHides))
	FBTUResponseCurve ResponseCurve;
};
