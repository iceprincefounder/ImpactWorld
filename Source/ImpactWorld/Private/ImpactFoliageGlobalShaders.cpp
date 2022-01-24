// Copyright Tencent Games, Inc / @KEVINTSUIXU. All Rights Reserved.

#pragma once

#include "ImpactFoliageGlobalShaders.h"
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

class FImpactFoliageGenerateSplatPS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactFoliageGenerateSplatPS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactFoliageGenerateSplatPS() {}

public:
	/** Initialization constructor. */
	FImpactFoliageGenerateSplatPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}

	void SetUniformBuffer(FRHICommandList& RHICmdList, FGenerateFoliageSplatUniformBufferRef UniformBuffer)
	{
		FRHIPixelShader* ShaderRHI = RHICmdList.GetBoundPixelShader();
		SetUniformBufferParameter(RHICmdList, ShaderRHI, GetUniformBufferParameter<FGenerateFoliageSplatUniformParameters>(), UniformBuffer);
	}
};

class FImpactFoliageGenerateSplatVS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactFoliageGenerateSplatVS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactFoliageGenerateSplatVS() {}

public:

	/** Initialization constructor. */
	FImpactFoliageGenerateSplatVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}
};

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FGenerateFoliageSplatUniformParameters, "GenerateFoliageSplatUniforms");
IMPLEMENT_SHADER_TYPE(, FImpactFoliageGenerateSplatPS, TEXT("/Plugin/ImpactWorld/Private/ImpactFoliageGenerateSplat.usf"), TEXT("MainPS"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(, FImpactFoliageGenerateSplatVS, TEXT("/Plugin/ImpactWorld/Private/ImpactFoliageGenerateSplat.usf"), TEXT("MainVS"), SF_Vertex);

class FImpactFoliagePropagateSplatPS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactFoliagePropagateSplatPS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactFoliagePropagateSplatPS() {}

public:
	LAYOUT_FIELD(FShaderResourceParameter, PreviewATextureParameter);
	LAYOUT_FIELD(FShaderResourceParameter, PreviewATextureSamplerParameter);
	LAYOUT_FIELD(FShaderResourceParameter, PreviewBTextureParameter);
	LAYOUT_FIELD(FShaderResourceParameter, PreviewBTextureSamplerParameter);
	LAYOUT_FIELD(FShaderParameter, DampeningScalarParameter);
	LAYOUT_FIELD(FShaderParameter, WaveTravelSpeedScalarParameter);
	LAYOUT_FIELD(FShaderParameter, TexelSizeScalarParameter);

	/** Initialization constructor. */
	FImpactFoliagePropagateSplatPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		PreviewATextureParameter.Bind(Initializer.ParameterMap, TEXT("PreviewATexture"));
		PreviewATextureSamplerParameter.Bind(Initializer.ParameterMap, TEXT("PreviewATextureSampler"));
		PreviewBTextureParameter.Bind(Initializer.ParameterMap, TEXT("PreviewBTexture"));
		PreviewBTextureSamplerParameter.Bind(Initializer.ParameterMap, TEXT("PreviewBTextureSampler"));
		DampeningScalarParameter.Bind(Initializer.ParameterMap, TEXT("DampeningScalar"));
		WaveTravelSpeedScalarParameter.Bind(Initializer.ParameterMap, TEXT("WaveTravelSpeedScalar"));
		TexelSizeScalarParameter.Bind(Initializer.ParameterMap, TEXT("TexelSizeScalar"));
	}
};

class FImpactFoliagePropagateSplatVS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactFoliagePropagateSplatVS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactFoliagePropagateSplatVS() {}

public:
	/** Initialization constructor. */
	FImpactFoliagePropagateSplatVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}
};

IMPLEMENT_SHADER_TYPE(, FImpactFoliagePropagateSplatPS, TEXT("/Plugin/ImpactWorld/Private/ImpactFoliagePropagateSplat.usf"), TEXT("MainPS"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(, FImpactFoliagePropagateSplatVS, TEXT("/Plugin/ImpactWorld/Private/ImpactFoliagePropagateSplat.usf"), TEXT("MainVS"), SF_Vertex);

class FImpactFoliageCreateNormalPS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactFoliageCreateNormalPS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactFoliageCreateNormalPS() {}

public:
	LAYOUT_FIELD(FShaderResourceParameter, HeightTextureParameter);
	LAYOUT_FIELD(FShaderResourceParameter, HeightTextureSamplerParameter);
	LAYOUT_FIELD(FShaderParameter, HeightScaleScalarParameter);
	LAYOUT_FIELD(FShaderParameter, TexelSizeScalarParameter);
	
	/** Initialization constructor. */
	FImpactFoliageCreateNormalPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		HeightTextureParameter.Bind(Initializer.ParameterMap, TEXT("HeightTexture"));
		HeightTextureSamplerParameter.Bind(Initializer.ParameterMap, TEXT("HeightTextureSampler"));
		HeightScaleScalarParameter.Bind(Initializer.ParameterMap, TEXT("HeightScaleScalar"));
		TexelSizeScalarParameter.Bind(Initializer.ParameterMap, TEXT("TexelSizeScalar"));
	}
};

