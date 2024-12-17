// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Subsystem/USMSubsystem.h"
#include "ConceptDesignLibrary/GetConceptDesignLibraryApi.h"
#include "ProjectContent/AssetDownloader/SAssetDownloadWidget.h"

struct FFileItemDetails;
struct FConceptDesignFileItem;

struct FSelectedConceptFileInfo
{
	FString FileName;
	
	FString FileUrl;
	
	FString FileMd5;
};

class UUSMSubsystem;

DECLARE_DELEGATE_ThreeParams(FOnSelectedConceptDownloadClicked, const FString&, const FString& , const FString&);
DECLARE_DELEGATE_TwoParams(FOnConceptDesignAssetClicked, const FConceptDesignFileItem&, TSharedRef<SWidget>);
DECLARE_DELEGATE(FOnConceptNothingToShow)
DECLARE_DELEGATE(FOnConceptRefresh)



class SConceptDesignWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SConceptDesignWidget) {}
	SLATE_EVENT(FOnConceptDesignAssetClicked, OnConceptDesignAssetClicked)  
	SLATE_EVENT(FOnSelectedConceptDownloadClicked, OnConceptDownload) 
	SLATE_EVENT(FOnConceptNothingToShow, OnConceptNothingToShow)  
	SLATE_EVENT(FOnConceptRefresh, OnConceptRefresh)  
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void UpdateConceptDesignAssets(const TArray<FConceptDesignFolderItem>& ConceptDesignAsset, const FString& InTagID);

	void UpdateConceptDesignTagPageAssets(const TArray<FFileItemDetails> ConceptItems);
	
	FString TruncateText(const FString& OriginalText, int32 MaxLength);

	TSharedRef<SWidget> GenerateDetailsWidget(const FConceptDesignFileItem& ConceptDesignFileItem);
	
	void OnDownloadCompleted(const FString& AssetFileName);

	void ClearConceptContent();

	TArray<UTexture2D*> LoadedTextures; 
private:
	
	TSharedPtr<SVerticalBox> ConceptDesignAssetsContainer;
	TSharedRef<SWidget> ConstructImageItem(const FString& ProjectImageUrl);

	FOnConceptDesignAssetClicked OnConceptDesignAssetClicked; 

	FOnConceptNothingToShow OnConceptNothingToShow;

	FOnConceptRefresh OnConceptRefresh;
	
	UUSMSubsystem* USMSubsystem = nullptr;

	void SetUserAndProjectParams();
	


	FString Ticket;
	FString Uuid;
	int32 FileId;
	FString ProjectNo;
	FString FileName;
	FString TagName;
	
	int32 CurrentPage;
	int32 PageSize;
	int32 FolderId;

	TArray<FString> Filetag;

	void InitializeDetailClickedButtonStyle();
	
	FButtonStyle DetailClickedButtonStyle;

	FButtonStyle ImportButtonStyle;

	FButtonStyle PlayImageCircleButtonStyle;

	FButtonStyle TagButtonStyle;

	FString ConceptFileName;

	TSharedPtr<SVerticalBox> CommentContainer;

	TSharedPtr<FButtonStyle> SelectedButtonStyle = nullptr;

	FSelectedConceptFileInfo SelectedConceptFileInfo;

	FOnSelectedConceptDownloadClicked OnSelectedConceptDownloadClicked;


	FReply OnDownloadConceptButtonClicked(FSelectedConceptFileInfo ConceptInfo);

	FReply OnImportConceptFileButtonClicked(const FSelectedConceptFileInfo& InSelectedConceptFileInfo);

	void ImportConceptFile(const FString& FilePath);

	bool IsFileDownloaded(const FString& ForbidFileName) const;

	TSharedPtr<SAssetDownloadWidget> AssetDownloadWidget;
	
	TSharedPtr<SButton> DownloadButton; 

	FString FormatFileSize(int64 FileSizeInBytes) const;  

	float AnimationProgress = 0.0f;

	bool bIsButtonEnabled = true;

	bool bIsDownloadButtonEnabled = true;  

	float LastClickTime = 0.0f;
	const float ClickCooldownTime = 1.0f; 


};
