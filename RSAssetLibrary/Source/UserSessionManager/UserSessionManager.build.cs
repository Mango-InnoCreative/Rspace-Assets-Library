// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

using UnrealBuildTool;

public class UserSessionManager : ModuleRules
{
	public UserSessionManager(ReadOnlyTargetRules Target) : base(Target)
	{
		
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		
		PublicIncludePaths.AddRange(
			new string[] {
				
			}
		);

	
		PrivateIncludePaths.AddRange(
			new string[] {
				
			}
		);

		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UnrealEd",        
				"EditorSubsystem", 
				"RSpaceAssetLibApi" 
			}
		);

		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"RSpaceAssetLibApi"
				
			}
		);
	}
}