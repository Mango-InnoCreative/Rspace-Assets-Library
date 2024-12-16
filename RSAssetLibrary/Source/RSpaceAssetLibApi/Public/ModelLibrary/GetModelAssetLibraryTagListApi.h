// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetModelAssetLibraryTagListData.h" 
#include "Interfaces/IHttpRequest.h"
#include "GetModelAssetLibraryTagListApi.generated.h"


DECLARE_DELEGATE_OneParam(FOnGetModelAssetLibraryTagListResponse, const FGetModelAssetLibraryTagListResponseData&);

UCLASS()
class RSPACEASSETLIBAPI_API UGetModelAssetLibraryTagListApi : public UObject
{
	GENERATED_BODY()

public:
	
	void SendGetModelAssetLibraryTagListRequest(const FString& Ticket, const FString& ProjectNo, FOnGetModelAssetLibraryTagListResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	FOnGetModelAssetLibraryTagListResponse OnResponseDelegate;
};
