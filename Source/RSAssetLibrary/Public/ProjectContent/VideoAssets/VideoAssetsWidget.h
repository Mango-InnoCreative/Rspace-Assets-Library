// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ProjectContent/AssetDownloader/SAssetDownloadWidget.h"
#include "VideoLibrary/GetVideoAssetLibraryListInfoData.h"
#include "Widgets/SCompoundWidget.h"
#include "Subsystem/USMSubsystem.h"
#include "VideoLibrary/GetVideoVersionFileInfoData.h"

class SVideoPlayerWidget;
class UFileMediaSource;
class UUSMSubsystem;

DECLARE_DELEGATE_OneParam(FOnVedioUpdateDetailsBar, TSharedRef<SWidget>); 
DECLARE_DELEGATE(FOnVideoNothingToShow)

struct FVideoAssetInfo;

struct FSelectedVideoFileInfo
{
	FString FileName;
	
	FString FileUrl;
	
	FString FileMd5;
};

DECLARE_DELEGATE_ThreeParams(FOnSelectedVideoDownloadClicked, const FString&, const FString& , const FString&);
DECLARE_DELEGATE_TwoParams(FOnVideoAssetClicked, const FVideoAssetInfo&, TSharedRef<SWidget>);



class SVideoAssetsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SVideoAssetsWidget) {}
	SLATE_EVENT(FOnVideoAssetClicked, OnVideoAssetClicked)
	SLATE_EVENT(FOnSelectedVideoDownloadClicked, OnVideoDownload)
	SLATE_EVENT(FOnVideoNothingToShow, OnVideoNothingToShow)
	
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);


	void UpdateVideoAssets(const TArray<FVideoAssetInfo>& VideoAssetsData);
	
	FString TruncateText(const FString& OriginalText, int32 MaxLength);

	TSharedRef<SWidget> GenerateDetailsWidget(UGetVideoVersionFileInfoData* VideoVersionFileDetail);

	FReply PlayMediaWithSystemPlayer(const FString& InPath);

	FReply OnDownloadVideoButtonClicked(FSelectedVideoFileInfo VideoInfo);

	FReply OnImportVideoFileButtonClicked();
	
	void ImportVideoFile(const FString& FilePath);


	void OnDownloadCompleted(const FString& AssetFileName);

	void SearchVideoFileByName(const FText& InputFileName);

	void ClearVideoContent();


	FOnVedioUpdateDetailsBar OnVedioUpdateDetailsBar;


private:

	TSharedPtr<SVerticalBox> VideoAssetsContainer;
	TSharedRef<SWidget> ConstructImageItem(const FString& ProjectImageUrl);

	FOnVideoAssetClicked OnVideoAssetClicked; 
	FOnVideoNothingToShow OnVideoNothingToShow;
	
	UUSMSubsystem* USMSubsystem = nullptr;
	void SetUserAndProjectParams();
	void OnImageReady(const TArray<uint8>& ImageData);

	FString Ticket;
	FString Uuid;
	int32 FileId;
	FString ProjectNo;
	FString FileName;
	FString TagName;

	FString Videourl;
	FString VideoImage;
	FString VideoNo;
	FString VideoFileName;
	
	TSharedPtr<FSlateBrush> PreviewBrush;

	TSharedPtr<SVerticalBox> CommentContainer;

	TSharedPtr<SVideoPlayerWidget> VideoPlayerWidget;

	void InitializeDetailButtonStyle();
	
	FButtonStyle DetailClickedButtonStyle;

	FButtonStyle ImportButtonStyle;

	FButtonStyle PlayCircleButtonStyle;
	
	FSelectedVideoFileInfo SelectedVideoFileInfo;

	FOnSelectedVideoDownloadClicked OnSelectedVideoDownloadClicked;
	
	UFileMediaSource* SavedMediaSource = nullptr;

	TSharedPtr<FButtonStyle> SelectedButtonStyle = nullptr;

	bool IsFileDownloaded(const FString& ForbidFileName) const;

	TSharedPtr<SAssetDownloadWidget> AssetDownloadWidget;
	
	TSharedPtr<SButton> DownloadButton; 


	TArray<TSharedPtr<FString>> VersionOptions;


	TSharedPtr<SComboBox<TSharedPtr<FString>>> VersionComboBox;

	TSharedPtr<FString> CurrentSelectedVersion;

	TSharedPtr<FString> SelectedVersion;       

	TMap<FString, FString> VersionToAuditNoMap;

	
	TSharedPtr<FString> SelectedAuditNo = nullptr;

	void OnVersionSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);

	FString InitialVideoVersion; 

	FString InitialURL;

	float AnimationProgress = 0.0f; 

	TArray<UTexture2D*> LoadedTextures; 

	TMap<FString, FVideoAssetInfo> SelectedVersionData;

	bool bIsButtonEnabled = true; 
	float LastClickTime = 0.0f;
	const float ClickCooldownTime = 1.0f; 
};
