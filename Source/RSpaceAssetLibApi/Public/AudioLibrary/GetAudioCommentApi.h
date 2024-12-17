// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetAudioCommentData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetAudioCommentApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetAudioCommentResponse, UGetAudioCommentResponseData* /* ResponseData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetAudioCommentApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetAudioCommentRequest(const FString& Ticket, const FString& Uuid, const FString& FileNo, int32 CurrentPage, int32 PageSize, FOnGetAudioCommentResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	FOnGetAudioCommentResponse OnGetAudioCommentResponseDelegate;
};
