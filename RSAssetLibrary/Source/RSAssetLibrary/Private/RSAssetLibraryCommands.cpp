// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "RSAssetLibraryCommands.h"

#define LOCTEXT_NAMESPACE "FRSAssetLibraryModule"

void FRSAssetLibraryCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "RSAssetLibrary", "Execute RSAssetLibrary action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
