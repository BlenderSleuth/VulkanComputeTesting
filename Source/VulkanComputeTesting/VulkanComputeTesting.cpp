// Copyright Epic Games, Inc. All Rights Reserved.

#include "VulkanComputeTesting.h"


void FVulkanComputeTesting::StartupModule()
{
	const FString ShaderDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping("/VulkanComputeTesting", ShaderDir);
}

void FVulkanComputeTesting::ShutdownModule()
{ }

IMPLEMENT_PRIMARY_GAME_MODULE( FVulkanComputeTesting, VulkanComputeTesting, "VulkanComputeTesting" );