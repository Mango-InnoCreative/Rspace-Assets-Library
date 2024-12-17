// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetVideoFileInfoData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetVideoFileInfoApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetVideoFileInfoResponse, UGetVideoFileInfoData* /* VideoFileInfoData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetVideoFileInfoApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetVideoFileInfoRequest(const FString& Ticket, const FString& FileNo, FOnGetVideoFileInfoResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	
	FOnGetVideoFileInfoResponse OnResponseDelegate;
};
