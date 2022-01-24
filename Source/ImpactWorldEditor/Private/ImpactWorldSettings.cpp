// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#include "ImpactWorldSettings.h"

//////////////////////////////////////////////////////////////////////////
// UImpactWorldSettings

UImpactWorldSettings::UImpactWorldSettings(const FObjectInitializer& ObjectInitializer)
        : Super(ObjectInitializer)
        , bEnableImpactFoliage(true)
        , TrailDampening(0.85f)
		, TrailTravelSpeed(0.1f)
		, FoliageNormalScale(0.01f)
		, bEnableImpactWater(true)
		, WaveDampening(0.95f)
		, FoamDampening(0.95f)
		, WaveTravelSpeed(0.25f)
		, WaterNormalScale(0.01f)
{
}