// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Terra : ModuleRules
{
	public Terra(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "GameplayAbilities", "UMG", "Slate", "SlateCore" });

    }
}
