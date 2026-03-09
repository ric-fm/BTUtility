// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once


#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"
#include "StructUtils/InstancedStruct.h"

/**
 * @class SBTUResponseCurvePreviewWidget
 * 
 * A custom Slate widget that renders a visual preview of a Utility Response Curve.
 */
class SBTUResponseCurvePreviewWidget : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SBTUResponseCurvePreviewWidget) {}

	SLATE_END_ARGS()

#pragma region SLeafWidget
	void Construct(const FArguments& InArgs) {}

	virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D(256, 64); }

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const override;
#pragma endregion
	
	/** Reference to the instanced struct containing the curve data to draw. */
	FInstancedStruct* ResponseCurveInstance = nullptr;
};
