// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"

/**
 * @class FBTUResponseCurveDetails
 *	
 * Property customization for FUtilityCurveWrapper.
 * This class handles the visual representation of utility curves in the Unreal Editor.
 */
class FBTUResponseCurveDetails : public IPropertyTypeCustomization
{
public:

#pragma region IPropertyTypeCustomization
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
#pragma endregion
	
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

private:
	/** Reference to the Slate widget that draws the curve */
	TSharedPtr<class SBTUResponseCurvePreviewWidget> PreviewWidget;
};
