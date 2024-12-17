// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetAudioFileDetailData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetAudioFileDetailApi.generated.h"


DECLARE_DELEGATE_OneParam(FOnGetAudioFileDetailResponse, const FAudioFileDetailData& /* AudioFileData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetAudioFileDetailApi : public UObject
{
	GENERATED_BODY()

public:
	
	void SendGetAudioFileDetailRequest(const FString& Ticket, const FString& FileNo, FOnGetAudioFileDetailResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FOnGetAudioFileDetailResponse OnResponseDelegate;
};
