// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#include "ImpactWorldSubSystem.h"
#include "ImpactWaterGlobalShaders.h"
#include "ImpactFoliageGlobalShaders.h"
#include "UObject/ConstructorHelpers.h"
#include "ClearQuad.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"

static TAutoConsoleVariable<bool> CVarImpactFoliageUseConfig(TEXT("r.ImpactFoliageUseConfig"), true, TEXT("Enable Interactive Foliage Use Config"));
static TAutoConsoleVariable<float> CVarImpactFoliageTrailDampening(TEXT("r.ImpactFoliage.TrailDampening"), 0.85f, TEXT("TrailDampening"));
static TAutoConsoleVariable<float> CVarImpactFoliageTrailTravelSpeed(TEXT("r.ImpactFoliage.TravelSpeed"), 0.1f, TEXT("Dampening"));

static TAutoConsoleVariable<bool> CVarImpactWaterUseConfig(TEXT("r.ImpactWaterUseConfig"), true, TEXT("Enable Interactive Water Use Config"));
static TAutoConsoleVariable<float> CVarImpactWaterWaveDampening(TEXT("r.ImpactWater.WaveDampening"), 0.95f, TEXT("WaveDampening"));
static TAutoConsoleVariable<float> CVarImpactWaterFoamDampening(TEXT("r.ImpactWater.FoamDampening"), 0.95f, TEXT("FoamDampening"));
static TAutoConsoleVariable<float> CVarImpactWaterWaveTravelSpeed(TEXT("r.ImpactWater.WaveTravelSpeed"), 0.25f, TEXT("WaveTravelSpeed"));

