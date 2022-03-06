// Copyright Ben Sutherland 2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"


class VULKANCOMPUTETESTING_API FTestComputeShader : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FTestComputeShader);
	SHADER_USE_PARAMETER_STRUCT(FTestComputeShader, FGlobalShader);
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<int>, TestUAV)
	END_SHADER_PARAMETER_STRUCT()

	// Called by the engine to determine which permutations to compile for this shader
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	// Modifies the compilations environment of the shader
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};
