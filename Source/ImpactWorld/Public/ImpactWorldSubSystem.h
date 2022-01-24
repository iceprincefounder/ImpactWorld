// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/EngineTypes.h"
#include "Interfaces/Interface_PostProcessVolume.h"
#include "GameFramework/Volume.h"
#include "ImpactWaterComponent.h"
#include "ImpactFoliageComponent.h"
#include "ImpactWorldSubSystem.generated.h"

struct FForceWaterSplatParameter
{
	TArray<FVector> HitLocationArray;
	TArray<float> RippleStrengthArray;
	TArray<float> RippleSizeInPercentArray;
	TArray<FVector2D> RippleSizeRangeArray;
	TArray<float> FoamStrengthArray;
	TArray<float> FoamSizeInPercentArray;
	TArray<FVector2D> FoamSizeRangeArray;

	int Num() const
	{
		return HitLocationArray.Num();
	}

	void Clear()
	{
        HitLocationArray.Empty();
        RippleStrengthArray.Empty();
		RippleSizeInPercentArray.Empty();
		RippleSizeRangeArray.Empty();
        FoamStrengthArray.Empty();
		FoamSizeInPercentArray.Empty();
		FoamSizeRangeArray.Empty();
	}
};

struct FForceFoliageSplatParameter
{
	TArray<FVector> HitLocationArray;
	TArray<float> ImpactStrengthArray;
	TArray<float> ImpactSizeInPercentArray;
	TArray<FVector2D> ImpactSizeRangeArray;

	int Num() const
	{
		return HitLocationArray.Num();
	}

	void Clear()
	{
		HitLocationArray.Empty();
		ImpactStrengthArray.Empty();
		ImpactSizeInPercentArray.Empty();
		ImpactSizeRangeArray.Empty();
	}
};

UCLASS(BlueprintType, Transient)
class IMPACTWORLD_API UImpactWorldSubSystem : public UWorldSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
	UImpactWorldSubSystem();

	//~ Begin UWorldSubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End UWorldSubsystem Interface

	//~ Begin FTickableGameObject Interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableInEditor() const { return true; };
	//~ End FTickableGameObject Interface

public:
	void AddForceWaterSplat(const FVector& HitLocation, const FWaterInteractParams& InParams);
	void AddForceFoliageSplat(const FVector& HitLocation, const FImpactFoliageParams& InParams);

	DECLARE_EVENT_TwoParams(UImpactWorldSubSystem, FOnAddForceWaterSplatEvent, const FVector&, const FWaterInteractParams&);
	FOnAddForceWaterSplatEvent& OnAddForceWaterSplatEventChanged()
	{
		return AddForceWaterSplatEvent;
	}

	DECLARE_EVENT_TwoParams(UImpactWorldSubSystem, FOnAddForceFoliageSplatEvent, const FVector&, const FImpactFoliageParams&);
	FOnAddForceFoliageSplatEvent& OnAddForceFoliageSplatEventChanged()
	{
		return AddForceFoliageSplatEvent;
	}

private:
	void GenerateWaterSplat(float InDeltaTime);
	void PropagateWaterSplat(float InDeltaTime);
	void ClearWaterSplatCanvas(float InDeltaTime);
	void GenerateFoliageSplat(float InDeltaTime);
	void PropagateFoliageSplat(float InDeltaTime);
	void ClearFoliageSplatCanvas(float InDeltaTime);
	FVector WorldPosToRelativeUV(FVector WorldPos);
	UTextureRenderTarget2D* GetWaterHeightRT(int Index);
	UTextureRenderTarget2D* GetImpactFoliageHeightRT(int Index);
	void ClearRenderTarget(UTextureRenderTarget2D* TextureRT);
	UTextureRenderTarget2D* CreateRenderTarget(UTextureRenderTarget2D* Parent);
private:
	UPROPERTY(Transient)
	UTextureRenderTarget2D* RT_ImpactWaterHeightMapA;
	UPROPERTY(Transient)
	UTextureRenderTarget2D* RT_ImpactWaterHeightMapB;
	UPROPERTY(Transient)
	UTextureRenderTarget2D* RT_ImpactWaterHeightMapC;
	UPROPERTY(Transient)
	UTextureRenderTarget2D* RT_ImpactWaterHeightMap;
	UPROPERTY(Transient)
	UTextureRenderTarget2D* RT_ImpactWaterNormalMap;
	UPROPERTY(Transient)
	UTextureRenderTarget2D* RT_ImpactFoliageHeightMapA;
	UPROPERTY(Transient)
	UTextureRenderTarget2D* RT_ImpactFoliageHeightMapB;
	UPROPERTY(Transient)
	UTextureRenderTarget2D* RT_ImpactFoliageHeightMapC;
	UPROPERTY(Transient)
	UTextureRenderTarget2D* RT_ImpactFoliageHeightMap;
	UPROPERTY(Transient)
	UTextureRenderTarget2D* RT_ImpactFoliageNormalMap;
	UPROPERTY(Transient)
	UTexture2D* HitStampTexture;
	UPROPERTY(Transient)
	UMaterialParameterCollection* MPC_InteractiveParms;

	int32 CurrentActiveWaterHeightMap;
	int32 CurrentActiveImpactFoliageHeightMap;
	bool bIsActiveGenerateWaterSplat;
	bool bIsActivePropagateWaterSplat;
	bool bIsActiveGenerateImpactFoliageSplat;
	bool bIsActivePropagateImpactFoliageSplat;
	float ActiveWaterLifeTime;
	float ActiveImpactFoliageLifeTime;
	float InteractiveDistance;
	FVector CharacterPosition;
	FForceWaterSplatParameter HitWaterForces;
	FForceFoliageSplatParameter ImpactFoliageForces;
	const float TexelSizeConstant = 0.00122f;

	FOnAddForceWaterSplatEvent AddForceWaterSplatEvent;
	FOnAddForceFoliageSplatEvent AddForceFoliageSplatEvent;
};