// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetAudioAssetFilterConditionData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetAudioAssetFilterConditionApi.generated.h"


DECLARE_DELEGATE_OneParam(FOnGetAudioAssetFilterConditionResponse, const FGetAudioAssetFilterConditionResponse& /* AudioFilterConditionData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetAudioAssetFilterConditionApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetAudioAssetFilterConditionRequest(const FString& Ticket, FOnGetAudioAssetFilterConditionResponse InResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	FOnGetAudioAssetFilterConditionResponse OnResponseDelegate;
};
