// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SwithModelFileVersionData.h"
#include "Interfaces/IHttpRequest.h"
#include "SwithModelFileVersionApi.generated.h"

DECLARE_DELEGATE_OneParam(FSwithModelFileVersionApiResponse, const FSwithModelFileVersionData& );

UCLASS()
class RSPACEASSETLIBAPI_API USwithModelFileVersionApi : public UObject
{
	GENERATED_BODY()

public:
	void SendSwithModelFileVersionRequest(const FString& Ticket, const FString& FileNo, const FString& Uuid, const int32& Version, FSwithModelFileVersionApiResponse);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	FSwithModelFileVersionApiResponse OnSwithModelFileVersionApiResponse;
};
