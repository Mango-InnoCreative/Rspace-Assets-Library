// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetAudioAssetLibraryFolderListData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetAudioAssetLibraryFolderListApi.generated.h"


DECLARE_DELEGATE_OneParam(FOnGetAudioAssetLibraryFolderListResponse, FGetAudioAssetLibraryFolderListData* /* AudioAssetFolderData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetAudioAssetLibraryFolderListApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetAudioAssetLibraryFolderListRequest(const FString& Ticket, const FString& Uuid, const FString& ProjectNo, const FString& GroupName, FOnGetAudioAssetLibraryFolderListResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FOnGetAudioAssetLibraryFolderListResponse OnResponseDelegate;
};
