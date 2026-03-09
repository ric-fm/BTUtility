#include "BTUtilityEditor.h"

#include "Core/BTUUtilityResponseCurves.h"
#include "DetailsCustomization/BTUResponseCurveDetails.h"

#define LOCTEXT_NAMESPACE "FBTUtilityEditorModule"

void FBTUtilityEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	// Bind FBTUResponseCurve to FBTUResponseCurveDetails
	PropertyModule.RegisterCustomPropertyTypeLayout(FBTUResponseCurve::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FBTUResponseCurveDetails::MakeInstance));
}

void FBTUtilityEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		
		PropertyModule.UnregisterCustomPropertyTypeLayout(FBTUResponseCurve::StaticStruct()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FBTUtilityEditorModule, BTUtilityEditor)