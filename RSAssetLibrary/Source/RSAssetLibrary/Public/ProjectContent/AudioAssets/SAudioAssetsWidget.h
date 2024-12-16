// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Subsystem/USMSubsystem.h"
#include "AudioLibrary/GetAudioFileByConditionApi.h"
#include "AudioLibrary/GetAudioCommentApi.h"
#include "ProjectContent/AssetDownloader/SAssetDownloadWidget.h"

class SVideoPlayerWidget;
struct FAudioAssetLibraryFolderItem;
class UUSMSubsystem;

struct FSelectedAudioFileInfo
{
	FString FileName;
	
	FString FileUrl;
	
	FString FileMd5;
};

DECLARE_DELEGATE_ThreeParams(FOnSelectedAudioDownloadClicked, const FString&, const FString& , const FString&);
DECLARE_DELEGATE_TwoParams(FOnAudioAssetClicked, const FAudioFileData&, TSharedRef<SWidget>);
DECLARE_DELEGATE(FOnAudioNothingToShow)
DECLARE_DELEGATE(FOnAudioRefresh)


class SAudioAssetsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAudioAssetsWidget) {}
	SLATE_EVENT(FOnAudioAssetClicked, OnAudioAssetClicked)
	SLATE_EVENT(FOnSelectedAudioDownloadClicked, OnAudioDownload)
	SLATE_EVENT(FOnAudioNothingToShow, OnAudioNothingToShow) 
	SLATE_EVENT(FOnAudioRefresh, OnAudioRefresh)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);


	void UpdateAudioAssets(const TArray<FAudioAssetLibraryFolderItem>& AudioAsset, const int64& InTagID);

	void UpdateTagPageAudioAssets (const TArray<FAudioFileData>& AudioFileData);
	
	
	FString TruncateText(const FString& OriginalText, int32 MaxLength);

	TSharedRef<SWidget> GenerateDetailsWidget(const FAudioFileData& AudioFile);

	FReply PlayAudioWithSystemPlayer(const FString& InPath);


	void OnDownloadCompleted(const FString& AssetFileName);

	void ClearAudioContent(){ AudioAssetsContainer->ClearChildren(); }

private:

	TSharedPtr<SVerticalBox> AudioAssetsContainer;

	FOnAudioAssetClicked OnAudioAssetClicked;  

	FOnAudioNothingToShow OnAudioNothingToShow;

	FOnAudioRefresh OnAudioRefresh;
	
	UUSMSubsystem* USMSubsystem = nullptr;

	void SetUserAndProjectParams();

	TSharedPtr<SVideoPlayerWidget> VideoPlayerWidget;
	


	FString Ticket;
	FString Uuid;
	int32 FileId;
	FString ProjectNo;
	FString FileName;
	FString TagName;
	FString AudioFileName;

	void InitializeDetailClickedButtonStyle();
	
	FButtonStyle DetailClickedButtonStyle;

	FButtonStyle ImportButtonStyle;

	FButtonStyle PlayCircleButtonStyle;

	TSharedPtr<SVerticalBox> CommentContainer;

	// void HandleAudioCommentResponse(UGetAudioCommentResponseData* ResponseData);

	TSharedPtr<FButtonStyle> SelectedButtonStyle = nullptr;

	FButtonStyle TagButtonStyle;

	FSelectedAudioFileInfo SelectedAudioFileInfo;

	FReply OnDownloadAudioButtonClicked(FSelectedAudioFileInfo AudioInfo);

	FReply OnImportAudioFileButtonClicked();

	void ImportAudioFile(const FString& FilePath);

	void ImportSupportedAudioFormats(const FString& FilePath, const FString& FileExtension);

	void ImportWithFileMediaSource(const FString& FilePath);

	FOnSelectedAudioDownloadClicked OnSelectedAudioDownloadClicked;

	bool IsFileDownloaded(const FString& ForbidFileName) const;

	TSharedPtr<SAssetDownloadWidget> AssetDownloadWidget;
	
	TSharedPtr<SButton> DownloadButton; 


	float AnimationProgress = 0.0f; 

	TSharedPtr<bool> bIsButtonEnabled = MakeShared<bool>(true);

	bool bIsDownloadButtonEnabled = true;

	float LastClickTime = 0.0f;
	const float ClickCooldownTime = 1.0f;

	
	
};
