// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetVideoAssetLibraryListInfoData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetVideoAssetLibraryListInfoApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetVideoAssetLibraryListInfoResponse, FGetVideoAssetLibraryListInfoData* /* VideoAssetData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetVideoAssetLibraryListInfoApi : public UObject
{
	GENERATED_BODY()

public:
	
	void SendGetVideoAssetLibraryListInfoRequest(const FString& Ticket, const FString& ProjectNo, const FString& ParentId, const FString& FileName, FOnGetVideoAssetLibraryListInfoResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void OnResponseReceivedWithCleanup(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString RequestKey);

	FOnGetVideoAssetLibraryListInfoResponse OnResponseDelegate;
};
