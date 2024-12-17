// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetModelFileTagData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetModelFileTagApi.generated.h"


DECLARE_DELEGATE_OneParam(FOnGetModelFileTagResponse, const FModelFileTagData& /* TagData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetModelFileTagApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetModelFileTagRequest(const FString& Ticket, const FString& FileNo, const FString& ProjectNo, FOnGetModelFileTagResponse InOnGetModelFileTagResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	FOnGetModelFileTagResponse OnGetModelFileTagResponseDelegate;
};
