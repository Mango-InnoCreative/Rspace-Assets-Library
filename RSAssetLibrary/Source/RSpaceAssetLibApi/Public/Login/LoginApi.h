// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "LoginApiResponseData.h" 
#include "Modules/ModuleManager.h"
#include "LoginApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnLoginResponse, const FLoginApiResponse&);

UCLASS()
class RSPACEASSETLIBAPI_API ULoginApi : public UObject
{
	GENERATED_BODY()

public:
	
	void SendLoginRequest(const FString& Mobile, const FString& Captcha, FOnLoginResponse InLoginResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	FString GetMD5(const FString& Input);

	FOnLoginResponse OnLoginResponseDelegate;
};

class FLoginApi : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
