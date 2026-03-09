// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "BTUResponseCurveDetails.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Core/BTUUtilityResponseCurves.h"
#include "BTUResponseCurvePreviewWidget.h"

void FBTUResponseCurveDetails::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
                                           IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// TODO: Review this, I think it look like cleaner without Struct header collapse
	// HeaderRow.NameContent()
	// [
	// 	PropertyHandle->CreatePropertyNameWidget()
	// ];
}

void FBTUResponseCurveDetails::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
                                             IDetailChildrenBuilder& ChildBuilder,
                                             IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (!PropertyHandle->IsValidHandle())
		return;

	// Display the original child properties for the struct.
	uint32 NumChildren = 0;
	PropertyHandle->GetNumChildren(NumChildren);
	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		ChildBuilder.AddProperty(PropertyHandle->GetChildHandle(ChildIndex).ToSharedRef());
	}

	// Get the Utility Curve data
	FBTUResponseCurve* StructPtr = nullptr;

	void* StructData = nullptr;
	if (PropertyHandle->GetValueData(StructData) == FPropertyAccess::Success)
	{
		StructPtr = static_cast<FBTUResponseCurve*>(StructData);
	}

	// Skip Custom Curve because it already shows an editor curve
	if (StructPtr && StructPtr->ResponseCurve.GetScriptStruct() == FBTUResponseCurve_Custom::StaticStruct())
		return;

	// Inject Curve Preview Widget
	ChildBuilder.AddCustomRow(FText::FromString("Preview"))
	            .NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Preview")))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		[
			SNew(SBox)
			.Padding(FMargin(4.0f)) // Ajusta el margen a tu gusto
			[
				SAssignNew(PreviewWidget, SBTUResponseCurvePreviewWidget)
			]
		];

	// Assign Curve to the Preview Widget
	if (PreviewWidget.IsValid() && StructPtr)
	{
		PreviewWidget->ResponseCurveInstance = &StructPtr->ResponseCurve;
	}
}

TSharedRef<IPropertyTypeCustomization> FBTUResponseCurveDetails::MakeInstance()
{
	return MakeShareable(new FBTUResponseCurveDetails);
}
