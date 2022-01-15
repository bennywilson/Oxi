// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Oxi : ModuleRules
{
	public Oxi(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.Add("Oxi");
        PublicDependencyModuleNames.AddRange(new string[] { "AIMODULE", "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
