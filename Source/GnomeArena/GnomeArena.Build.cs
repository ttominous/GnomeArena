// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class GnomeArena : ModuleRules
{
	public GnomeArena(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EngineSettings", "UMG", "Slate", "SlateCore", "XmlParser", "AnimationCore", "Niagara", "AIModule", "Networking", "Sockets", "COREPlay" });

		PrivateDependencyModuleNames.AddRange(new string[] { "COREPlay" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
