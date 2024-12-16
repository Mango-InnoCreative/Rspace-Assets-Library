// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "FindProjectListResponseData.h" 
#include "FindProjectListApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnFindProjectListResponse, UFindProjectListResponseData* /* ProjectListData */);

UCLASS()
class RSPACEASSETLIBAPI_API UFindProjectListApi : public UObject
{
	GENERATED_BODY()

public:
	
	void SendFindProjectListRequest(const FString& Ticket, const FString& Uuid, FOnFindProjectListResponse InFindProjectListResponseDelegate);

private:
	FString PreprocessJsonString(const FString& JsonString);
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	
	FOnFindProjectListResponse OnFindProjectListResponseDelegate;
};
