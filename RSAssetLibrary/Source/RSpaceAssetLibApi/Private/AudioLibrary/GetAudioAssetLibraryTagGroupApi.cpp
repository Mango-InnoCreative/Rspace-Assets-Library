// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "AudioLibrary/GetAudioAssetLibraryTagGroupApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"

void UGetAudioAssetLibraryTagGroupApi::SendGetAudioAssetLibraryTagGroupRequest(const FString& Ticket, const FString& Uuid, const FString& ProjectNo, FOnGetAudioAssetLibraryTagGroupResponse InResponseDelegate)
{
    this->OnResponseDelegate = InResponseDelegate;
    
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/audio/tagGroup/findTagGroupAllListByParam"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), Ticket);
    
    FString JsonPayload = FString::Printf(TEXT("{\"appId\":10011,\"projectNo\":\"%s\",\"uuid\":\"%s\"}"), *ProjectNo, *Uuid);
    Request->SetContentAsString(JsonPayload);
    
    Request->OnProcessRequestComplete().BindUObject(this, &UGetAudioAssetLibraryTagGroupApi::OnResponseReceived);
    
    Request->ProcessRequest();
}

FString UGetAudioAssetLibraryTagGroupApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UGetAudioAssetLibraryTagGroupApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetAudioAssetLibraryTagGroupApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        FGetAudioAssetLibraryTagGroupResponseData ResponseData;
        if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &ResponseData, 0, 0))
        {
            if (OnResponseDelegate.IsBound())
            {
                OnResponseDelegate.Execute(ResponseData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse GetAudioAssetLibraryTagGroupApi response"));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send GetAudioAssetLibraryTagGroupApi request"));
    }
}
