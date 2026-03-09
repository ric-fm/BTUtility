// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "BTUUtilityResponseCurves.generated.h"


/** 
 * @struct FBTUResponseCurveBase
 * 
 * Base struct for all utility response curves. 
 * Implementations define how a normalized input (0-1) is transformed.
 */
USTRUCT(meta = (Hidden))
struct FBTUResponseCurveBase
{
	GENERATED_BODY()
	
	virtual ~FBTUResponseCurveBase() = default;
	
	/**
	 * Transforms the input value based on the curve logic.
	 * @param Value Normalized input value (0.0 to 1.0).
	 * @return Transformed utility score, clamped between 0.0 and 1.0.
	 */
	float Evaluate(float Value) const
	{
		return FMath::Clamp(Evaluate_Internal(Value), 0.0f, 1.0f);
	}
	
protected:
	/** Actual curve implementation logic. To be overridden by child structs. */
	virtual float Evaluate_Internal(float InputValue) const { return 0.0f; };
};


/**
 * @struct FBTUResponseCurve_Constant
 * 
 * Returns a fixed utility value regardless of the input.
 * Useful for base priorities or fallback behaviors.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Constant"))
struct FBTUResponseCurve_Constant : public FBTUResponseCurveBase
{
	GENERATED_BODY()

protected:
	virtual float Evaluate_Internal(float InputValue) const override { return Value; }
	
	/* The normalized value of the curve */
	UPROPERTY(EditAnywhere, meta=(ClampMin=0.0f, ClampMax=1.0f))
	float Value = 1.0f;
};


