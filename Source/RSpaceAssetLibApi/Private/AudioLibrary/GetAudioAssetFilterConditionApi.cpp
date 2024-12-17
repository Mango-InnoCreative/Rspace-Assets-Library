// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "AudioLibrary/GetAudioAssetFilterConditionApi.h"
#include "HttpModule.h"
#include "Json.h"
#include "JsonUtilities.h"

void UGetAudioAssetFilterConditionApi::SendGetAudioAssetFilterConditionRequest(const FString& Ticket, FOnGetAudioAssetFilterConditionResponse InResponseDelegate)
{
	this->OnResponseDelegate = InResponseDelegate;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/audio/file/getComboBox/audio_manage"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), Ticket);

	Request->OnProcessRequestComplete().BindUObject(this, &UGetAudioAssetFilterConditionApi::OnResponseReceived);
	Request->ProcessRequest();
}

FString UGetAudioAssetFilterConditionApi::PreprocessJsonString(const FString& JsonString)
{
	FString ProcessedString = JsonString;
	ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
	return ProcessedString;
}

void UGetAudioAssetFilterConditionApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		//FString ResponseContent = Response->GetContentAsString();
		//// UE_LOG(LogTemp, Log, TEXT("GetAudioAssetFilterConditionApi Response: %s"), *ResponseContent);
		FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
		FGetAudioAssetFilterConditionResponse AudioFilterConditionResponse;

		if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &AudioFilterConditionResponse, 0, 0))
		{
			if (OnResponseDelegate.IsBound())
			{
				OnResponseDelegate.Execute(AudioFilterConditionResponse);
			}
		}
		else
		{
			// UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response to FGetAudioAssetFilterConditionResponse."));
		}
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("Failed to send GetAudioAssetFilterCondition request."));
	}
}
