// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "RSAssetLibraryStyle.h"

class FRSAssetLibraryCommands : public TCommands<FRSAssetLibraryCommands>
{
public:

	FRSAssetLibraryCommands()
		: TCommands<FRSAssetLibraryCommands>(TEXT("RSAssetLibrary"), NSLOCTEXT("Contexts", "RSAssetLibrary", "RSAssetLibrary Plugin"), NAME_None, FRSAssetLibraryStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
