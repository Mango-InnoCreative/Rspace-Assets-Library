// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetVideoAssetLibraryData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetVideoAssetLibraryApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetVideoAssetLibraryResponse, FGetVideoAssetLibraryResponseData* /* ResponseData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetVideoAssetLibraryApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetVideoAssetLibraryRequest(const FString& Ticket, const FString& Uuid, const FString& ProjectNo, const FString& FileNo, FOnGetVideoAssetLibraryResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FOnGetVideoAssetLibraryResponse OnGetVideoAssetLibraryResponseDelegate;
};
