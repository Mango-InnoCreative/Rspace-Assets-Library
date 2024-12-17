// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "Login/LoginApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/SecureHash.h"
#include "Json.h"
#include "JsonObjectConverter.h"


IMPLEMENT_MODULE(FLoginApi, LoginApi)  

void ULoginApi::SendLoginRequest(const FString& Mobile, const FString& Captcha, FOnLoginResponse InLoginResponseDelegate)
{
	this->OnLoginResponseDelegate = InLoginResponseDelegate;

	FString MD5Hash = GetMD5(Mobile);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/am/user/mobileLogin"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	FString JsonPayload = FString::Printf(TEXT("{\"mobile\":\"%s\",\"captcha\":\"%s\"}"), *Mobile, *Captcha);
	Request->SetContentAsString(JsonPayload);

	Request->OnProcessRequestComplete().BindUObject(this, &ULoginApi::OnResponseReceived);
	Request->ProcessRequest();
}

FString ULoginApi::PreprocessJsonString(const FString& JsonString)
{
	FString ProcessedString = JsonString;
	ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
	return ProcessedString;
}

void ULoginApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		FString ResponseContent = Response->GetContentAsString();
		//// UE_LOG(LogTemp, Log, TEXT("LoginApi Response: %s"), *ResponseContent);
		//FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
	
		FLoginApiResponse ApiResponse;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &ApiResponse, 0, 0))
		{
			
			if (OnLoginResponseDelegate.IsBound())
			{
				OnLoginResponseDelegate.Execute(ApiResponse); 
			}
		}
		else
		{
			// UE_LOG(LogTemp, Error, TEXT("Failed to parse LoginApi response JSON"));
		}
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("Failed to send LoginApi request"));
	}
}


FString ULoginApi::GetMD5(const FString& Input)
{
	FMD5 MD5;
	FTCHARToUTF8 Convert(*Input);
	const uint8* Data = (const uint8*)Convert.Get();
	MD5.Update(Data, Convert.Length());

	uint8 Hash[16];
	MD5.Final(Hash);

	return BytesToHex(Hash, 16).ToLower();  
}

void FLoginApi::StartupModule()
{
	//// UE_LOG(LogTemp, Log, TEXT("FLoginApi module has started"));
}

void FLoginApi::ShutdownModule()
{
	//// UE_LOG(LogTemp, Log, TEXT("FLoginApi module has shut down"));
}
