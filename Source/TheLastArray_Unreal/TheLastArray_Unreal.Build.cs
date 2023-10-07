// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheLastArray_Unreal : ModuleRules
{
	public TheLastArray_Unreal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "AIModule", "GameplayTasks", "UMG", "GameplayTags" });
	}
}
