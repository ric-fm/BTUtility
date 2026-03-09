// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "BTUResponseCurvePreviewWidget.h"

#include "Core/BTUUtilityResponseCurves.h"

FVector2D GetWidgetPosition(float X, float Y, const FGeometry& Geom)
{
	return FVector2D((X * Geom.GetLocalSize().X), (Geom.GetLocalSize().Y - 1) - (Y * Geom.GetLocalSize().Y));
}

int32 SBTUResponseCurvePreviewWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                          const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                                          const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	bool bEnabled = ShouldBeEnabled(bParentEnabled);
	ESlateDrawEffect DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	
	const FSlateBrush* BackgroundBrush = FAppStyle::GetBrush("Profiler.LineGraphArea");
	const FSlateBrush* AxisBrush = FAppStyle::GetBrush("WhiteTexture");
	
	constexpr FLinearColor CurveColor = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);
	const FLinearColor AxisColor = AxisBrush->GetTint(InWidgetStyle) * 0.5f;

	// Background
	FSlateDrawElement::MakeBox(OutDrawElements, LayerId,
							   AllottedGeometry.ToPaintGeometry(FVector2D(0, 0), FSlateLayoutTransform(FVector2D(AllottedGeometry.Size.X, AllottedGeometry.Size.Y))),
							   BackgroundBrush, DrawEffects, BackgroundBrush->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint());
	
	// Draw Axis
	TArray<FVector2D> AxisPoints;
	AxisPoints.Add(GetWidgetPosition(0, 1, AllottedGeometry));
	AxisPoints.Add(GetWidgetPosition(0, 0, AllottedGeometry));
	AxisPoints.Add(GetWidgetPosition(1, 0, AllottedGeometry));

	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), AxisPoints, DrawEffects,
								 InWidgetStyle.GetColorAndOpacityTint() * AxisColor);
	
	// Skip if no Valid Curve
	if (!ResponseCurveInstance || !ResponseCurveInstance->IsValid())
		return LayerId;

	const FBTUResponseCurveBase* CurveData = ResponseCurveInstance->GetPtr<FBTUResponseCurveBase>();
	if (!CurveData)
		return LayerId;

	// Draw Curve
	constexpr int32 NumSamples = 100;
	TArray<FVector2D> Points;
	
	for (int32 i = 0; i <= NumSamples; ++i)
	{
		const float X = static_cast<float>(i) / NumSamples;
		const float Y = CurveData->Evaluate(X);

		Points.Add(GetWidgetPosition(X, Y, AllottedGeometry));
	}

	FSlateDrawElement::MakeLines(OutDrawElements, LayerId++, AllottedGeometry.ToPaintGeometry(), Points, 
								DrawEffects, InWidgetStyle.GetColorAndOpacityTint() * CurveColor, true, 2.0f);

	return LayerId;
}
