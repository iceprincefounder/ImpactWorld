// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#pragma once

#include "ImpactWaterGlobalShaders.h"
#include "ShaderParameters.h"
#include "Shader.h"
#include "StaticBoundShaderState.h"
#include "SceneUtils.h"
#include "RHIStaticStates.h"
#include "PostProcess/SceneRenderTargets.h"
#include "GlobalShader.h"
#include "SceneRendering.h"
#include "PostProcess/SceneFilterRendering.h"
#include "PipelineStateCache.h"
#include "ClearQuad.h"
#include "CommonRenderResources.h"
#include "GenerateMips.h"

class FImpactWaterGenerateSplatPS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactWaterGenerateSplatPS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactWaterGenerateSplatPS() {}

public:
	/** Initialization constructor. */
	FImpactWaterGenerateSplatPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}

	void SetUniformBuffer(FRHICommandList& RHICmdList, FGenerateWaterSplatUniformBufferRef UniformBuffer)
	{
		FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();
		SetUniformBufferParameter(RHICmdList, ShaderRHI, GetUniformBufferParameter<FGenerateWaterSplatUniformParameters>(), UniformBuffer);
	}
};

class FImpactWaterGenerateSplatVS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactWaterGenerateSplatVS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactWaterGenerateSplatVS() {}

public:

	/** Initialization constructor. */
	FImpactWaterGenerateSplatVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}
};

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FGenerateWaterSplatUniformParameters, "GenerateWaterSplatUniforms");
IMPLEMENT_SHADER_TYPE(, FImpactWaterGenerateSplatPS, TEXT("/Plugin/ImpactWorld/Private/ImpactWaterGenerateSplat.usf"), TEXT("MainPS"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(, FImpactWaterGenerateSplatVS, TEXT("/Plugin/ImpactWorld/Private/ImpactWaterGenerateSplat.usf"), TEXT("MainVS"), SF_Vertex);

class FImpactWaterPropagateSplatPS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactWaterPropagateSplatPS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactWaterPropagateSplatPS() {}

public:
	LAYOUT_FIELD(FShaderResourceParameter, PreviewATextureParameter);
	LAYOUT_FIELD(FShaderResourceParameter, PreviewATextureSamplerParameter);
	LAYOUT_FIELD(FShaderResourceParameter, PreviewBTextureParameter);
	LAYOUT_FIELD(FShaderResourceParameter, PreviewBTextureSamplerParameter);
	LAYOUT_FIELD(FShaderParameter, WaveDampeningScalarParameter);
	LAYOUT_FIELD(FShaderParameter, FoamDampeningScalarParameter);
	LAYOUT_FIELD(FShaderParameter, WaveTravelSpeedScalarParameter);
	LAYOUT_FIELD(FShaderParameter, TexelSizeScalarParameter);

	/** Initialization constructor. */
	FImpactWaterPropagateSplatPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		PreviewATextureParameter.Bind(Initializer.ParameterMap, TEXT("PreviewATexture"));
		PreviewATextureSamplerParameter.Bind(Initializer.ParameterMap, TEXT("PreviewATextureSampler"));
		PreviewBTextureParameter.Bind(Initializer.ParameterMap, TEXT("PreviewBTexture"));
		PreviewBTextureSamplerParameter.Bind(Initializer.ParameterMap, TEXT("PreviewBTextureSampler"));
		WaveDampeningScalarParameter.Bind(Initializer.ParameterMap, TEXT("WaveDampeningScalar"));
		FoamDampeningScalarParameter.Bind(Initializer.ParameterMap, TEXT("FoamDampeningScalar"));
		WaveTravelSpeedScalarParameter.Bind(Initializer.ParameterMap, TEXT("WaveTravelSpeedScalar"));
		TexelSizeScalarParameter.Bind(Initializer.ParameterMap, TEXT("TexelSizeScalar"));
	}
};

class FImpactWaterPropagateSplatVS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactWaterPropagateSplatVS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactWaterPropagateSplatVS() {}

public:
	/** Initialization constructor. */
	FImpactWaterPropagateSplatVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}
};

