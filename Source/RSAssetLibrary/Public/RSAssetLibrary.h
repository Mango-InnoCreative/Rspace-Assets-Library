// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class SMainWidget;
class FToolBarBuilder;
class FMenuBuilder;

class FRSAssetLibraryModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();

	void HandleEditorClose();

	void GetAllWindowsRecursive(TArray<TSharedRef<SWindow>>& OutWindows, TSharedRef<SWindow> CurrentWindow);
	
private:

	void RegisterMenus();

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	const FSlateBrush* TabInIcon;

	TSharedPtr<SDockTab> DockTab;
	
	TSharedPtr<SMainWidget> MainWidgetPtr;

	TSharedRef<SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	void OnPluginTabClosed(TSharedRef<SDockTab> ClosedTab);
	
	void LoadLocalizationForEditorLanguage();

	void OnCultureChanged();
};
