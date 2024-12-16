// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetVideoFolderInfoData.h"  
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetVideoFolderInfoApi.generated.h"


DECLARE_DELEGATE_OneParam(FOnGetVideoFolderInfoResponse, FGetVideoFolderInfoData* /* VideoFolderData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetVideoFolderInfoApi : public UObject
{
	GENERATED_BODY()

public:
	
	void SendGetVideoFolderInfoRequest(const FString& Ticket, const FString& Uuid, const FString& FileNo, const FString& ProjectNo, FOnGetVideoFolderInfoResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	
	FOnGetVideoFolderInfoResponse OnResponseDelegate;
};
