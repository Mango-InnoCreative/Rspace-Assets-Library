// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "RSAssetLibrary.h"
#include "RSAssetLibraryStyle.h"
#include "RSAssetLibraryCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "SMainWidget.h"
#include "ProjectContent/SProjectWidget.h"
#include "Tickable.h"
#include "ProjectContent/Imageload/FImageLoader.h"


static const FName RSAssetLibraryTabName("RSAssetLibrary");

#define LOCTEXT_NAMESPACE "FRSAssetLibraryModule"

void FRSAssetLibraryModule::StartupModule()
{
	if (GEditor)
	{
		GEditor->OnEditorClose().AddRaw(this, &FRSAssetLibraryModule::HandleEditorClose);
	}

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FRSAssetLibraryStyle::Initialize();
	FRSAssetLibraryStyle::ReloadTextures();

	FRSAssetLibraryCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	TabInIcon = FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.Icon");

	PluginCommands->MapAction(
		FRSAssetLibraryCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FRSAssetLibraryModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FRSAssetLibraryModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner("RSAssetLibrary", FOnSpawnTab::CreateRaw(this, &FRSAssetLibraryModule::OnSpawnPluginTab))
	.SetDisplayName(FText::FromString("RSpace Asset Library"))
	.SetMenuType(ETabSpawnerMenuType::Hidden).SetIcon(FSlateIcon(TEXT("RSAssetLibraryStyle"),TEXT("PluginIcon.Icon")));;
}

void FRSAssetLibraryModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FImageLoader::CancelAllImageRequests();
	
	DockTab.Reset();

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FRSAssetLibraryStyle::Shutdown();

	FRSAssetLibraryCommands::Unregister();
}

void FRSAssetLibraryModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("RSAssetLibrary"));
}

void FRSAssetLibraryModule::HandleEditorClose()
{
	if (MainWidgetPtr.IsValid())
	{
		// Get the current widget and clean it up as necessary 获取当前的小部件并进行必要的清理操作
		TSharedPtr<SCompoundWidget> InitialWidget = MainWidgetPtr->GetCurrentWidget();

		TSharedPtr<SProjectWidget> ProjectWidget = StaticCastSharedPtr<SProjectWidget>(InitialWidget);
		if (ProjectWidget.IsValid())
		{
			ProjectWidget->CancelAllDownloads();
			ProjectWidget->CloseAllOpenedWindows();
		}
	}
}

void FRSAssetLibraryModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FRSAssetLibraryCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FRSAssetLibraryCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);

				// Set the static Tooltip 设置静态 Tooltip
				Entry.ToolTip = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateLambda([]()
				{
					return FText::FromString(TEXT("打开 RSpace 插件面板"));
				}));
			}
		}
	}
	
}

TSharedRef<SDockTab> FRSAssetLibraryModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	if (!MainWidgetPtr.IsValid())
	{
		MainWidgetPtr = SNew(SMainWidget);
	}

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.OnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this, &FRSAssetLibraryModule::OnPluginTabClosed))
		[
			SNew(SBox)
			.MinDesiredWidth(1620.0f)
			.MinDesiredHeight(1080.0f)
			[
				MainWidgetPtr.ToSharedRef()
			]
		];
}



void FRSAssetLibraryModule::OnPluginTabClosed(TSharedRef<SDockTab> ClosedTab)
{
	// UE_LOG(LogTemp, Error, TEXT("OnPluginTabClosed called for tab: %s"), *ClosedTab->GetTabLabel().ToString());

	if (MainWidgetPtr.IsValid())
	{
		TSharedPtr<SCompoundWidget> InitialWidget = MainWidgetPtr->GetCurrentWidget();
		
		TSharedPtr<SProjectWidget> ProjectWidget = StaticCastSharedPtr<SProjectWidget>(InitialWidget);
		if (ProjectWidget.IsValid())
		{
			ProjectWidget->CancelAllDownloads();
			ProjectWidget->CloseAllOpenedWindows();
		}
	}

	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	

}







#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRSAssetLibraryModule, RSAssetLibrary)