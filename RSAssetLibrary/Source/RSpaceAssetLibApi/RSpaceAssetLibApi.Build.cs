// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

using UnrealBuildTool;

public class RSpaceAssetLibApi : ModuleRules
{
	public RSpaceAssetLibApi(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"HTTP",
			"Json",
			"JsonUtilities",
			"WebBrowser",
			"WebBrowserWidget"
		});
		
		// Conditionally include UnrealEd only for editor builds
		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.Add("UnrealEd");
		}
        
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"WebBrowser",
		});
	}
}