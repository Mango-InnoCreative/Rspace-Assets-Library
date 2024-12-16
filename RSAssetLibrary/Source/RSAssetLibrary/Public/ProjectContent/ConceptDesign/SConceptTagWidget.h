// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UGetConceptDesignLibraryApi;
class SConceptDesignWidget;
struct FConceptDesignFileItemTagList;

DECLARE_DELEGATE(FOnClearConceptTagFilter);
DECLARE_DELEGATE(FOnConceptTagClickClearWidget);
DECLARE_DELEGATE(FOnConceptTagDataReceived);



class SConceptTagWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SConceptTagWidget) {}
	SLATE_ARGUMENT(TSharedPtr<SConceptDesignWidget>, ConceptDesignWidget)
	SLATE_EVENT(FOnClearConceptTagFilter, OnClearConceptTagFilter)
	SLATE_EVENT(FOnConceptTagClickClearWidget, OnConceptTagClickClearWidget)
	SLATE_EVENT(FOnConceptTagDataReceived, OnConceptTagDataReceived)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FReply ClearSelectedTags();


	// void ConceptSetShouldCallAudioApi(bool bValue);

private:
	TSharedPtr<SConceptDesignWidget> ConceptDesignWidget;

	TSharedPtr<SVerticalBox> TagButtonContainer;
	TSharedPtr<SHorizontalBox> SelectedTagsContainer; 

	TArray<FString> SelectedTags;  

	void FetchTagList();
	void AddTagButtons(const TArray<FConceptDesignFileItemTagList>& TagInfoList);
	FReply OnTagButtonClicked(const FString& InTagName, const int64& InTagID);  

	FButtonStyle TagButtonStyle;
	FButtonStyle ImportButtonStyle;
	FSlateBrush TransparentBrush;

	void AddTagToSelected(const FString& TagName);  

	UGetConceptDesignLibraryApi* GetConceptDesignLibraryApi;


	FOnClearConceptTagFilter OnClearConceptTagFilter;
	FOnConceptTagClickClearWidget OnConceptTagClickClearWidget;
	FOnConceptTagDataReceived OnConceptTagDataReceived;
};