UImpactWorldSubSystem::UImpactWorldSubSystem()
{
	InteractiveDistance = 3000.0f;
	CharacterPosition = FVector::ZeroVector;

	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> HeightMapFinder(TEXT("/ImpactWorld/Textures/RT_InteractiveWater_HeightMap"));
	if (HeightMapFinder.Succeeded())
	{
		RT_ImpactWaterHeightMap = HeightMapFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> NormalMapFinder(TEXT("/ImpactWorld/Textures/RT_InteractiveWater_NormalMap"));
	if (NormalMapFinder.Succeeded())
	{
		RT_ImpactWaterNormalMap = NormalMapFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> ImpactHeightFinder(TEXT("/ImpactWorld/Textures/RT_InteractiveFoliage_HeightMap"));
	if (ImpactHeightFinder.Succeeded())
	{
		RT_ImpactFoliageHeightMap = ImpactHeightFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> ImpactNormalFinder(TEXT("/ImpactWorld/Textures/RT_InteractiveFoliage_NormalMap"));
	if (ImpactNormalFinder.Succeeded())
	{
		RT_ImpactFoliageNormalMap = ImpactNormalFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> HitStampTextureFinder(TEXT("/ImpactWorld/Textures/T_CircleGradient"));
	if (HitStampTextureFinder.Succeeded())
	{
		HitStampTexture = HitStampTextureFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> InteractiveFinder(TEXT("/ImpactWorld/Materials/MaterialParameterCollections/MPC_InteractiveParms"));
	if (InteractiveFinder.Succeeded())
	{
		MPC_InteractiveParms = InteractiveFinder.Object;
	}

	AddForceWaterSplatEvent.AddUObject(this, &UImpactWorldSubSystem::AddForceWaterSplat);
	AddForceFoliageSplatEvent.AddUObject(this, &UImpactWorldSubSystem::AddForceFoliageSplat);
}

void UImpactWorldSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (RT_ImpactWaterHeightMap)
	{
		RT_ImpactWaterHeightMapA = CreateRenderTarget(RT_ImpactWaterHeightMap);
		RT_ImpactWaterHeightMapB = CreateRenderTarget(RT_ImpactWaterHeightMap);
		RT_ImpactWaterHeightMapC = CreateRenderTarget(RT_ImpactWaterHeightMap);
	}

	if (RT_ImpactFoliageHeightMap)
	{
		RT_ImpactFoliageHeightMapA = CreateRenderTarget(RT_ImpactFoliageHeightMap);
		RT_ImpactFoliageHeightMapB = CreateRenderTarget(RT_ImpactFoliageHeightMap);
		RT_ImpactFoliageHeightMapC = CreateRenderTarget(RT_ImpactFoliageHeightMap);
	}
}

void UImpactWorldSubSystem::Deinitialize()
{
	if (RT_ImpactWaterHeightMapA)
	{
		RT_ImpactWaterHeightMapA->MarkPendingKill();
	}
	if (RT_ImpactWaterHeightMapB)
	{
		RT_ImpactWaterHeightMapB->MarkPendingKill();
	}
	if (RT_ImpactWaterHeightMapC)
	{
		RT_ImpactWaterHeightMapC->MarkPendingKill();
	}

	if (RT_ImpactFoliageHeightMapA)
	{
		RT_ImpactFoliageHeightMapA->MarkPendingKill();
	}
	if (RT_ImpactFoliageHeightMapB)
	{
		RT_ImpactFoliageHeightMapB->MarkPendingKill();
	}
	if (RT_ImpactFoliageHeightMapC)
	{
		RT_ImpactFoliageHeightMapC->MarkPendingKill();
	}
}

void UImpactWorldSubSystem::Tick(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			if (APawn* Pawn = PlayerController->GetPawn())
			{
				CharacterPosition = Pawn->GetActorLocation();
			}
		}

		bool bEnableImpactFoliage = true; bool bEnableImpactWater = true;
		GConfig->GetBool(TEXT("/Script/ImpactWorldEditor.ImpactWorldSettings"), TEXT("bEnableImpactFoliage"), bEnableImpactFoliage, GEngineIni);
		GConfig->GetBool(TEXT("/Script/ImpactWorldEditor.ImpactWorldSettings"), TEXT("bEnableImpactWater"), bEnableImpactWater, GEngineIni);

		if (bEnableImpactFoliage)
		{
			if (bIsActiveGenerateImpactFoliageSplat)
			{
				GenerateFoliageSplat(DeltaTime);
			}
			if (bIsActivePropagateImpactFoliageSplat)
			{
				if (ActiveImpactFoliageLifeTime - DeltaTime > 0.0f)
				{
					PropagateFoliageSplat(DeltaTime);
				}
				else
				{
					ClearFoliageSplatCanvas(DeltaTime);
				}
			}
		}

		if (bEnableImpactWater)
		{
			if (bIsActiveGenerateWaterSplat)
			{
				GenerateWaterSplat(DeltaTime);
			}
			if (bIsActivePropagateWaterSplat)
			{
				if (ActiveWaterLifeTime - DeltaTime > 0.0f)
				{
					PropagateWaterSplat(DeltaTime);
				}
				else
				{
					ClearWaterSplatCanvas(DeltaTime);
				}
			}
		}

		UMaterialParameterCollectionInstance* MPC_Instance = World->GetParameterCollectionInstance(MPC_InteractiveParms);
		MPC_Instance->SetVectorParameterValue(FName(TEXT("PlayerPos")), FLinearColor(CharacterPosition));
		MPC_Instance->SetScalarParameterValue(FName(TEXT("InteractiveDistance")), InteractiveDistance);
	} // If World
}

TStatId UImpactWorldSubSystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UImpactWorldSubSystem, STATGROUP_Tickables);
}

void UImpactWorldSubSystem::AddForceWaterSplat(const FVector& HitLocation, const FWaterInteractParams& InParams)
{
	FVector WPVec = FVector(CharacterPosition.X, CharacterPosition.Y, 0);
	if (HitLocation.X > WPVec.X - (InteractiveDistance - 400) / 2 && HitLocation.X<WPVec.X + (InteractiveDistance - 400) / 2 && HitLocation.Y>WPVec.Y - (InteractiveDistance - 400) / 2 && HitLocation.Y < WPVec.Y + (InteractiveDistance - 400) / 2)
	{
		bIsActiveGenerateWaterSplat = true;
		bIsActivePropagateWaterSplat = true;
		ActiveWaterLifeTime = 4.0f;

		HitWaterForces.HitLocationArray.Add(WorldPosToRelativeUV(HitLocation));
		HitWaterForces.RippleStrengthArray.Add(InParams.RippleStrength);
		HitWaterForces.RippleSizeInPercentArray.Add(InParams.RippleSizeInPercent);
		HitWaterForces.RippleSizeRangeArray.Add(InParams.RippleSizeRange);
		HitWaterForces.FoamStrengthArray.Add(InParams.FoamStrength);
		HitWaterForces.FoamSizeInPercentArray.Add(InParams.FoamSizeInPercent);
		HitWaterForces.FoamSizeRangeArray.Add(InParams.FoamSizeRange);
	}
}

