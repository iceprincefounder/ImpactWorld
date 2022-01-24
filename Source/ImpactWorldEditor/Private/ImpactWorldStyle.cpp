// Copyright Epic Games, Inc. All Rights Reserved.

#include "ImpactWorldStyle.h"
#include "ImpactWorldEditorModule.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FImpactWorldStyle::StyleInstance = NULL;

void FImpactWorldStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FImpactWorldStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FImpactWorldStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ImpactWorldStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon32x32(40.0f, 40.0f);
const FVector2D Icon128x128(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FImpactWorldStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ImpactWorldStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ImpactWorld")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ImpactWorld.PluginAction", new IMAGE_BRUSH(TEXT("Icon128.png"), Icon128x128));
	Style->Set("ClassIcon.ImpactWaterComponent", new IMAGE_BRUSH(TEXT("ImpactWaterIcon_16x"), Icon16x16));
	Style->Set("ClassThumbnail.ImpactWaterComponent", new IMAGE_BRUSH(TEXT("ImpactWaterIcon_32x"), Icon32x32));
	Style->Set("ClassIcon.ImpactFoliageComponent", new IMAGE_BRUSH(TEXT("ImpactFoliageIcon_16x"), Icon16x16));
	Style->Set("ClassThumbnail.ImpactFoliageComponent", new IMAGE_BRUSH(TEXT("ImpactFoliageIcon_32x"), Icon32x32));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FImpactWorldStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FImpactWorldStyle::Get()
{
	return *StyleInstance;
}
