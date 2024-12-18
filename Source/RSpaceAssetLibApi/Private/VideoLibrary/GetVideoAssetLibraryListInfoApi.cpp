// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "VideoLibrary/GetVideoAssetLibraryListInfoApi.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"


FCriticalSection VideoActiveRequestsLock;
TSet<FString> VideoActiveRequests;

void UGetVideoAssetLibraryListInfoApi::SendGetVideoAssetLibraryListInfoRequest(const FString& Ticket, const FString& ProjectNo, const FString& ParentId, const FString& FileName, FOnGetVideoAssetLibraryListInfoResponse InResponseDelegate)
{
	this->OnResponseDelegate = InResponseDelegate;


	FString RequestKey = FString::Printf(TEXT("%s-%s-%s"), *ParentId, *ProjectNo, *FileName);

	{

		FScopeLock Lock(&VideoActiveRequestsLock);
		if (VideoActiveRequests.Contains(RequestKey))
		{
			UE_LOG(LogTemp, Warning, TEXT("Duplicate request detected, skipping: %s"), *RequestKey);
			return;
		}
	
		VideoActiveRequests.Add(RequestKey);
	}


	FString Url = FString::Printf(TEXT("https://api.meta.mg.xyz/spaceapi/video/file/getFileListInfo%s/%s?fileName=%s"), *ParentId, *ProjectNo, *FileName);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("authorization"), Ticket);


	Request->OnProcessRequestComplete().BindUObject(this, &UGetVideoAssetLibraryListInfoApi::OnResponseReceivedWithCleanup, RequestKey);
	Request->ProcessRequest();
}

FString UGetVideoAssetLibraryListInfoApi::PreprocessJsonString(const FString& JsonString)
{
	FString ProcessedString = JsonString;
	ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
	return ProcessedString;
}

void UGetVideoAssetLibraryListInfoApi::OnResponseReceivedWithCleanup(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString RequestKey)
{
	
	{
		FScopeLock Lock(&VideoActiveRequestsLock);
		VideoActiveRequests.Remove(RequestKey);
	}


	OnResponseReceived(Request, Response, bWasSuccessful);
}

void UGetVideoAssetLibraryListInfoApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
		FGetVideoAssetLibraryListInfoData VideoAssetData;

		
		if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &VideoAssetData, 0, 0))
		{
			
			if (OnResponseDelegate.IsBound())
			{
				OnResponseDelegate.Execute(&VideoAssetData);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to receive valid response."));
	}
}