void UImpactWorldSubSystem::AddForceFoliageSplat(const FVector& HitLocation, const FImpactFoliageParams& InParams)
{
	FVector WPVec = FVector(CharacterPosition.X, CharacterPosition.Y, 0);
	if (HitLocation.X > WPVec.X - (InteractiveDistance - 400) / 2 && HitLocation.X<WPVec.X + (InteractiveDistance - 400) / 2 && HitLocation.Y>WPVec.Y - (InteractiveDistance - 400) / 2 && HitLocation.Y < WPVec.Y + (InteractiveDistance - 400) / 2)
	{
		bIsActiveGenerateImpactFoliageSplat = true;
		bIsActivePropagateImpactFoliageSplat = true;
		ActiveImpactFoliageLifeTime = 4.0f;

		ImpactFoliageForces.HitLocationArray.Add(WorldPosToRelativeUV(HitLocation));
		ImpactFoliageForces.ImpactStrengthArray.Add(InParams.ImpactStrength);
		ImpactFoliageForces.ImpactSizeInPercentArray.Add(InParams.ImpactSizeInPercent);
		ImpactFoliageForces.ImpactSizeRangeArray.Add(InParams.ImpactSizeRange);
	}
}

void UImpactWorldSubSystem::GenerateWaterSplat(float InDeltaTime)
{
	bIsActiveGenerateWaterSplat = false;

	UTextureRenderTarget2D* CurrentHeightRT = GetWaterHeightRT(CurrentActiveWaterHeightMap);
    const FForceWaterSplatParameter ForceSplatParameter = HitWaterForces;
	ENQUEUE_RENDER_COMMAND(ImpactWorldGenerateSplat)(
		[this, CurrentHeightRT, ForceSplatParameter](FRHICommandListImmediate& RHICmdList)
		{
			SCOPED_DRAW_EVENT(RHICmdList, SubWorldTick);
			SCOPED_NAMED_EVENT(ImpactWorldGenerateSplat_RenderThread, FColor::Blue);
			QUICK_SCOPE_CYCLE_COUNTER(ImpactWorldGenerateSplat_RenderThread);

            FGenerateWaterSplatUniformParameters EmptyGenerateSplatUniformParameters;
            {
                EmptyGenerateSplatUniformParameters.HitStampTexture = HitStampTexture->Resource->TextureRHI;
                EmptyGenerateSplatUniformParameters.HitStampTextureSampler = TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap>::GetRHI();
                EmptyGenerateSplatUniformParameters.CanvasTexture = RT_ImpactWaterHeightMap->Resource->TextureRHI;
                EmptyGenerateSplatUniformParameters.CanvasTextureSampler = TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap>::GetRHI();
                EmptyGenerateSplatUniformParameters.TexelSizeScalar = TexelSizeConstant;
                EmptyGenerateSplatUniformParameters.HitLocationNum = ForceSplatParameter.Num();
                for (int32 i = 0; i < MAX_HITLOCATION_SIZE; i++)
                {
                    if (i < ForceSplatParameter.Num())
                    {
                        EmptyGenerateSplatUniformParameters.HitParam0Array[i].X = ForceSplatParameter.RippleStrengthArray[i];
                        EmptyGenerateSplatUniformParameters.HitParam0Array[i].Y = ForceSplatParameter.RippleSizeInPercentArray[i];
                        EmptyGenerateSplatUniformParameters.HitParam0Array[i].Z = ForceSplatParameter.RippleSizeRangeArray[i].X;
                        EmptyGenerateSplatUniformParameters.HitParam0Array[i].W = ForceSplatParameter.RippleSizeRangeArray[i].Y;
                        EmptyGenerateSplatUniformParameters.HitParam1Array[i].X = ForceSplatParameter.FoamStrengthArray[i];
                        EmptyGenerateSplatUniformParameters.HitParam1Array[i].Y = ForceSplatParameter.FoamSizeInPercentArray[i];
                        EmptyGenerateSplatUniformParameters.HitParam1Array[i].Z = ForceSplatParameter.FoamSizeRangeArray[i].X;
                        EmptyGenerateSplatUniformParameters.HitParam1Array[i].W = ForceSplatParameter.FoamSizeRangeArray[i].Y;
                        EmptyGenerateSplatUniformParameters.HitLocationArray[i] = ForceSplatParameter.HitLocationArray[i];
                    }
                }
            }
        
			FGenerateWaterSplatUniformBufferRef GenerateSplatUniformBuffer = FGenerateWaterSplatUniformBufferRef::CreateUniformBufferImmediate(EmptyGenerateSplatUniformParameters, UniformBuffer_SingleFrame);
			FTextureRenderTargetResource* CurHeightRTResource = CurrentHeightRT->GetRenderTargetResource();
			FImpactWaterGlobalShaderCommands::GenerateSplatToRenderTarget(RHICmdList, CurHeightRTResource, GenerateSplatUniformBuffer);
		});

	HitWaterForces.Clear();
}

