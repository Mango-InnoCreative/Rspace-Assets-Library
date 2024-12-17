// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ModelLibrary/SwithModelFileVersionApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"

void USwithModelFileVersionApi::SendSwithModelFileVersionRequest(const FString& Ticket, const FString& FileNo, const FString& Uuid, const int32& Version, FSwithModelFileVersionApiResponse InResponseDelegate)
{
	this->OnSwithModelFileVersionApiResponse = InResponseDelegate;
    
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/model/history/switchVersion"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Authorization"), Ticket);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	FString JsonPayload = FString::Printf(TEXT("{\"uuid\":\"%s\", \"appId\":\"10011\",\"fileNo\":\"%s\",\"version\":\"%d\"}"), *Uuid, *FileNo, Version);
	Request->SetContentAsString(JsonPayload);
	Request->OnProcessRequestComplete().BindUObject(this, &USwithModelFileVersionApi::OnResponseReceived);
	Request->ProcessRequest();
}

FString USwithModelFileVersionApi::PreprocessJsonString(const FString& JsonString)
{
	FString ProcessedString = JsonString;
	ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\""));
	return ProcessedString;
}

void USwithModelFileVersionApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		//FString ResponseContent = Response->GetContentAsString();
		// UE_LOG(LogTemp, Error, TEXT("SwithModelFileVersionApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
		FSwithModelFileVersionData ResponseData;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &ResponseData, 0, 0))
		{
			if (OnSwithModelFileVersionApiResponse.IsBound())
			{
				OnSwithModelFileVersionApiResponse.Execute(ResponseData);
			}
		}
		else
		{
			// UE_LOG(LogTemp, Error, TEXT("Failed to parse SwithModelFileVersionApi response"));
		}
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("Failed to send GetModelAssetLibraryTagListApi request"));
	}
}
