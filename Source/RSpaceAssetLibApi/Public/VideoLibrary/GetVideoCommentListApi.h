// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetVideoCommentListData.h"  
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetVideoCommentListApi.generated.h"


DECLARE_DELEGATE_OneParam(FOnGetVideoCommentListResponse, const FGetVideoCommentListResponseData& /* VideoCommentData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetVideoCommentListApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetVideoCommentListRequest(const FString& Ticket, const FString& Uuid, const FString& AuditNo, FOnGetVideoCommentListResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	
	FOnGetVideoCommentListResponse OnResponseDelegate;
};
