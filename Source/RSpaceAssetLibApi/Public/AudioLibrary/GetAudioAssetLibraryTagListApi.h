// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetAudioAssetLibraryTagListData.h"  // 引入定义结构体的头文件
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetAudioAssetLibraryTagListApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetAudioAssetLibraryTagListResponse, const FGetAudioAssetLibraryTagListResponseData& /* AudioTagListData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetAudioAssetLibraryTagListApi : public UObject
{
	GENERATED_BODY()

public:
	void SendGetAudioAssetLibraryTagListRequest(const FString& Ticket, const FString& Uuid, const FString& ProjectNo, int32 GroupStatus, FOnGetAudioAssetLibraryTagListResponse InOnResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FOnGetAudioAssetLibraryTagListResponse OnResponseDelegate;
};