/**
 * @struct FBTUResponseCurve_Linear
 * 
 * Direct mapping where Output = Input adjusted by slope and shifts.
 * The simplest form of response for proportional behaviors.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Linear"))
struct FBTUResponseCurve_Linear : public FBTUResponseCurveBase
{
	GENERATED_BODY()

protected:
	virtual float Evaluate_Internal(float InputValue) const override
	{
		return (Slope * (InputValue - XShift)) + YShift;
	}
	
	/** The steepness of the curve. Higher values create a sharper response. */
	UPROPERTY(EditAnywhere, meta=(UIMin=-10.0f, UIMax=10.0f))
	float Slope = 1.0f;
	
	/** Horizontal offset. Shifts the entire curve along the X axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float XShift = 0.0f;
	
	/** Vertical offset. Shifts the entire curve along the Y axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float YShift = 0.0f;
};


/**
 * @struct FBTUResponseCurve_Polynomial
 * 
 * Power-based curve (Input^Exp). 
 * Allows for accelerated (Exp > 1) or decelerated (Exp < 1) response rates.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Polynomial"))
struct FBTUResponseCurve_Polynomial : public FBTUResponseCurveBase
{
	GENERATED_BODY()

protected:
	virtual float Evaluate_Internal(float InputValue) const override 
	{ 
		return (Slope * FMath::Pow(InputValue - XShift, Exponent)) + YShift; 
	}
	
	/** The steepness of the curve. Higher values create a sharper response. */
	UPROPERTY(EditAnywhere, meta=(UIMin=-5.0f, UIMax=5.0f))
	float Slope = 1.0f;
	
	/** The degree of the function. Controls the curvature; values > 1 create an accelerated growth, while < 1 create a decelerated one. */
	UPROPERTY(EditAnywhere, meta=(UIMin=0.1f, UIMax=10.0f))
	float Exponent = 1.0f;
	
	/** Horizontal offset. Shifts the entire curve along the X axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float XShift = 0.0f;
	
	/** Vertical offset. Shifts the entire curve along the Y axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float YShift = 0.0f;
};


/**
 * @struct FBTUResponseCurve_Logistic
 * 
 * S-shaped sigmoid curve. 
 * Provides a smooth transition with slow growth at the start and end, and rapid growth in the middle.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Logistic"))
struct FBTUResponseCurve_Logistic : public FBTUResponseCurveBase
{
	GENERATED_BODY()

protected:
	virtual float Evaluate_Internal(float InputValue) const override 
	{ 
		return (Slope / (1.0f + FMath::Exp(-10.0f * Exponent * (InputValue - 0.5f - XShift)))) + YShift; 
	}
	
	/** The steepness of the curve. Higher values create a sharper response. */
	UPROPERTY(EditAnywhere, meta=(UIMin=-5.0f, UIMax=5.0f))
	float Slope = 1.0f;
	
	/** The degree of the function. Controls the curvature; values > 1 create an accelerated growth, while < 1 create a decelerated one. */
	UPROPERTY(EditAnywhere, meta=(UIMin=0.1f, UIMax=20.0f))
	float Exponent = 1.0f;
	
	/** Horizontal offset. Shifts the entire curve along the X axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float XShift = 0.0f;
	
	/** Vertical offset. Shifts the entire curve along the Y axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float YShift = 0.0f;
};


/**
 * @struct FBTUResponseCurve_Logit
 * 
 * Inverse of the logistic function. 
 * Maps probabilities to real numbers, useful for sensitive threshold detection near edges.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Logit"))
struct FBTUResponseCurve_Logit : public FBTUResponseCurveBase
{
	GENERATED_BODY()

protected:
	virtual float Evaluate_Internal(float InputValue) const override 
	{ 
		return Slope * FMath::Loge((InputValue - XShift) / (1.0 - (InputValue - XShift))) / 5.0 + 0.5 + YShift;
	}
	
	/** The steepness of the curve. Higher values create a sharper response. */
	UPROPERTY(EditAnywhere, meta=(UIMin=0.1f, UIMax=5.0f))
	float Slope = 1.0f;
	
	/** Horizontal offset. Shifts the entire curve along the X axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float XShift = 0.0f;
	
	/** Vertical offset. Shifts the entire curve along the Y axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float YShift = 0.0f;
};


/**
 * @struct FBTUResponseCurve_Normal
 * 
 * Gaussian / Bell curve distribution. 
 * Highest utility is concentrated around a specific mean value, tapering off at the edges.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Normal"))
struct FBTUResponseCurve_Normal : public FBTUResponseCurveBase
{
	GENERATED_BODY()

protected:
	virtual float Evaluate_Internal(float InputValue) const override 
	{ 
		return Slope * FMath::Exp(-30.0f * Exponent * FMath::Square(InputValue - XShift - 0.5f)) + YShift; 
	}
	
	/** The steepness of the curve. Higher values create a sharper response. */
	UPROPERTY(EditAnywhere, meta=(UIMin=-2.0f, UIMax=2.0f))
	float Slope = 1.0f;
	
	/** The degree of the function. Controls the curvature; values > 1 create an accelerated growth, while < 1 create a decelerated one. */
	UPROPERTY(EditAnywhere, meta=(UIMin=0.1f, UIMax=5.0f))
	float Exponent = 1.0f;
	
	/** Horizontal offset. Shifts the entire curve along the X axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float XShift = 0.0f;
	
	/** Vertical offset. Shifts the entire curve along the Y axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float YShift = 0.0f;
};


/**
 * @struct FBTUResponseCurve_Sine
 * 
 * Oscillatory response based on Sine waves. 
 * Ideal for periodic behaviors or organic "pulsing" utility patterns.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Sine"))
struct FBTUResponseCurve_Sine : public FBTUResponseCurveBase
{
	GENERATED_BODY()

protected:
	virtual float Evaluate_Internal(float InputValue) const override 
	{ 
		return 0.5f * Slope * FMath::Sin(2.0f * UE_PI * (InputValue - XShift)) + 0.5f + YShift; 
	}
	
	/** The steepness of the curve. Higher values create a sharper response. */
	UPROPERTY(EditAnywhere, meta=(UIMin=-5.0f, UIMax=5.0f))
	float Slope = 1.0f;
	
	/** Horizontal offset. Shifts the entire curve along the X axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float XShift = 0.0f;
	
	/** Vertical offset. Shifts the entire curve along the Y axis. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=-1.0f, ClampMax=1.0f))
	float YShift = 0.0f;
};


/**
 * @struct FBTUResponseCurve_Custom
 * 
 * Allows the use of a standard Unreal Engine UCureFloat asset. 
 * Provides maximum artistic control via the external Curve Editor.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Custom"))
struct FBTUResponseCurve_Custom : public FBTUResponseCurveBase
{
	GENERATED_BODY()

protected:
	virtual float Evaluate_Internal(float InputValue) const override 
	{ 
		return Curve.GetRichCurveConst()->Eval(InputValue);
	}
	
	// Native Unreal Runtime Curve
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve Curve;
};


/**
 * @struct FBTUResponseCurve
 * 
 * A wrapper struct designed to abstract the complexity of FInstancedStruct while 
 * enabling a dedicated Detail Customization in the Editor.
 * This abstraction allows for a Curve Preview Widget to be drawn 
 * within the Details Panel, providing immediate visual feedback to designers 
 * when adjusting mathematical parameters.
 */
USTRUCT(BlueprintType)
struct BTUTILITY_API FBTUResponseCurve
{
	GENERATED_BODY()
	
	/** Evaluates the internal curve. 
	 * @param InputValue Normalized input.
	 * @return The utility score (0-1).
	 */
	float Evaluate(float InputValue) const
	{
		if (const FBTUResponseCurveBase* CurvePtr = ResponseCurve.GetPtr<FBTUResponseCurveBase>())
		{
			return CurvePtr->Evaluate(InputValue);
		}
		
		return 0.0f;
	}
	
	/** Returns a human-readable name of the current curve type. */
	FString GetDescription() const
	{
		if (ResponseCurve.IsValid())
		{
			if (const UScriptStruct* ScriptStruct = ResponseCurve.GetScriptStruct())
			{
				FString Name = ScriptStruct->GetName();
				Name.RemoveFromStart(TEXT("BTUResponseCurve_"));
				return Name;
			}
		}

		return TEXT("None");
	}
	
	/** The actual curve implementation (Linear, Polynomial, Logistic, etc.) */
	UPROPERTY(EditAnywhere, meta = (ExcludeBaseStruct, BaseStruct = "/Script/BTUtility.BTUResponseCurveBase"))
	FInstancedStruct ResponseCurve;
};
