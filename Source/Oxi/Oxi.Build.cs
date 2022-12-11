// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Oxi : ModuleRules
{
	public Oxi(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.Add("Oxi");
        PublicDependencyModuleNames.AddRange(new string[] { "AIModule", "Core", "CoreUObject", "Engine", "PhysicsCore", "InputCore", "HeadMountedDisplay" });
	}
}
