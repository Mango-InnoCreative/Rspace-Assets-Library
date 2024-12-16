// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetVideoFileVersionInfoData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetVideoFileVersionInfoApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetVideoFileVersionInfoResponse, const FGetVideoFileVersionInfoData& /* VideoFileVersionData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetVideoFileVersionInfoApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetVideoFileVersionInfoRequest(const FString& Ticket, const FString& FileNo, FOnGetVideoFileVersionInfoResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	FOnGetVideoFileVersionInfoResponse OnResponseDelegate;
};
