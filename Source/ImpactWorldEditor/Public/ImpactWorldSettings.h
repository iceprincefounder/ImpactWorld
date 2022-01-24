// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Engine/EngineTypes.h"
#include "ImpactWorldSettings.generated.h"

/**
* Implements the settings for the ImpactWorld plugin.
*/
UCLASS(Config = Engine, DefaultConfig)
class UImpactWorldSettings : public UObject
{
    GENERATED_UCLASS_BODY()

    UPROPERTY(EditAnywhere, config, Category = ImpactFoliage)
    bool bEnableImpactFoliage;

	UPROPERTY(EditAnywhere, config, Category = ImpactFoliage)
	float TrailDampening;

	UPROPERTY(EditAnywhere, config, Category = ImpactFoliage)
	float TrailTravelSpeed;

	UPROPERTY(EditAnywhere, config, Category = ImpactFoliage)
	float FoliageNormalScale;

	UPROPERTY(EditAnywhere, config, Category = ImpactWater)
	bool bEnableImpactWater;

	UPROPERTY(EditAnywhere, config, Category = ImpactWater)
	float WaveDampening;

	UPROPERTY(EditAnywhere, config, Category = ImpactWater)
	float FoamDampening;

	UPROPERTY(EditAnywhere, config, Category = ImpactWater)
	float WaveTravelSpeed;

	UPROPERTY(EditAnywhere, config, Category = ImpactWater)
	float WaterNormalScale;
};