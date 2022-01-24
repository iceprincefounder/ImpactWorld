// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#include "ImpactWorldModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FImpactPointModule"

void FImpactWorldModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("ImpactWorld"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/ImpactWorld"), PluginShaderDir);
}

void FImpactWorldModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
    // unregister settings
}

IMPLEMENT_MODULE(FImpactWorldModule, ImpactWorld)

#undef LOCTEXT_NAMESPACE