void UImpactWorldSubSystem::PropagateWaterSplat(float InDeltaTime)
{
	ActiveWaterLifeTime -= InDeltaTime;

	CurrentActiveWaterHeightMap = (1 + CurrentActiveWaterHeightMap) % 3;
	UTextureRenderTarget2D* Prev_RT;
	UTextureRenderTarget2D* Next_RT;
	if (GetWaterHeightRT((CurrentActiveWaterHeightMap + 2) % 3))
	{
		Prev_RT = GetWaterHeightRT((CurrentActiveWaterHeightMap + 2) % 3);
	}
	if (GetWaterHeightRT((CurrentActiveWaterHeightMap + 1) % 3))
	{
		Next_RT = GetWaterHeightRT((CurrentActiveWaterHeightMap + 1) % 3);
	}
	if (GetWaterHeightRT(CurrentActiveWaterHeightMap) != nullptr)
	{
		UTextureRenderTarget2D* Curr_RT = GetWaterHeightRT(CurrentActiveWaterHeightMap);
		
		ENQUEUE_RENDER_COMMAND(ImpactWorldPropagateSplat)(
			[this, Curr_RT, Prev_RT, Next_RT](FRHICommandListImmediate& RHICmdList)
			{
				SCOPED_DRAW_EVENT(RHICmdList, SubWorldTick);
				SCOPED_NAMED_EVENT(ImpactWorldPropagateSplat_RenderThread, FColor::Blue);
				QUICK_SCOPE_CYCLE_COUNTER(ImpactWorldPropagateSplat_RenderThread);
				FTextureRenderTargetResource* NormalMap_RTResource = RT_ImpactWaterNormalMap->GetRenderTargetResource();
				FTextureRenderTargetResource* HeightMap_RTResource = RT_ImpactWaterHeightMap->GetRenderTargetResource();

				// PropagateWaterSplat Command
				FTextureRHIRef PrevA_RT_RHIRef = Prev_RT->GetRenderTargetResource()->TextureRHI;
				FTextureRHIRef PrevB_RT_RHIRef = Next_RT->GetRenderTargetResource()->TextureRHI;
				float WaveDampening = CVarImpactWaterWaveDampening.GetValueOnAnyThread(), FoamDampening = CVarImpactWaterFoamDampening.GetValueOnAnyThread(), WaveTravelSpeed = CVarImpactWaterWaveTravelSpeed.GetValueOnAnyThread();
				if (CVarImpactWaterUseConfig.GetValueOnAnyThread())
				{
					GConfig->GetFloat(TEXT("/Script/ImpactWorldEditor.ImpactWorldSettings"), TEXT("WaveDampening"), WaveDampening, GEngineIni);
					GConfig->GetFloat(TEXT("/Script/ImpactWorldEditor.ImpactWorldSettings"), TEXT("FoamDampening"), FoamDampening, GEngineIni);
					GConfig->GetFloat(TEXT("/Script/ImpactWorldEditor.ImpactWorldSettings"), TEXT("WaveTravelSpeed"), WaveTravelSpeed, GEngineIni);
				}
				FImpactWaterGlobalShaderCommands::PropagateSplatToRenderTarget(RHICmdList, Curr_RT->GetRenderTargetResource(), PrevA_RT_RHIRef, PrevB_RT_RHIRef,
					WaveDampening, FoamDampening, WaveTravelSpeed, this->TexelSizeConstant);

				// CopyRenderTarget Command
				RHICmdList.CopyToResolveTarget(Curr_RT->GetRenderTargetResource()->GetRenderTargetTexture(), RT_ImpactWaterHeightMap->GetRenderTargetResource()->GetRenderTargetTexture(), FResolveParams());
				// ComputeNormal Command
				float HeightScale = 0.01f;
				GConfig->GetFloat(TEXT("/Script/ImpactWorldEditor.ImpactWorldSettings"), TEXT("WaterNormalScale"), HeightScale, GEngineIni);
				FImpactWaterGlobalShaderCommands::ComputeNormalToRenderTarget(RHICmdList, NormalMap_RTResource, HeightMap_RTResource->TextureRHI, HeightScale, TexelSizeConstant);
			});
	}
}

