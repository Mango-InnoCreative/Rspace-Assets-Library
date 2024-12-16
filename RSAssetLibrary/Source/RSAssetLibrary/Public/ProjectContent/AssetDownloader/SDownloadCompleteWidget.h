// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"


struct FCompletedDownloadEntry
{
	TSharedPtr<SHorizontalBox> DownloadBox;
	FString FileName;
	FString FilePath;
};

class SDownloadCompleteWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDownloadCompleteWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);


	void AddCompletedDownload(const FString& FileName, const FString& FilePath, const FString& FileSizeText);

	void RemoveFileEntryFromCSV(const FString& FileName);

	FString TruncateText(const FString& OriginalText, int32 MaxLength);

private:
	TSharedPtr<SVerticalBox> CompletedDownloadsContainer;

	void LoadCompletedDownloadsFromFolder();

	TArray<FCompletedDownloadEntry> CompletedDownloads; 

	FButtonStyle DeleteButtonStyle;
	FButtonStyle SearchButtonStyle;
};