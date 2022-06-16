// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TutorialDemo : ModuleRules
{
	public TutorialDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"DeveloperSettings",
			"SqliteGameDB"
		});
	}
}