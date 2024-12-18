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
	// 初始化时加载本地化文件
	LoadLocalizationForEditorLanguage();
	
	// 监听语言切换事件
	FInternationalization::Get().OnCultureChanged().AddRaw(this, &FRSAssetLibraryModule::OnCultureChanged);

	if (GEditor)
	{
		GEditor->OnEditorClose().AddRaw(this, &FRSAssetLibraryModule::HandleEditorClose);
	}

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
	.SetMenuType(ETabSpawnerMenuType::Hidden).SetIcon(FSlateIcon(TEXT("RSAssetLibraryStyle"),TEXT("PluginIcon.Icon")));
}

void FRSAssetLibraryModule::LoadLocalizationForEditorLanguage()
{
	// 获取当前编辑器的语言
	FString CurrentCulture = FTextLocalizationManager::Get().GetRequestedLocaleName();

	// 根据当前语言加载插件的本地化资源
	FString LocalizationPath = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("RSAssetLibrary"), TEXT("Content"), TEXT("Localization"), TEXT("Game"),CurrentCulture, TEXT("Game.locres"));

	// 确保文件存在
	if (FPaths::FileExists(LocalizationPath))
	{
		// 创建本地化资源对象
		FTextLocalizationResource LocalizationResource;

		// 设置优先级为 0（您可以根据需求修改）
		int32 Priority = 0;

		// 加载本地化资源
		LocalizationResource.LoadFromFile(LocalizationPath, Priority);

		// 使用 `UpdateFromLocalizationResource` 方法更新本地化资源
		FTextLocalizationManager::Get().UpdateFromLocalizationResource(LocalizationResource);
	}
}

// 语言切换事件处理函数
void FRSAssetLibraryModule::OnCultureChanged()
{
	// 当语言发生变化时重新加载本地化资源
	LoadLocalizationForEditorLanguage();
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

	TArray<TSharedRef<SWindow>> AllWindows;

	// Get root window 获取根窗口
	const TArray<TSharedRef<SWindow>>& RootWindows = FSlateApplication::Get().GetTopLevelWindows();
	for (const TSharedRef<SWindow>& RootWindow : RootWindows)
	{
		// Gets all Windows recursively, including minimized Windows 递归获取所有窗口，包括最小化的窗口
		GetAllWindowsRecursive(AllWindows, RootWindow);
	}

	for (const TSharedRef<SWindow>& Window : AllWindows)
	{
		FText WindowTitle = Window->GetTitle();

		// Determine whether the plug-in is related to the window 判断是否是插件相关窗口
		if (WindowTitle.EqualTo(FText::FromString(TEXT("传输列表")))
			|| WindowTitle.EqualTo(FText::FromString(TEXT("提示")))
			|| WindowTitle.EqualTo(FText::FromString(TEXT("Video Player")))
			|| WindowTitle.EqualTo(FText::FromString(TEXT("Audio Player")))
			|| WindowTitle.EqualTo(FText::FromString(TEXT("Image Preview")))
			|| WindowTitle.EqualTo(FText::FromString(TEXT("Download List")))
			|| WindowTitle.EqualTo(FText::FromString(TEXT("Tip")))) 
            
		{
			// UE_LOG(LogTemp, Log, TEXT("Closing plugin-related window: %s"), *WindowTitle.ToString());

			FSlateApplication::Get().RequestDestroyWindow(Window);
		}
	}
	
}

void FRSAssetLibraryModule::GetAllWindowsRecursive(TArray<TSharedRef<SWindow>>& OutWindows, TSharedRef<SWindow> CurrentWindow)
{
	// Add the current window (whether visible or minimized) 添加当前窗口（无论是否可见或最小化）
	OutWindows.Add(CurrentWindow);

	// Traverse the child Windows of the current window 遍历当前窗口的子窗口
	const TArray<TSharedRef<SWindow>>& WindowChildren = CurrentWindow->GetChildWindows();
	for (int32 ChildIndex = 0; ChildIndex < WindowChildren.Num(); ++ChildIndex)
	{
		GetAllWindowsRecursive(OutWindows, WindowChildren[ChildIndex]);
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
					return FText::FromString(TEXT("Open RSpace Plugin"));
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