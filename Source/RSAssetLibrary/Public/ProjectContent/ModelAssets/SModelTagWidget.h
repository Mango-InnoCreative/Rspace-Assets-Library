// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ModelAssetsWidget.h"

class UGetModelLibrary;
struct FModelTagInfo;

DECLARE_DELEGATE(FOnClearModelTagFilter);
DECLARE_DELEGATE(FOnModelTagClickClearWidget);
DECLARE_DELEGATE(FOnModelTagDataReceived);



class SModelTagWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SModelTagWidget) {}
	SLATE_ARGUMENT(TSharedPtr<SModelAssetsWidget>, ModelAssetsWidget)
	SLATE_EVENT(FOnClearModelTagFilter, OnClearModelTagFilter)
	SLATE_EVENT(FOnModelTagClickClearWidget, OnModelTagClickClearWidget)
	SLATE_EVENT(FOnModelTagDataReceived, OnModelTagDataReceived)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FReply ClearSelectedTags();

private:
	TSharedPtr<SModelAssetsWidget> ModelAssetsWidget;


	TSharedPtr<SVerticalBox> TagButtonContainer;
	TSharedPtr<SHorizontalBox> SelectedTagsContainer; 

	TArray<FString> SelectedTags;  

	void FetchTagList();
	void AddTagButtons(const TArray<FModelTagInfo>& TagInfoList);
	FReply OnTagButtonClicked(const FString& InTagName);  

	FButtonStyle TagButtonStyle;
	FButtonStyle ImportButtonStyle;
	FSlateBrush TransparentBrush;

	void AddTagToSelected(const FString& TagName);  

	UGetModelLibrary* GetModelLibraryApi;


	FOnClearModelTagFilter OnClearModelTagFilterDelegate; 
	FOnModelTagClickClearWidget OnModelTagClickClearWidget;
	FOnModelTagDataReceived OnModelTagDataReceived;
};
