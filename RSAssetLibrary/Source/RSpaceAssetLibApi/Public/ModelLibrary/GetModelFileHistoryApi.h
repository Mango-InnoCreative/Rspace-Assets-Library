// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetModelFileHistoryData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetModelFileHistoryApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetModelFileHistoryResponse, UGetModelFileHistoryResponseData* /* ModelFileHistoryData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetModelFileHistoryApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetModelFileHistoryRequest(const FString& FileNo, const FString& Ticket, FOnGetModelFileHistoryResponse InGetModelFileHistoryResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	
	FOnGetModelFileHistoryResponse OnGetModelFileHistoryResponseDelegate;
};
