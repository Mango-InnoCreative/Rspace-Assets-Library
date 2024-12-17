// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "AssetDownloader.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "GenericPlatform/GenericPlatformFile.h"

void UAssetDownloader::StartChunkDownload(const FString& URL, const FString& FileName, const FString& MD5)
{
    DownloadURL = URL;
    DownloadFileName = FileName;
    DownloadMD5 = MD5;
    
    FString SaveDir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"));
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*SaveDir))
    {
        if (PlatformFile.CreateDirectoryTree(*SaveDir))
        {
            // UE_LOG(LogTemp, Log, TEXT("Successfully created directory: %s"), *SaveDir);
        }
        else
        {
            //UE_LOG(LogTemp, Error, TEXT("Failed to create directory: %s"), *SaveDir);
            if (OnDownloadError.IsBound())
            {
                OnDownloadError.Execute();
            }
            return;
        }
    }
    
    // if (CheckIfFileExistsWithMD5(FileName, MD5))
    // {
    //     // UE_LOG(LogTemp, Log, TEXT("File %s already exists with MD5 %s, skipping download."), *FileName, *MD5);
    //     if (OnFileAlreadyDownloaded.IsBound())
    //     {
    //         OnFileAlreadyDownloaded.Execute();
    //     }
    //     return;
    // }
    HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAssetDownloader::HandleInitialResponse);
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb(TEXT("HEAD"));
    HttpRequest->ProcessRequest();
}

void UAssetDownloader::HandleInitialResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        FString ContentLengthStr = Response->GetHeader("Content-Length");
        DownloadFileSize = FCString::Atoi64(*ContentLengthStr);

        TotalChunks = FMath::CeilToInt((float)DownloadFileSize / ChunkSize);
        DownloadedBytes = 0;
        bIsPaused = false;
        DownloadNextChunk();
    }
    else
    {
        //UE_LOG(LogTemp, Error, TEXT("Failed to get file size from server."));
        if (OnDownloadError.IsBound())
        {
            OnDownloadError.Execute();
        }
    }
}

void UAssetDownloader::DownloadNextChunk()
{
    if (bIsPaused) return;

    if (DownloadedBytes >= DownloadFileSize)
    {
         UE_LOG(LogTemp, Log, TEXT("Download complete."));
        //SaveToCSV(DownloadFileName, DownloadMD5);
        if (OnDownloadComplete.IsBound())
        {
            OnDownloadComplete.Execute();
        }
        return;
    }

    int64 StartByte = DownloadedBytes;
    int64 EndByte = FMath::Min(StartByte + ChunkSize - 1, DownloadFileSize - 1);

    HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAssetDownloader::HandleChunkDownloadComplete);
    HttpRequest->SetURL(DownloadURL);
    HttpRequest->SetVerb(TEXT("GET"));

    FString RangeHeader = FString::Printf(TEXT("bytes=%lld-%lld"), StartByte, EndByte);
    HttpRequest->SetHeader(TEXT("Range"), RangeHeader);
    HttpRequest->ProcessRequest();
}

void UAssetDownloader::HandleChunkDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bIsPaused)
    {
        return;
    }
    
    if (bWasSuccessful && Response->GetResponseCode() == 206)
    {
        FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), DownloadFileName);
        const TArray<uint8>& ChunkData = Response->GetContent();

        IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
        IFileHandle* FileHandle = PlatformFile.OpenWrite(*FilePath, true);
        if (FileHandle)
        {
            //UE_LOG(LogTemp, Error, TEXT("Failed to open file for writing chunk data!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
            FileHandle->Seek(DownloadedBytes);
            FileHandle->Write(ChunkData.GetData(), ChunkData.Num());
            delete FileHandle;

            DownloadedBytes += ChunkData.Num();
            CurrentChunk++;

            float Progress = (float)DownloadedBytes / (float)DownloadFileSize;
            if (OnDownloadProgress.IsBound())
            {
                OnDownloadProgress.Execute(Progress, DownloadedBytes, DownloadFileSize);
            }

            DownloadNextChunk();
            
        }
        else
        {
            //UE_LOG(LogTemp, Error, TEXT("Failed to open file for writing chunk data."));
            if (OnDownloadError.IsBound())
            {
                OnDownloadError.Execute();
            }
        }
    }
    else
    {
       // UE_LOG(LogTemp, Error, TEXT("Chunk download failed."));
        if (OnDownloadError.IsBound())
        {
            OnDownloadError.Execute();
        }
    }
}

void UAssetDownloader::PauseDownload()
{
    if (!bIsPaused)
    {
        bIsPaused = true;
        if (HttpRequest.IsValid() && HttpRequest->GetStatus() == EHttpRequestStatus::Processing)
        {
            HttpRequest->CancelRequest();
            //// UE_LOG(LogTemp, Log, TEXT("Download paused at byte: %lld."), DownloadedBytes);
        }
    }
}

void UAssetDownloader::ResumeDownload()
{
    if (bIsPaused)
    {
        bIsPaused = false;
        DownloadNextChunk();
    }
}

void UAssetDownloader::CancelDownload()
{
    PauseDownload();
    FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), DownloadFileName);
    if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
    {
        FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*FilePath);
    }
}

// bool UAssetDownloader::CheckIfFileExistsWithMD5(const FString& FileName, const FString& MD5)
// {
//     FString CSVFilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), TEXT("AssetsManagement.csv"));
//     FString CSVContent;
//
//     if (FFileHelper::LoadFileToString(CSVContent, *CSVFilePath))
//     {
//         TArray<FString> Lines;
//         CSVContent.ParseIntoArrayLines(Lines);
//
//         for (const FString& Line : Lines)
//         {
//             TArray<FString> Tokens;
//             Line.ParseIntoArray(Tokens, TEXT(","), true);
//             if (Tokens.Num() == 2 && Tokens[0] == FileName && Tokens[1] == MD5)
//             {
//                 return true;
//             }
//         }
//     }
//     return false;
// }

// void UAssetDownloader::SaveToCSV(const FString& FileName, const FString& MD5)
// {
//     FString CSVFilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), TEXT("AssetsManagement.csv"));
//     if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*CSVFilePath))
//     {
//         FFileHelper::SaveStringToFile(TEXT("FileName,MD5\n"), *CSVFilePath);
//     }
//
//     FString CSVLine = FString::Printf(TEXT("%s,%s\n"), *FileName, *MD5);
//     FFileHelper::SaveStringToFile(CSVLine, *CSVFilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM, &IFileManager::Get(), FILEWRITE_Append);
// }

void UAssetDownloader::SetOnDownloadProgress(FOnDownloadProgress InOnDownloadProgress)
{
    OnDownloadProgress = InOnDownloadProgress;
}

void UAssetDownloader::SetOnDownloadComplete(FOnDownloadComplete InOnDownloadComplete)
{
    OnDownloadComplete = InOnDownloadComplete;
}

void UAssetDownloader::SetOnDownloadError(FOnDownloadError InOnDownloadError)
{
    OnDownloadError = InOnDownloadError;
}