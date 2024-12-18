// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AudioAssets/SAudioTagWidget.h"
#include "ConceptDesign/SConceptTagWidget.h"
#include "ModelAssets/SModelTagWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Projectlist/FindProjectListResponseData.h"  // 包含 FProjectItem 的定义
#include "Subsystem/USMSubsystem.h"
#include "ProjectContent/AssetDownloader/SDownloadCompleteWidget.h"
#include "Widgets/Layout/SScrollBox.h"


class UGetModelLibrary;
class SAssetDownloadWidget;
DECLARE_DELEGATE(FOnLogoutDelegate);

struct FConceptDesignFileItem;
struct FVideoAssetInfo;
struct FConceptDesignFolderItem;
struct FAudioAssetLibraryFolderItem;
struct FAudioFileData;
struct FModelFileItem;
class SModelAssetsWidget;
class SVideoAssetsWidget;
class SAudioAssetsWidget;
class SConceptDesignWidget;

enum class EButtonClick 
{
	ConceptDesign,
	AudioAssets,
	VideoAssets,
	ModelAssets,
	None
};

enum class ETagClick 
{
	ConceptTag,
	AudioTag,
	VideoTag,
	ModelTag,
	None
};

enum class EDownloadQueueState
{
	Downloading,
	Completed
};

enum class EActiveWidget
{
	None,
	ConceptDesign,
	AudioAssets,
	VideoAssets,
	ModelAssets
};

class SProjectWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SProjectWidget) {}
	SLATE_ARGUMENT(FUserAndProjectInfo, UserAndProjectInfo)  
	SLATE_EVENT(FOnLogoutDelegate, OnLogout) 
    SLATE_END_ARGS()


	/** Constructs this widget with InArgs */
	
	void Construct(const FArguments& InArgs);


public:
	EButtonClick ButtonClick = EButtonClick::None;
	EActiveWidget CurrentActiveWidget = EActiveWidget::None;
	ETagClick TagClick = ETagClick::None;
	
