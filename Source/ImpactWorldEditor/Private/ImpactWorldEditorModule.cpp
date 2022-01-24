// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#include "ImpactWorldEditorModule.h"
#include "ImpactWorldStyle.h"
#include "ImpactWorldSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FImpactWorldEditorModule"

void FImpactWorldEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    FImpactWorldStyle::Initialize();
    FImpactWorldStyle::ReloadTextures();

	// register settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "ImpactWorld",
			LOCTEXT("ImpactWorldSettingsName", "ImpactWorld"),
			LOCTEXT("ImpactWorldSettingsDescription", "Project global settings for ImpactWorld plugin"),
			GetMutableDefault<UImpactWorldSettings>()
		);
	}
}

void FImpactWorldEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
    // unregister settings
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

    if (SettingsModule != nullptr)
    {
            SettingsModule->UnregisterSettings("Project", "Plugins", "ImpactWorld");
    }
    
    FImpactWorldStyle::Shutdown();
}

IMPLEMENT_MODULE(FImpactWorldEditorModule, ImpactWorldEditor)

#undef LOCTEXT_NAMESPACE
