// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetDownloader.h"
#include "Async/AsyncWork.h"
#include "Containers/Queue.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "ProjectContent/AssetDownloader/SDownloadCompleteWidget.h"

DECLARE_DELEGATE_OneParam(FOnDownloadCompleted, const FString& /*FileName*/);


class SAssetDownloadWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetDownloadWidget)
	: _URL(TEXT("")), _FileName(TEXT("")), _MD5(TEXT(""))
	{}
	SLATE_ARGUMENT(FString, URL)
	SLATE_ARGUMENT(FString, FileName)
	SLATE_ARGUMENT(FString, MD5)
	SLATE_ARGUMENT(TWeakPtr<SVerticalBox>, ParentContainer)
	SLATE_ARGUMENT(TWeakPtr<SDownloadCompleteWidget>, DownloadCompleteWidget)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);


	void SetDownloadParameters(const FString& InURL, const FString& InFileName, const FString& InMD5);


	FReply OnStartOrResumeClicked();
	FReply OnPauseClicked();  
	FReply OnCancelClicked(); 


	void OnDownloadProgress(float Progress, int64 BytesDownloaded, int64 InTotalBytes);
	void OnDownloadComplete();
	void OnDownloadError();
	void StartDownload();


	FText GetStartOrResumeButtonText() const;


	bool IsPaused() const { return AssetDownloader->IsPaused(); }

	bool IsFileDownloading(const FString& FilePath);


	void ProcessPendingTasks();
	TWeakPtr<SVerticalBox> ParentContainer;


	static TQueue<TSharedPtr<SAssetDownloadWidget>> PendingTasks; 
	static TArray<TSharedPtr<SAssetDownloadWidget>> ActiveTasks;  
	static TArray<TWeakPtr<SAssetDownloadWidget>> PausedTasks; 
	static TArray<TSharedPtr<SAssetDownloadWidget>> FailedTasks;   
	
	static FCriticalSection ActiveTasksMutex;
	static FCriticalSection PendingTasksMutex;
	static FCriticalSection PausedTasksMutex;  


	FOnDownloadCompleted OnDownloadCompleted;


	void BindOnDownloadCompleted(const FOnDownloadCompleted& Delegate)
	{
		OnDownloadCompleted = Delegate;
	}

	FString TruncateText(const FString& OriginalText, int32 MaxLength);

	bool bIsPausedAllDownloads = false; 

	void SetPausedAllDownloads(bool bPaused);


private:

	
	FString OriginalURL;
	FString AssetURL;
	FString AssetFileName;
	FString AssetMD5;


	UPROPERTY()
	UAssetDownloader* AssetDownloader;


	TSharedPtr<STextBlock> DownloadStatusText;


	TSharedPtr<SProgressBar> DownloadProgressBar;

	TSharedPtr<SButton> StartOrResumeButton;

	FButtonStyle BeginButtonStyle;
	FButtonStyle PauseButtonStyle;
	FButtonStyle CancelButtonStyle;

	bool bIsPaused = false; 

	TWeakPtr<SDownloadCompleteWidget> DownloadCompleteWidget; 
	
	int64 TotalBytes; 
	
};