IMPLEMENT_SHADER_TYPE(, FImpactWaterPropagateSplatPS, TEXT("/Plugin/ImpactWorld/Private/ImpactWaterPropagateSplat.usf"), TEXT("MainPS"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(, FImpactWaterPropagateSplatVS, TEXT("/Plugin/ImpactWorld/Private/ImpactWaterPropagateSplat.usf"), TEXT("MainVS"), SF_Vertex);

class FImpactWaterCreateNormalPS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactWaterCreateNormalPS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactWaterCreateNormalPS() {}

public:
	LAYOUT_FIELD(FShaderResourceParameter, HeightTextureParameter);
	LAYOUT_FIELD(FShaderResourceParameter, HeightTextureSamplerParameter);
	LAYOUT_FIELD(FShaderParameter, HeightScaleScalarParameter);
	LAYOUT_FIELD(FShaderParameter, TexelSizeScalarParameter);
	
	/** Initialization constructor. */
	FImpactWaterCreateNormalPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		HeightTextureParameter.Bind(Initializer.ParameterMap, TEXT("HeightTexture"));
		HeightTextureSamplerParameter.Bind(Initializer.ParameterMap, TEXT("HeightTextureSampler"));
		HeightScaleScalarParameter.Bind(Initializer.ParameterMap, TEXT("HeightScaleScalar"));
		TexelSizeScalarParameter.Bind(Initializer.ParameterMap, TEXT("TexelSizeScalar"));
	}
};

class FImpactWaterCreateNormalVS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactWaterCreateNormalVS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactWaterCreateNormalVS() {}

public:
	/** Initialization constructor. */
	FImpactWaterCreateNormalVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}
};

