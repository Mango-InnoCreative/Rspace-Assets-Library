// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetCaptchaApi.generated.h"

UCLASS()
class RSPACEASSETLIBAPI_API UGetCaptchaApi : public UObject
{
	GENERATED_BODY()

public:
	void SendCaptchaRequest(const FString& Mobile);

private:
	FString GenerateMt(const FString& Mobile);
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
