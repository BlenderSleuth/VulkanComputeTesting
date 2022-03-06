// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VulkanComputeTesting : ModuleRules
{
	public VulkanComputeTesting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine" });
		PrivateDependencyModuleNames.AddRange(new[] { "RenderCore", "RHI" });
	}
}
