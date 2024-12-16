// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "UObject/NoExportTypes.h"
#include "AssetDownloader.generated.h"

DECLARE_DELEGATE_ThreeParams(FOnDownloadProgress, float /*Progress*/, int64 /*BytesDownloaded*/, int64 /*TotalBytes*/);
DECLARE_DELEGATE(FOnDownloadComplete);
DECLARE_DELEGATE(FOnDownloadError);
DECLARE_DELEGATE(FOnFileAlreadyDownloaded);

UCLASS()
class RSPACEASSETLIBAPI_API UAssetDownloader : public UObject
{
	GENERATED_BODY()

public:
	void StartChunkDownload(const FString& URL, const FString& FileName, const FString& MD5);
	void PauseDownload();
	void ResumeDownload();
	void CancelDownload();
	FString PreprocessJsonString(const FString& JsonString);

	void SetOnDownloadProgress(FOnDownloadProgress InOnDownloadProgress);
	void SetOnDownloadComplete(FOnDownloadComplete InOnDownloadComplete);
	void SetOnDownloadError(FOnDownloadError InOnDownloadError);
	void SetOnFileAlreadyDownloaded(FOnFileAlreadyDownloaded InOnFileAlreadyDownloaded);
	bool IsPaused() const { return bIsPaused; }
	bool bIsPaused = false;

private:
	void HandleInitialResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void DownloadNextChunk();
	void HandleChunkDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	bool CheckIfFileExistsWithMD5(const FString& FileName, const FString& MD5);
	void SaveToCSV(const FString& FileName, const FString& MD5);

	FOnDownloadProgress OnDownloadProgress;
	FOnDownloadComplete OnDownloadComplete;
	FOnDownloadError OnDownloadError;
	FOnFileAlreadyDownloaded OnFileAlreadyDownloaded;

	FString DownloadURL;
	FString DownloadFileName;
	FString DownloadMD5;
	int64 DownloadFileSize = 0;
	int64 DownloadedBytes = 0;
	int32 TotalChunks = 0;
	int32 CurrentChunk = 0;

	TSharedPtr<IHttpRequest> HttpRequest;
	static constexpr int64 ChunkSize = 512 * 512; 
};
