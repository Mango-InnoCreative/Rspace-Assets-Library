// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "Login/GetCaptchaApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/SecureHash.h"

void UGetCaptchaApi::SendCaptchaRequest(const FString& Mobile)
{
	FString Mt = GenerateMt(Mobile);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/am/user/getCaptcha"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FString JsonPayload = FString::Printf(TEXT("{\"mobile\":\"%s\",\"mt\":\"%s\"}"), *Mobile, *Mt);
	Request->SetContentAsString(JsonPayload);

	Request->OnProcessRequestComplete().BindUObject(this, &UGetCaptchaApi::OnResponseReceived);
	Request->ProcessRequest();
}

FString UGetCaptchaApi::GenerateMt(const FString& Mobile)
{
	int32 Count1 = Mobile.Len() - Mobile.Replace(TEXT("1"), TEXT("")).Len();
	int32 Count8 = Mobile.Len() - Mobile.Replace(TEXT("8"), TEXT("")).Len();

	FString InputString = FString::Printf(TEXT("%s:%d:%d"), *Mobile, Count1, Count8);

	FMD5 MD5;
	FTCHARToUTF8 Convert(*InputString);
	const uint8* Data = (const uint8*)Convert.Get();
	MD5.Update(Data, Convert.Length());

	uint8 Hash[16];
	MD5.Final(Hash);

	return BytesToHex(Hash, 16).ToLower();
}

void UGetCaptchaApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		//// UE_LOG(LogTemp, Log, TEXT("Captcha response: %s"), *Response->GetContentAsString());
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("Failed to send captcha request"));
	}
}