void UImpactWorldSubSystem::ClearWaterSplatCanvas(float InDeltaTime)
{
	bIsActivePropagateWaterSplat = false;

	if (RT_ImpactWaterHeightMapA && RT_ImpactWaterHeightMapB && RT_ImpactWaterHeightMapC && RT_ImpactWaterHeightMap && RT_ImpactWaterNormalMap)
	{
		ClearRenderTarget(RT_ImpactWaterHeightMapA);
		ClearRenderTarget(RT_ImpactWaterHeightMapB);
		ClearRenderTarget(RT_ImpactWaterHeightMapC);
		ClearRenderTarget(RT_ImpactWaterHeightMap);
		ClearRenderTarget(RT_ImpactWaterNormalMap);
	}
}

void UImpactWorldSubSystem::GenerateFoliageSplat(float InDeltaTime)
{
	bIsActiveGenerateImpactFoliageSplat = false;

	UTextureRenderTarget2D* CurrentHeightRT = GetImpactFoliageHeightRT(CurrentActiveImpactFoliageHeightMap);

	//UTextureRenderTarget2D* ImpactHeightRT = RT_ImpactHeight;
	const FForceFoliageSplatParameter ForceSplatParameter = ImpactFoliageForces;
	ENQUEUE_RENDER_COMMAND(ACMImpactFoliageGenerateSplat)(
		[this, CurrentHeightRT, ForceSplatParameter](FRHICommandListImmediate& RHICmdList)
		{
			SCOPED_DRAW_EVENT(RHICmdList, SubWorldTick);
			SCOPED_NAMED_EVENT(ACMImpactFoliageGenerateSplat_RenderThread, FColor::Blue);
			QUICK_SCOPE_CYCLE_COUNTER(ACMImpactFoliageGenerateSplat_RenderThread);

			FGenerateFoliageSplatUniformParameters EmptyGenerateSplatUniformParameters;
			{
				EmptyGenerateSplatUniformParameters.HitStampTexture = HitStampTexture->Resource->TextureRHI;
				EmptyGenerateSplatUniformParameters.HitStampTextureSampler = TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap>::GetRHI();
				EmptyGenerateSplatUniformParameters.CanvasTexture = RT_ImpactFoliageHeightMap->Resource->TextureRHI;
				EmptyGenerateSplatUniformParameters.CanvasTextureSampler = TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap>::GetRHI();
				EmptyGenerateSplatUniformParameters.TexelSizeScalar = TexelSizeConstant;
				EmptyGenerateSplatUniformParameters.HitLocationNum = ForceSplatParameter.Num();
				for (int32 i = 0; i < MAX_HITLOCATION_SIZE; i++)
				{
					if (i < ForceSplatParameter.Num())
					{
						EmptyGenerateSplatUniformParameters.HitParamArray[i].X = ForceSplatParameter.ImpactStrengthArray[i];
						EmptyGenerateSplatUniformParameters.HitParamArray[i].Y = ForceSplatParameter.ImpactSizeInPercentArray[i];
						EmptyGenerateSplatUniformParameters.HitParamArray[i].Z = ForceSplatParameter.ImpactSizeRangeArray[i].X;
						EmptyGenerateSplatUniformParameters.HitParamArray[i].W = ForceSplatParameter.ImpactSizeRangeArray[i].Y;
						EmptyGenerateSplatUniformParameters.HitLocationArray[i] = ForceSplatParameter.HitLocationArray[i];
					}
				}
			}

			FGenerateFoliageSplatUniformBufferRef GenerateSplatUniformBuffer = FGenerateFoliageSplatUniformBufferRef::CreateUniformBufferImmediate(EmptyGenerateSplatUniformParameters, UniformBuffer_SingleFrame);
			FTextureRenderTargetResource* HeightRTResource = CurrentHeightRT->GetRenderTargetResource();
			FImpactFoliageGlobalShaderCommands::GenerateSplatToRenderTarget(RHICmdList, HeightRTResource, GenerateSplatUniformBuffer);
		});

	ImpactFoliageForces.Clear();
}

