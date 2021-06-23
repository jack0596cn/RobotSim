// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RobotSim : ModuleRules
{
	public RobotSim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Slate",
            "SlateCore",
            "Engine",
            "InputCore",
            "Networking",
            "Json",
            "JsonUtilities",
            "Sockets"
        });
	}
}
