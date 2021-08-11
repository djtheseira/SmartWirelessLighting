using UnrealBuildTool;
using System.IO;
using System;

public class SmartWirelessLighting : ModuleRules
{
    public SmartWirelessLighting(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bLegacyPublicIncludePaths = false;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject",
            "Engine",
            "InputCore",
            "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemNULL",
            "SignificanceManager",
            "PhysX", "APEX", "PhysXVehicles", "ApexDestruction",
            "AkAudio",
            "ReplicationGraph",
            "UMG",
            "AIModule",
            "NavigationSystem",
            "AssetRegistry",
            "GameplayTasks",
            "AnimGraphRuntime",
            "Slate", "SlateCore",
            "Json"
            });


        if (Target.Type == TargetRules.TargetType.Editor) {
			PublicDependencyModuleNames.AddRange(new string[] {"OnlineBlueprintSupport", "AnimGraph"});
		}

        if (Target.bBuildEditor)
        {
            PublicDependencyModuleNames.Add("UnrealEd");
            PrivateDependencyModuleNames.Add("MainFrame");
        }
        PublicDependencyModuleNames.AddRange(new string[] {"FactoryGame", "SML"});
    }
}