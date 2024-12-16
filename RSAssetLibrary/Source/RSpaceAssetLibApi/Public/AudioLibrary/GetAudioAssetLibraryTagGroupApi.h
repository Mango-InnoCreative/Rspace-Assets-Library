// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetAudioAssetLibraryTagGroupData.h"  // 引入定义结构体的头文件
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetAudioAssetLibraryTagGroupApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetAudioAssetLibraryTagGroupResponse, const FGetAudioAssetLibraryTagGroupResponseData&);

UCLASS()
class RSPACEASSETLIBAPI_API UGetAudioAssetLibraryTagGroupApi : public UObject
{
	GENERATED_BODY()

public:
	void SendGetAudioAssetLibraryTagGroupRequest(const FString& Ticket, const FString& Uuid, const FString& ProjectNo, FOnGetAudioAssetLibraryTagGroupResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FOnGetAudioAssetLibraryTagGroupResponse OnResponseDelegate;
};