public:
	void CLearWidgetTexture();

	TArray<TSharedPtr<FProjectItem>> ProjectItems;
	
	FReply OnProjectButtonClicked();

	TSharedRef<SWidget> MakeOptionBar();
	
	TSharedRef<SWidget> MakeSearchBar();

	void UpdateDetailsBar(TSharedRef<SWidget> InWidget);


	FReply OnConceptDesignClicked();

	bool bIsConceptFirstPage = false;
	
	FReply OnAudioAssetsClicked();

	// Video Asset Tree
	
	// TMap<int32, bool> VideoChildExpandedStateMap;	

	TSet<int32> VideoChildExpandedStateSet;
	
	FReply OnVideoAssetsClicked();
	
	void HandleVideoRootDirectory(EButtonClick ButtonType);

	void GenerateVideoAssetTree(int32 CurrentFileId, TSharedPtr<SVerticalBox> ParentBox, FString CurrentParentId, int32 MaxLength);

	bool bIsAudioFirstPage = false;

	
	// Model Asset Tree
	TMap<int32, bool> ModelChildExpandedStateMap;	
	
	FReply OnModelAssetsClicked();

	void HandleModelRootDirectory(EButtonClick ButtonType);

	void GenerateModelAssetTree(int32 CurrentFileId, TSharedPtr<SVerticalBox> ParentBox, int32 MaxLength);

	void ShowAllAudioFiles();

	void ShowAllConceptFiles();

	void CollapseAllExcept(EButtonClick ActiveButton);

	void ResetSelectedTag();

	void OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType);
	
	FReply OnTagButtonClicked();
	
	FReply OnDownloadButtonClicked();

	FReply OnUserInfoButtonClicked();

	FReply  OnLogoutButtonClicked();

	// EVisibility GetTagBoxVisibility() const;

	void ToggleTagContainerVisibility();

	void ResetToggleTagContainer();

	void ClearConceptTagFilter();

	void ClearAudioTagFilter();
	
	void ClearModelTagFilter();

	void ClearAllCachedTextures();


	void LoadAndDisplayUserAvatar();

	bool CheckProjectIsSelected();


	FTextBlockStyle InitializeFont();

	void SetUserAndProjectParams();
	
	void RequestAudioAssetLibrary(int32 CurrentFileId, TSharedPtr<SVerticalBox> ParentBox);

	void RequestConceptDesignLibrary(int32 CurrentFileId, TSharedPtr<SVerticalBox> ParentBox);
	
	void HandleModelAssetClicked(const FModelFileItem& ClickedFileItem, TSharedRef<SWidget> DetailsWidget);

	void HandleModelAssetDownloadClicked(const FString& InName, const FString& InURL, const FString& InMD5);

	void HandleAudioAssetClicked(const FAudioFileData& ClickedAudioFolder, TSharedRef<SWidget> DetailsWidget);
	
	void HandleAudioAssetDownloadClicked(const FString& InName, const FString& InURL, const FString& InMD5);
	
	void HandleConceptDesignAssetClicked(const FConceptDesignFileItem& ClickedAudioFolder, TSharedRef<SWidget> DetailsWidget);

	void HandleConceptAssetDownloadClicked(const FString& InName, const FString& InURL, const FString& InMD5);

	void HandleVideoAssetClicked(const FVideoAssetInfo& ClickedVideoFileItem, TSharedRef<SWidget> DetailsWidget);

	void HandleVideoAssetDownloadClicked(const FString& InName, const FString& InURL, const FString& InMD5);

	void ResetExpandedState(EButtonClick ParentButtonType);

	void UpdateRightContentBox(TSharedRef<SWidget> NewContentWidget);

	void UpdateModelAssetsWidget(const TArray<FModelFileItem>& ModelAssetsData);

	void UpdateAudioAssetsWidget(const TArray<FAudioAssetLibraryFolderItem>& SpecificAudioData);

	void UpdateConceptDesignAssetsWidget(const TArray<FConceptDesignFolderItem>&SpecificConceptDesignData);

	void UpdateVideoAssetsWidget(const TArray<FVideoAssetInfo>& VideoAssetsData);

	void ClearSessionVariables();

	void ResetSlateWidgets();

	void ResetDetailBar();

	void ResetAllButtonStyles();

	void CloseProjectListWindow();

	void HandleFocusChanging(const FFocusEvent& InFocusEvent, 
							  const FWeakWidgetPath& InWeakWidgetPath, 
							  const TSharedPtr<SWidget>& InNewFocusedWidget, 
							  const FWidgetPath& InWidgetPath, 
							  const TSharedPtr<SWidget>& InCurrentFocus);
	
	void SetProjectButtonEnabled(bool bEnabled);

	void OnProjectSelected();

	void HandleProjectSwitched();

	void DoubleClearCheck(TSharedPtr<SVerticalBox> TreeContainer);
	

	// Initialize Button Style

	void InitializeConceptButtonStyle();
	void InitializeAudioButtonStyle();
	void InitializeVideoButtonStyle();
	void InitializeModelButtonStyle();
	void InitializeProjectListButtonStyle();
	void InitializeConceptFolderButtonStyle();
	void InitializeAudioFolderButtonStyle();
	void InitializeVideoFolderButtonStyle();
	void InitializeModelFolderButtonStyle();
	void InitializeLogoutButtonStyle();

	

	void ShowDownloadQueue();
	TSharedPtr<SAssetDownloadWidget> AssetDownloadWidget; 
	TSharedPtr<SVerticalBox> DownloadQueueContainer;
	TSharedPtr<SWindow> DownloadQueueWindow;
	TSharedPtr<SWindow> ExistingNotificationWindow;
	TSharedPtr<SWindow> ProjectSelectedNotificationWindow;

	void OnDownloadQueueWindowClosed(const TSharedRef<SWindow>& ClosedWindow);
	void AddToDownloadQueue(const FString& FileName, const FString& URL, const FString& MD5);
	static TQueue<TSharedPtr<SAssetDownloadWidget>> PendingTasks;
	static TArray<TSharedPtr<SAssetDownloadWidget>> ActiveTasks;


public:
	void StartAllDownloads();
	void PauseAllDownloads();
	void CancelAllDownloads();