void UImpactWorldSubSystem::PropagateFoliageSplat(float InDeltaTime)
{
	ActiveImpactFoliageLifeTime -= InDeltaTime;

	CurrentActiveImpactFoliageHeightMap = (1 + CurrentActiveImpactFoliageHeightMap) % 3;
	UTextureRenderTarget2D* Prev_RT;
	UTextureRenderTarget2D* Next_RT;
	if (GetImpactFoliageHeightRT((CurrentActiveImpactFoliageHeightMap + 2) % 3))
	{
		Prev_RT = GetImpactFoliageHeightRT((CurrentActiveImpactFoliageHeightMap + 2) % 3);
	}
	if (GetImpactFoliageHeightRT((CurrentActiveImpactFoliageHeightMap + 1) % 3))
	{
		Next_RT = GetImpactFoliageHeightRT((CurrentActiveImpactFoliageHeightMap + 1) % 3);
	}
	if (GetImpactFoliageHeightRT(CurrentActiveImpactFoliageHeightMap) != nullptr)
	{
		UTextureRenderTarget2D* Curr_RT = GetImpactFoliageHeightRT(CurrentActiveImpactFoliageHeightMap);

		ENQUEUE_RENDER_COMMAND(ACMImpactFoliagePropagateSplat)(
			[this, Curr_RT, Prev_RT, Next_RT](FRHICommandListImmediate& RHICmdList)
			{
				SCOPED_DRAW_EVENT(RHICmdList, SubWorldTick);
				SCOPED_NAMED_EVENT(ACMImpactFoliagePropagateSplat_RenderThread, FColor::Blue);
				QUICK_SCOPE_CYCLE_COUNTER(ACMImpactFoliagePropagateSplat_RenderThread);
				FTextureRenderTargetResource* NormalMap_RTResource = RT_ImpactFoliageNormalMap->GetRenderTargetResource();
				FTextureRenderTargetResource* HeightMap_RTResource = RT_ImpactFoliageHeightMap->GetRenderTargetResource();
				FTextureRHIRef PrevA_RT_RHIRef = Prev_RT->GetRenderTargetResource()->TextureRHI;
				FTextureRHIRef PrevB_RT_RHIRef = Next_RT->GetRenderTargetResource()->TextureRHI;
				// PropagateImpactFoliageSplat Command
				float TrailDampening = CVarImpactFoliageTrailDampening.GetValueOnAnyThread(), TrailTravelSpeed = CVarImpactFoliageTrailTravelSpeed.GetValueOnAnyThread();
				if (CVarImpactFoliageUseConfig.GetValueOnAnyThread())
				{
					GConfig->GetFloat(TEXT("/Script/ImpactWorldEditor.ImpactWorldSettings"), TEXT("TrailDampening"), TrailDampening, GEngineIni);
					GConfig->GetFloat(TEXT("/Script/ImpactWorldEditor.ImpactWorldSettings"), TEXT("TrailTravelSpeed"), TrailTravelSpeed, GEngineIni);
				}
				FImpactFoliageGlobalShaderCommands::PropagateSplatToRenderTarget(RHICmdList, Curr_RT->GetRenderTargetResource(), PrevA_RT_RHIRef, PrevB_RT_RHIRef,
					TrailDampening, TrailTravelSpeed, TexelSizeConstant);
				// CopyRenderTarget Command
				RHICmdList.CopyToResolveTarget(Curr_RT->GetRenderTargetResource()->GetRenderTargetTexture(), RT_ImpactFoliageHeightMap->GetRenderTargetResource()->GetRenderTargetTexture(), FResolveParams());
				// ComputeNormal Command
				float HeightScale = 0.01f;
				GConfig->GetFloat(TEXT("/Script/ImpactWorldEditor.ImpactWorldSettings"), TEXT("FoliageNormalScale"), HeightScale, GEngineIni);
				FImpactFoliageGlobalShaderCommands::ComputeNormalToRenderTarget(RHICmdList, NormalMap_RTResource, HeightMap_RTResource->TextureRHI, HeightScale, TexelSizeConstant);
			});
	}
}

