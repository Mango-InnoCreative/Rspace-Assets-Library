// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "VideoLibrary/GetVideoFileVersionInfoApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"

void UGetVideoFileVersionInfoApi::SendGetVideoFileVersionInfoRequest(const FString& Ticket, const FString& FileNo, FOnGetVideoFileVersionInfoResponse InResponseDelegate)
{
	// 保存传入的委托
	OnResponseDelegate = InResponseDelegate;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString Url = FString::Printf(TEXT("https://api.meta.mg.xyz/spaceapi/video/audit/version/getAuditVersionInfo/%s"), *FileNo);
	Request->SetURL(Url);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Authorization"), Ticket);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->OnProcessRequestComplete().BindUObject(this, &UGetVideoFileVersionInfoApi::OnResponseReceived);
	Request->ProcessRequest();
}

FString UGetVideoFileVersionInfoApi::PreprocessJsonString(const FString& JsonString)
{
	FString ProcessedString = JsonString;
	ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
	return ProcessedString;
}

void UGetVideoFileVersionInfoApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		//FString ResponseContent = Response->GetContentAsString();
		//// UE_LOG(LogTemp, Log, TEXT("GetVideoFileVersionInfo Response: %s"), *ResponseContent);
		FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
		FGetVideoFileVersionInfoData ResponseData;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &ResponseData, 0, 0))
		{
			if (OnResponseDelegate.IsBound())
			{
				OnResponseDelegate.Execute(ResponseData);
			}
		}
		else
		{
			// UE_LOG(LogTemp, Error, TEXT("Failed to parse GetVideoFileVersionInfo response"));
		}
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("Failed to receive valid response for GetVideoFileVersionInfo"));
	}
}
