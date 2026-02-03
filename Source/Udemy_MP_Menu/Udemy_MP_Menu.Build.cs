// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Udemy_MP_Menu : ModuleRules
{
	public Udemy_MP_Menu(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Udemy_MP_Menu",
			"Udemy_MP_Menu/Variant_Platforming",
			"Udemy_MP_Menu/Variant_Platforming/Animation",
			"Udemy_MP_Menu/Variant_Combat",
			"Udemy_MP_Menu/Variant_Combat/AI",
			"Udemy_MP_Menu/Variant_Combat/Animation",
			"Udemy_MP_Menu/Variant_Combat/Gameplay",
			"Udemy_MP_Menu/Variant_Combat/Interfaces",
			"Udemy_MP_Menu/Variant_Combat/UI",
			"Udemy_MP_Menu/Variant_SideScrolling",
			"Udemy_MP_Menu/Variant_SideScrolling/AI",
			"Udemy_MP_Menu/Variant_SideScrolling/Gameplay",
			"Udemy_MP_Menu/Variant_SideScrolling/Interfaces",
			"Udemy_MP_Menu/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
