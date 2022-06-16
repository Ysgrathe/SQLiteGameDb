// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SqliteGameDbEditor : ModuleRules
{
	public SqliteGameDbEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{

			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"DeveloperSettings",
				"SqliteGameDB",
				"UnrealEd"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"BlueprintGraph",
				"UnrealEd",
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}