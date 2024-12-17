// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SAudioAssetsWidget.h"
#include "Widgets/SCompoundWidget.h"

class UGetAudioAssetLibraryTagListApi;

struct FAudioTagInfo;

DECLARE_DELEGATE(FOnClearAudioTagFilter);
DECLARE_DELEGATE(FOnAudioTagClickClearWidget);
DECLARE_DELEGATE(FOnAudioTagDataReceived);

class SAudioTagWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAudioTagWidget) {}
	SLATE_ARGUMENT(TSharedPtr<SAudioAssetsWidget>, AudioAssetWidget)
	SLATE_EVENT(FOnClearAudioTagFilter, OnClearAudioTagFilter)
	SLATE_EVENT(FOnAudioTagClickClearWidget, OnAudioTagClickClearWidget)
	SLATE_EVENT(FOnAudioTagDataReceived, OnAudioTagDataReceived)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FReply ClearSelectedTags();

private:
	TSharedPtr<SAudioAssetsWidget> AudioAssetsWidget;


	TSharedPtr<SVerticalBox> TagButtonContainer;
	TSharedPtr<SHorizontalBox> SelectedTagsContainer;  

	TArray<FString> SelectedTags; 

	void FetchTagList();
	void AddTagButtons(const TArray<FAudioTagInfo>& TagInfoArray);
	FReply OnTagButtonClicked(const FString& InTagName, const int64& InTagID); 

	FButtonStyle TagButtonStyle;
	FButtonStyle ImportButtonStyle;
	FSlateBrush TransparentBrush;

	void AddTagToSelected(const FString& TagName);  
	
	FOnClearAudioTagFilter OnClearAudioTagFilterDelegate;  
	FOnAudioTagClickClearWidget OnAudioTagClickClearWidget;
	FOnAudioTagDataReceived OnAudioTagDataReceived;

	
};