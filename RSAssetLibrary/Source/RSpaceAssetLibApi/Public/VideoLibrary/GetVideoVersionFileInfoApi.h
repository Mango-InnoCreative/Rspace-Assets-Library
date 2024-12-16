// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetVideoVersionFileInfoData.h"
#include "Interfaces/IHttpRequest.h"
#include "GetVideoVersionFileInfoApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetVideoVersionFileInfoResponse, UGetVideoVersionFileInfoData* );

UCLASS()
class RSPACEASSETLIBAPI_API UGetVideoVersionFileInfoApi : public UObject
{
	GENERATED_BODY()

public:
	void SendGetVideoVersionFileInfoRequest(const FString& Ticket, const FString& Uuid, const FString& AuditNo, FOnGetVideoVersionFileInfoResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	FOnGetVideoVersionFileInfoResponse OnResponseDelegate;
};