IMPLEMENT_SHADER_TYPE(, FImpactWaterCreateNormalPS, TEXT("/Plugin/ImpactWorld/Private/ImpactWaterComputeNormal.usf"), TEXT("MainPS"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(, FImpactWaterCreateNormalVS, TEXT("/Plugin/ImpactWorld/Private/ImpactWaterComputeNormal.usf"), TEXT("MainVS"), SF_Vertex);

void FImpactWaterGlobalShaderCommands::GenerateSplatToRenderTarget(FRHICommandList& RHICmdList, FTextureRenderTargetResource* RenderTargetResource, const FGenerateWaterSplatUniformBufferRef& UniformBuffer)
{
	FSceneViewFamily ViewFamily(FSceneViewFamily::ConstructionValues(
		RenderTargetResource,
		NULL,
		FEngineShowFlags(ESFIM_Game))
		.SetWorldTimes(0, 0, 0)
		.SetGammaCorrection(RenderTargetResource->GetDisplayGamma()));

	// Set the view family's render target/viewport.
	FRHIRenderPassInfo RPInfo(ViewFamily.RenderTarget->GetRenderTargetTexture(), ERenderTargetActions::DontLoad_Store);

	// Deferred the clear until here so the garbage left in the non rendered regions by the post process effects do not show up
	if (ViewFamily.bDeferClear)
	{
		if (ensure(ViewFamily.RenderTarget->GetRenderTargetTexture()->GetClearColor() == FLinearColor::Black))
		{
			RPInfo.ColorRenderTargets[0].Action = ERenderTargetActions::Clear_Store;
			RHICmdList.BeginRenderPass(RPInfo, TEXT("GenerateSplatToRenderTarget"));
		}
		else
		{
			RHICmdList.BeginRenderPass(RPInfo, TEXT("GenerateSplatToRenderTarget"));
			DrawClearQuad(RHICmdList, FLinearColor::Black);
		}
		ViewFamily.bDeferClear = false;
	}
	else
	{
		RHICmdList.BeginRenderPass(RPInfo, TEXT("GenerateSplatToRenderTarget"));
	}

	DECLARE_GPU_STAT(GenerateSplatToRenderTarget);
	SCOPED_DRAW_EVENT(RHICmdList, GenerateSplatToRenderTarget);
	SCOPED_GPU_STAT(RHICmdList, GenerateSplatToRenderTarget);

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

	// turn off culling and blending
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();

	// turn off depth reads/writes
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

	TShaderMapRef<FImpactWaterGenerateSplatVS> VertexShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	TShaderMapRef<FImpactWaterGenerateSplatPS> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;

	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	PixelShader->SetUniformBuffer(RHICmdList, UniformBuffer);

	// Draw a quad mapping scene color to the view's render target
	DrawRectangle(
		RHICmdList,
		0, 0, // Dest X, Y
		RenderTargetResource->GetSizeX(), RenderTargetResource->GetSizeY(), // Dest Width, Height
		0, 0, // Source U, V
		RenderTargetResource->GetSizeX(), RenderTargetResource->GetSizeX(), // Source USize, VSize
		RenderTargetResource->GetSizeXY(), // Target buffer size
		RenderTargetResource->GetSizeXY(), // Source texture size
		VertexShader,
		EDRF_UseTriangleOptimization);

	RHICmdList.EndRenderPass();
}

void FImpactWaterGlobalShaderCommands::PropagateSplatToRenderTarget(FRHICommandList& RHICmdList, FTextureRenderTargetResource* RenderTargetResource,
	const FTextureRHIRef& PreviewATexture, const FTextureRHIRef& PreviewBTexture, float WaveDampening, float FoamDampening, float WaveTravelSpeed, float TexelSize)
{
	FSceneViewFamily ViewFamily(FSceneViewFamily::ConstructionValues(
		RenderTargetResource,
		NULL,
		FEngineShowFlags(ESFIM_Game))
		.SetWorldTimes(0, 0, 0)
		.SetGammaCorrection(RenderTargetResource->GetDisplayGamma()));

	// Set the view family's render target/viewport.
	FRHIRenderPassInfo RPInfo(ViewFamily.RenderTarget->GetRenderTargetTexture(), ERenderTargetActions::DontLoad_Store);

	// Deferred the clear until here so the garbage left in the non rendered regions by the post process effects do not show up
	if (ViewFamily.bDeferClear)
	{
		if (ensure(ViewFamily.RenderTarget->GetRenderTargetTexture()->GetClearColor() == FLinearColor::Black))
		{
			RPInfo.ColorRenderTargets[0].Action = ERenderTargetActions::Clear_Store;
			RHICmdList.BeginRenderPass(RPInfo, TEXT("PropagateSplatToRenderTarget"));
		}
		else
		{
			RHICmdList.BeginRenderPass(RPInfo, TEXT("PropagateSplatToRenderTarget"));
			DrawClearQuad(RHICmdList, FLinearColor::Black);
		}
		ViewFamily.bDeferClear = false;
	}
	else
	{
	}
	RHICmdList.BeginRenderPass(RPInfo, TEXT("PropagateSplatToRenderTarget"));

	DECLARE_GPU_STAT(PropagateSplatToRenderTarget);
	SCOPED_DRAW_EVENT(RHICmdList, PropagateSplatToRenderTarget);
	SCOPED_GPU_STAT(RHICmdList, PropagateSplatToRenderTarget);

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

	// turn off culling and blending
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();

	// turn off depth reads/writes
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

	TShaderMapRef<FImpactWaterPropagateSplatVS> VertexShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	TShaderMapRef<FImpactWaterPropagateSplatPS> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;

	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	FRHIPixelShader* ShaderRHI = PixelShader.GetPixelShader();

	SetShaderValue(RHICmdList, ShaderRHI, PixelShader->WaveDampeningScalarParameter, WaveDampening);
	SetShaderValue(RHICmdList, ShaderRHI, PixelShader->FoamDampeningScalarParameter, FoamDampening);
	SetShaderValue(RHICmdList, ShaderRHI, PixelShader->WaveTravelSpeedScalarParameter, WaveTravelSpeed);
	SetShaderValue(RHICmdList, ShaderRHI, PixelShader->TexelSizeScalarParameter, TexelSize);

	SetTextureParameter(
		RHICmdList,
		ShaderRHI,
		PixelShader->PreviewATextureParameter,
		PixelShader->PreviewATextureSamplerParameter,
		TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap, AM_Wrap>::GetRHI(),
		PreviewATexture
	);

	SetTextureParameter(
		RHICmdList,
		ShaderRHI,
		PixelShader->PreviewBTextureParameter,
		PixelShader->PreviewBTextureSamplerParameter,
		TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap, AM_Wrap>::GetRHI(),
		PreviewBTexture
	);

	// Draw a quad mapping scene color to the view's render target
	DrawRectangle(
		RHICmdList,
		0, 0, // Dest X, Y
		RenderTargetResource->GetSizeX(), RenderTargetResource->GetSizeY(), // Dest Width, Height
		0, 0, // Source U, V
		RenderTargetResource->GetSizeX(), RenderTargetResource->GetSizeX(), // Source USize, VSize
		RenderTargetResource->GetSizeXY(), // Target buffer size
		RenderTargetResource->GetSizeXY(), // Source texture size
		VertexShader,
		EDRF_UseTriangleOptimization);

	RHICmdList.EndRenderPass();
}

void FImpactWaterGlobalShaderCommands::ComputeNormalToRenderTarget(FRHICommandList& RHICmdList, FTextureRenderTargetResource* RenderTargetResource,
	const FTextureRHIRef& HeightTextureParameter, float HeightScale, float TexelSize)
{
	FSceneViewFamily ViewFamily(FSceneViewFamily::ConstructionValues(
		RenderTargetResource,
		NULL,
		FEngineShowFlags(ESFIM_Game))
		.SetWorldTimes(0, 0, 0)
		.SetGammaCorrection(RenderTargetResource->GetDisplayGamma()));

	// Set the view family's render target/viewport.
	FRHIRenderPassInfo RPInfo(ViewFamily.RenderTarget->GetRenderTargetTexture(), ERenderTargetActions::DontLoad_Store);

	// Deferred the clear until here so the garbage left in the non rendered regions by the post process effects do not show up
	if (ViewFamily.bDeferClear)
	{
		if (ensure(ViewFamily.RenderTarget->GetRenderTargetTexture()->GetClearColor() == FLinearColor::Black))
		{
			RPInfo.ColorRenderTargets[0].Action = ERenderTargetActions::Clear_Store;
			RHICmdList.BeginRenderPass(RPInfo, TEXT("ComputeNormalToRenderTarget"));
		}
		else
		{
			RHICmdList.BeginRenderPass(RPInfo, TEXT("ComputeNormalToRenderTarget"));
			DrawClearQuad(RHICmdList, FLinearColor::Black);
		}
		ViewFamily.bDeferClear = false;
	}
	else
	{
		RHICmdList.BeginRenderPass(RPInfo, TEXT("ComputeNormalToRenderTarget"));
	}

	DECLARE_GPU_STAT(ComputeNormalToRenderTarget);
	SCOPED_DRAW_EVENT(RHICmdList, ComputeNormalToRenderTarget);
	SCOPED_GPU_STAT(RHICmdList, ComputeNormalToRenderTarget);

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

	// turn off culling and blending
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();

	// turn off depth reads/writes
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

	TShaderMapRef<FImpactWaterCreateNormalVS> VertexShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	TShaderMapRef<FImpactWaterCreateNormalPS> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;

	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	FRHIPixelShader* ShaderRHI = PixelShader.GetPixelShader();

	SetShaderValue(RHICmdList, ShaderRHI, PixelShader->HeightScaleScalarParameter, HeightScale);
	SetShaderValue(RHICmdList, ShaderRHI, PixelShader->TexelSizeScalarParameter, TexelSize);

	SetTextureParameter(
		RHICmdList,
		ShaderRHI,
		PixelShader->HeightTextureParameter,
		PixelShader->HeightTextureSamplerParameter,
		TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap, AM_Wrap>::GetRHI(),
		HeightTextureParameter
	);

	// Draw a quad mapping scene color to the view's render target
	DrawRectangle(
		RHICmdList,
		0, 0, // Dest X, Y
		RenderTargetResource->GetSizeX(), RenderTargetResource->GetSizeY(), // Dest Width, Height
		0, 0, // Source U, V
		RenderTargetResource->GetSizeX(), RenderTargetResource->GetSizeX(), // Source USize, VSize
		RenderTargetResource->GetSizeXY(), // Target buffer size
		RenderTargetResource->GetSizeXY(), // Source texture size
		VertexShader,
		EDRF_UseTriangleOptimization);

	RHICmdList.EndRenderPass();
}