class FImpactFoliageCreateNormalVS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImpactFoliageCreateNormalVS, Global);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	/** Default constructor. */
	FImpactFoliageCreateNormalVS() {}

public:
	/** Initialization constructor. */
	FImpactFoliageCreateNormalVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}
};

IMPLEMENT_SHADER_TYPE(, FImpactFoliageCreateNormalPS, TEXT("/Plugin/ImpactWorld/Private/ImpactFoliageComputeNormal.usf"), TEXT("MainPS"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(, FImpactFoliageCreateNormalVS, TEXT("/Plugin/ImpactWorld/Private/ImpactFoliageComputeNormal.usf"), TEXT("MainVS"), SF_Vertex);

void FImpactFoliageGlobalShaderCommands::GenerateSplatToRenderTarget(FRHICommandList& RHICmdList, FTextureRenderTargetResource* RenderTargetResource, const FGenerateFoliageSplatUniformBufferRef& UniformBuffer)
{
	// Set the view family's render target/viewport.
	FRHIRenderPassInfo RPInfo(RenderTargetResource->GetRenderTargetTexture(), ERenderTargetActions::DontLoad_Store);

	RHICmdList.BeginRenderPass(RPInfo, TEXT("ImpactFoliage_GenerateSplatToRenderTarget"));

	DECLARE_GPU_STAT(ImpactFoliage_GenerateSplatToRenderTarget);
	SCOPED_DRAW_EVENT(RHICmdList, ImpactFoliage_GenerateSplatToRenderTarget);
	SCOPED_NAMED_EVENT(ImpactFoliage_GenerateSplatToRenderTarget, FColor::Blue);
	SCOPED_GPU_STAT(RHICmdList, ImpactFoliage_GenerateSplatToRenderTarget);

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

	// turn off culling and blending
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();

	// turn off depth reads/writes
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

	TShaderMapRef<FImpactFoliageGenerateSplatVS> VertexShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	TShaderMapRef<FImpactFoliageGenerateSplatPS> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

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

void FImpactFoliageGlobalShaderCommands::PropagateSplatToRenderTarget(FRHICommandList& RHICmdList, FTextureRenderTargetResource* RenderTargetResource,
	const FTextureRHIRef& PreviewATexture, const FTextureRHIRef& PreviewBTexture, float Dampening, float WaveTravelSpeed, float TexelSize)
{
	// Set the view family's render target/viewport.
	FRHIRenderPassInfo RPInfo(RenderTargetResource->GetRenderTargetTexture(), ERenderTargetActions::DontLoad_Store);

	RHICmdList.BeginRenderPass(RPInfo, TEXT("ImpactFoliage_PropagateSplatToRenderTarget"));

	DECLARE_GPU_STAT(ImpactFoliage_PropagateSplatToRenderTarget);
	SCOPED_DRAW_EVENT(RHICmdList, ImpactFoliage_PropagateSplatToRenderTarget);
	SCOPED_NAMED_EVENT(ImpactFoliage_PropagateSplatToRenderTarget, FColor::Blue);
	SCOPED_GPU_STAT(RHICmdList, ImpactFoliage_PropagateSplatToRenderTarget);

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

	// turn off culling and blending
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();

	// turn off depth reads/writes
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

	TShaderMapRef<FImpactFoliagePropagateSplatVS> VertexShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	TShaderMapRef<FImpactFoliagePropagateSplatPS> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;

	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	FRHIPixelShader* ShaderRHI = PixelShader.GetPixelShader();

	SetShaderValue(RHICmdList, ShaderRHI, PixelShader->DampeningScalarParameter, Dampening);
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

void FImpactFoliageGlobalShaderCommands::ComputeNormalToRenderTarget(FRHICommandList& RHICmdList, FTextureRenderTargetResource* RenderTargetResource,
	const FTextureRHIRef& HeightTexture, float HeightScale, float TexelSize)
{
	// Set the view family's render target/viewport.
	FRHIRenderPassInfo RPInfo(RenderTargetResource->GetRenderTargetTexture(), ERenderTargetActions::DontLoad_Store);

	RHICmdList.BeginRenderPass(RPInfo, TEXT("ImpactFoliage_ComputeNormalToRenderTarget"));

	DECLARE_GPU_STAT(ImpactFoliage_ComputeNormalToRenderTarget);
	SCOPED_DRAW_EVENT(RHICmdList, ImpactFoliage_ComputeNormalToRenderTarget);
	SCOPED_GPU_STAT(RHICmdList, ImpactFoliage_ComputeNormalToRenderTarget);

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

	// turn off culling and blending
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();

	// turn off depth reads/writes
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

	TShaderMapRef<FImpactFoliageCreateNormalVS> VertexShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	TShaderMapRef<FImpactFoliageCreateNormalPS> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

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
		HeightTexture
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