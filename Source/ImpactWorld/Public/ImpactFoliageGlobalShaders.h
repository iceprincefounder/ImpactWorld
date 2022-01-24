// Copyright Tencent Games Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "RenderResource.h"
#include "TextureResource.h"
#include "RHICommandList.h"
#include "ShaderParameterMacros.h"

#define MAX_HITLOCATION_SIZE 8
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FGenerateFoliageSplatUniformParameters, )
	SHADER_PARAMETER_TEXTURE(Texture2D, HitStampTexture)
	SHADER_PARAMETER_SAMPLER(SamplerState, HitStampTextureSampler)
	SHADER_PARAMETER_TEXTURE(Texture2D, CanvasTexture)
	SHADER_PARAMETER_SAMPLER(SamplerState, CanvasTextureSampler)
	SHADER_PARAMETER(float, TexelSizeScalar)
	SHADER_PARAMETER(int, HitLocationNum)
    SHADER_PARAMETER_ARRAY(FVector4, HitParamArray, [MAX_HITLOCATION_SIZE])
    SHADER_PARAMETER_ARRAY(FVector, HitLocationArray, [MAX_HITLOCATION_SIZE]) // This Buffer MUST list at last to adopt METAL API(bug?)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

typedef TUniformBufferRef<FGenerateFoliageSplatUniformParameters> FGenerateFoliageSplatUniformBufferRef;

struct FImpactFoliageGlobalShaderCommands
{
	static void GenerateSplatToRenderTarget(FRHICommandList& RHICmdList, FTextureRenderTargetResource* RenderTargetResource,
		const FGenerateFoliageSplatUniformBufferRef& UniformBuffer);

	static void PropagateSplatToRenderTarget(FRHICommandList& RHICmdList, FTextureRenderTargetResource* RenderTargetResource,
		const FTextureRHIRef& PreviewATexture, const FTextureRHIRef& PreviewBTexture, float Dampening, float TravelSpeed, float TexelSize);

	static void ComputeNormalToRenderTarget(FRHICommandList& RHICmdList, FTextureRenderTargetResource* RenderTargetResource,
		const FTextureRHIRef& HeightTexture, float HeightScale, float TexelSize);
};
