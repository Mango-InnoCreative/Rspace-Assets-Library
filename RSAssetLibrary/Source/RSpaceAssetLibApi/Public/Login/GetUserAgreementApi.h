// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetUserAgreementApi.generated.h"


DECLARE_DELEGATE_OneParam(FOnUserAgreementResponse, FString);

/**
 * Get User Agreement API Class
 */
UCLASS()
class RSPACEASSETLIBAPI_API UGetUserAgreementApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetUserAgreementRequest(FOnUserAgreementResponse ResponseCallback);
private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnUserAgreementResponse OnResponse);

	FString ParseHtmlContent(const FString& HtmlContent);
};