private:
	TSharedPtr<SAudioTagWidget> AudioTagWidget; 
	TSharedPtr<SConceptTagWidget> ConceptTagWidget; 
	TSharedPtr<SModelTagWidget> ModelTagWidget;
	
	bool bIsVideoButtonClicked = false;
	bool bIsProjectListWindowOpen = false;
	
	// UI Components
	TSharedPtr<SEditableTextBox> SearchBox;
	TSharedPtr<SButton> DownloadButton;
	TSharedPtr<SButton> ProjectlistButton;
	TSharedPtr<SBox> ContentBar;
	TSharedPtr<SBox> DetailsBar;
	TSharedPtr<SButton> InputButton;
	TSharedPtr<SButton> UserInfoButton;
	TSharedPtr<SButton> MainTagButton;

	TSharedPtr<SConceptDesignWidget> ConceptDesignWidget;
	TSharedPtr<SAudioAssetsWidget> AudioAssetsWidget;
	TSharedPtr<SVideoAssetsWidget> VideoAssetsWidget;
	TSharedPtr<SModelAssetsWidget> ModelAssetsWidget;


	TSharedPtr<SVerticalBox> ConceptDesignTreeContainer;
	TSharedPtr<SVerticalBox> AudioTreeContainer;
	TSharedPtr<SVerticalBox> VideoTreeContainer;
	TSharedPtr<SVerticalBox> ModelTreeContainer;

	TSharedPtr<SBox> TagContainer;

	TSharedPtr<SVerticalBox> ProjectListContainer;

	FString CurrentProjectName = TEXT(""); 

	// Button Style
	FButtonStyle ConceptButtonStyle;
	FButtonStyle AudioButtonStyle;
	FButtonStyle VideoButtonStyle;
	FButtonStyle ModelButtonStyle;
	FButtonStyle ProjectListButtonStyle;
	FButtonStyle ConceptFolderButtonStyle;
	FButtonStyle AudioFolderButtonStyle;
	FButtonStyle VideoFolderButtonStyle;
	FButtonStyle ModelFolderButtonStyle;
	FButtonStyle DownloadButtonStyle;
	FButtonStyle CompleteButtonStyle;
	FButtonStyle LogoutButtonStyle;
	FButtonStyle AllDownloadButtonStyle;
	FButtonStyle TagButtonStyle;
	FButtonStyle DownloadWindowButtonStyle;
	FSlateBrush TransparentBrush;

	bool TagButton = true;
	bool DownloadWindowButton = true;
	

	TSharedPtr<FButtonStyle> SelectedButtonStyle = nullptr;
	
	TSharedPtr<SImage> ModelAssetsIcon;
	TSharedPtr<SImage> UserAvatarImageWidget;
	TSharedPtr<FSlateBrush> UserAvatarBrush;

	TMap<EButtonClick, bool> ExpandedStateMap;
	
	// Windows for popups
	TSharedPtr<SWindow> DownloadWindow;
	TSharedPtr<SWindow> ProjectlistWindow;
	TSharedPtr<SWindow> LoginWindow;

	UUSMSubsystem* USMSubsystem = nullptr;

	// Current Saved Params
	FString Ticket;
	FString Uuid;
	int32 FileId;
	FString ProjectNo;
	FString FileName;
	FString TagName;
	int64 TagID;

	FUserAndProjectInfo UserAndProjectInfo;

	FString TruncateText(const FString& OriginalText, int32 MaxLength);

private:
	UGetModelLibrary* GetModelLibraryApi = nullptr;
	
private:
	FOnLogoutDelegate OnLogoutDelegate;


	TSharedPtr<SDownloadCompleteWidget> DownloadCompleteWidget; 

	bool bIsTagBoxVisible = false;
	
	void UpdateScrollBoxContent(const TSharedRef<SWidget>& NewContent);

	void ShowDownloadingQueue();

	void ShowCompletedQueue();

	void OnDownloadComplete(const FString& InFileName, const FString& InFilePath, const FString& FileSizeText );
	
	TSharedPtr<SVerticalBox> CompletedDownloadContainer;
	TSharedPtr<SScrollBox> ScrollBox;
	EDownloadQueueState CurrentQueueState;

private:
	TSharedPtr<FActiveTimerHandle> ActiveTimerHandle;
	
	FCurveSequence TagAnimationSequence;
	FCurveHandle TagAnimationCurve;

	FCurveSequence ProjectListAnimationSequence;
	FCurveHandle ProjectListAnimationCurve;

	bool bIsAllDownloadButtonEnabled = false; 
	bool bIsPauseAllButtonEnabled = true;   
	FDelegateHandle FocusChangeHandle;
	
public:
	void CleanInvalidPausedTasks();
	
	void CloseAllOpenedWindows();

	void GetAllWindowsRecursive(TArray<TSharedRef<SWindow>>& OutWindows, TSharedRef<SWindow> CurrentWindow);
	
	const int32 MaxDownloadQueueSize = 10;
	
};