void UImpactWorldSubSystem::ClearFoliageSplatCanvas(float InDeltaTime)
{
	bIsActivePropagateImpactFoliageSplat = false;

	if (RT_ImpactFoliageHeightMap && RT_ImpactFoliageNormalMap)
	{
		ClearRenderTarget(RT_ImpactFoliageHeightMapA);
		ClearRenderTarget(RT_ImpactFoliageHeightMapB);
		ClearRenderTarget(RT_ImpactFoliageHeightMapC);
		ClearRenderTarget(RT_ImpactFoliageHeightMap);
		ClearRenderTarget(RT_ImpactFoliageNormalMap);
	}
}

FVector UImpactWorldSubSystem::WorldPosToRelativeUV(FVector WorldPos)
{
	auto GenericPercent_FloatFloat = [](float A, float B)
	{
		return (B != 0.f) ? FMath::Fmod(A, B) : 0.f;
	};

	float X = (GenericPercent_FloatFloat(WorldPos.X + (InteractiveDistance / 2), InteractiveDistance)) / InteractiveDistance;
	float Y = (GenericPercent_FloatFloat(WorldPos.Y + (InteractiveDistance / 2), InteractiveDistance)) / InteractiveDistance;
	return FVector(X, Y, 0);
}

UTextureRenderTarget2D* UImpactWorldSubSystem::GetWaterHeightRT(int Index)
{
	if (Index == 0)
	{
		return RT_ImpactWaterHeightMapA;
	}
	else if (Index == 1)
	{
		return RT_ImpactWaterHeightMapB;
	}
	else
	{
		return RT_ImpactWaterHeightMapC;
	}
}

UTextureRenderTarget2D* UImpactWorldSubSystem::GetImpactFoliageHeightRT(int Index)
{
	if (Index == 0)
	{
		return RT_ImpactFoliageHeightMapA;
	}
	else if (Index == 1)
	{
		return RT_ImpactFoliageHeightMapB;
	}
	else
	{
		return RT_ImpactFoliageHeightMapC;
	}
}

void UImpactWorldSubSystem::ClearRenderTarget(UTextureRenderTarget2D* TextureRT)
{
	ENQUEUE_RENDER_COMMAND(DrawClearRenderTarget)(
		[TextureRT](FRHICommandList& RHICmdList)
		{
			// Use hardware fast clear if the clear colors match.
			const FTexture2DRHIRef& TextureRHI = TextureRT->GetRenderTargetResource()->GetRenderTargetTexture();
			const bool bFastClear = TextureRHI->GetClearColor() == FLinearColor(0.0, 0.0, 1.0, 1.0);
			FLinearColor ClearColor = bFastClear ? FLinearColor::Black : FLinearColor(0.0, 0.0, 1.0, 1.0);

			FRHIRenderPassInfo RPInfo(TextureRHI, bFastClear ? ERenderTargetActions::Clear_Store : ERenderTargetActions::DontLoad_Store);
			TransitionRenderPassTargets(RHICmdList, RPInfo);
			RHICmdList.BeginRenderPass(RPInfo, TEXT("ClearRT"));
			if (!bFastClear)
			{
				DrawClearQuad(RHICmdList, ClearColor);
			}
			RHICmdList.EndRenderPass();
		});
}

UTextureRenderTarget2D* UImpactWorldSubSystem::CreateRenderTarget(UTextureRenderTarget2D* Parent)
{
	UTextureRenderTarget2D* NewRenderTarget2D = NewObject<UTextureRenderTarget2D>(GetWorld());
	check(NewRenderTarget2D);
	NewRenderTarget2D->RenderTargetFormat = Parent->RenderTargetFormat;
	NewRenderTarget2D->ClearColor = FLinearColor(0.0, 0.0, 0.0, 0.0);
	NewRenderTarget2D->bAutoGenerateMips = false;
	NewRenderTarget2D->InitAutoFormat(Parent->SizeX, Parent->SizeY);
	NewRenderTarget2D->UpdateResourceImmediate(true);
	return NewRenderTarget2D;
}
