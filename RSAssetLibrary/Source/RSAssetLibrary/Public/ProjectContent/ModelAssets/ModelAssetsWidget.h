// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectContent/AssetDownloader/SAssetDownloadWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Subsystem/USMSubsystem.h"

struct FModelFileDetails;
class UUSMSubsystem;
struct FModelFileItem;

struct FSelectedModelFileInfo
{
	FString FileName;
	
	FString FileUrl;
	
	FString FileMd5;
};

DECLARE_DELEGATE_TwoParams(FOnModelAssetClicked, const FModelFileItem&, TSharedRef<SWidget>);
DECLARE_DELEGATE_ThreeParams(FOnSelectedModelDownloadClicked, const FString&, const FString& , const FString&);
DECLARE_DELEGATE_OneParam(FOnModelUpdateDetailsBar, TSharedRef<SWidget>);
DECLARE_DELEGATE(FOnModelNothingToShow);
DECLARE_DELEGATE(FOnModelRefresh);



class SModelAssetsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SModelAssetsWidget) {}
	SLATE_EVENT(FOnModelAssetClicked, OnModelAssetClicked)
	SLATE_EVENT(FOnSelectedModelDownloadClicked, OnSelectedModelDownloadClicked)
	SLATE_EVENT(FOnModelNothingToShow, OnModelNothingToShow)
	SLATE_EVENT(FOnModelRefresh, OnModelRefresh)
	
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);


	void UpdateModelAssets(const TArray<FModelFileItem>& ModelAssetsData);
	
	FString TruncateText(const FString& OriginalText, int32 MaxLength);

	TSharedRef<SWidget> GenerateDetailsWidget(const FModelFileDetails& FileDetailsItem);

	void OnModelFileDownloadButtonClicked(FModelFileDetails ModelFileDetailsItem, const FString& RelativePath);

	FReply OnImportFBXButtonClicked();

	void ImportFBXFile(const FString& FilePath);

	TSharedRef<SWidget> LoadImageFromUrl(const FString& GifUrl);

	void OnDownloadCompleted(const FString& AssetFileName);

	void OnVersionSelected(TSharedPtr<FString> SelectedVersion, ESelectInfo::Type SelectInfo);
	
	void ClearModelContent();

	FOnModelUpdateDetailsBar OnModelUpdateDetailsBar;
	FOnModelNothingToShow OnModelNothingToShow;
	FOnModelRefresh OnModelRefresh;
	

private:

	TSharedPtr<SVerticalBox> ModelAssetsContainer;

	FOnModelAssetClicked OnModelAssetClicked;  

	FOnSelectedModelDownloadClicked OnSelectedModelDownloadClicked;
	
	UUSMSubsystem* USMSubsystem = nullptr;

	FSelectedModelFileInfo SelectedModelFileInfo;

	FString Ticket;
	FString Uuid;
	int32 FileId;
	FString ProjectNo;
	FString FileName;
	FString TagName;
	FString Tag;
	FString ModelFileName;

	void InitializeDetailClickedButtonStyle();
	
	FButtonStyle DetailClickedButtonStyle;

	FButtonStyle PlayImageCircleButtonStyle;

	FButtonStyle ImportButtonStyle;

	TSharedPtr<SVerticalBox> CommentContainer;
	
	TSharedPtr<FButtonStyle> SelectedButtonStyle = nullptr;


	TArray<FString> ModelTags;

	void SetUserAndProjectParams();

	FButtonStyle TagButtonStyle;

	TSharedRef<SWidget> GenerateTagsWidget(const FString& TagData);
	
	void RequestModelTags(const FModelFileItem& FileItem);

	void RefreshTagsWidget();
	
	TSharedPtr<SWidget> TagsPanel;
	
	TArray<FString> TagArray;

	void UpdateTagUI(TSharedRef<SWidget> DetailsWidget);

	bool IsFileDownloaded(const FString& ForbidFileName) const;

	TSharedPtr<SAssetDownloadWidget> AssetDownloadWidget;
	
	TSharedPtr<SButton> DownloadButton;


	TArray<TSharedPtr<FString>> VersionOptions;

	TSharedPtr<SComboBox<TSharedPtr<FString>>> VersionComboBox;

	TSharedPtr<FString> CurrentSelectedVersion;

	TSharedPtr<FString> SelectedVersion;         
	TMap<int32, FString> VersionToFileNoPathMap;

	TMap<int32, FString> VersionToRelativePathMap;


	TSharedPtr<FString> SelectedFileNo = nullptr;


	float AnimationProgress = 0.0f;

	TArray<UTexture2D*> LoadedTextures; 

	int32 InitialModelVersion;  

	
	TSharedPtr<FString> SelectedRelativePath = nullptr;

	bool bIsButtonEnabled = true; 
	
	float LastClickTime = 0.0f;
	const float ClickCooldownTime = 1.0f; 

};

