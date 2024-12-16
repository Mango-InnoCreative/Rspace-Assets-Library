// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetAudioFileByConditionData.h"
#include "GetAudioFileByConditionApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetAudioFileByConditionResponse, const FGetAudioFileByConditionResponse&);

UCLASS()
class RSPACEASSETLIBAPI_API UGetAudioFileByConditionApi : public UObject
{
	GENERATED_BODY()

public:
	void SendGetAudioFileByConditionRequest(
	const FString& Ticket,
	const FString& Uuid,
	const FString& ProjectNo,
	const FString& GroupId,

	const FString& AudioChannel,
	const FString& AudioHarvestBits,
	const FString& AudioHarvestRate,
	const int32& BpmBegin,
	const int32& BpmEnd,
	const int32& CurrentPage,
	const FString& FileFormat,
	const int32& MenuType,
	const int32& PageSize,
	const FString& Search,
	const FString& Sort,
	const FString& SortType,
	const int64& TagId,
	FOnGetAudioFileByConditionResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	FOnGetAudioFileByConditionResponse OnResponseDelegate;
};
