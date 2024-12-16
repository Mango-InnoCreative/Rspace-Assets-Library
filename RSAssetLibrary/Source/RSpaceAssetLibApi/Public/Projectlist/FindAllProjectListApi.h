// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "FindAllProjectListResponseData.h"
#include "FindAllProjectListApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnFindAllProjectListResponse, UFindAllProjectListResponseData* /* AllProjectListData */);

UCLASS()
class RSPACEASSETLIBAPI_API UFindAllProjectListApi : public UObject
{
	GENERATED_BODY()

public:
	
	void SendFindAllProjectListRequest(const FString& Ticket, const FString& Uuid, FOnFindAllProjectListResponse InFindAllProjectListResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	
	FOnFindAllProjectListResponse OnFindAllProjectListResponseDelegate;
};
