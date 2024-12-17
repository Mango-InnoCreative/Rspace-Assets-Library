// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "AudioLibrary/GetAudioAssetLibraryTagListApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"

void UGetAudioAssetLibraryTagListApi::SendGetAudioAssetLibraryTagListRequest(const FString& Ticket, const FString& Uuid, const FString& ProjectNo, int32 GroupStatus, FOnGetAudioAssetLibraryTagListResponse InOnResponseDelegate)
{
    this->OnResponseDelegate = InOnResponseDelegate;
    
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/audio/tag/findAllListByParam"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), Ticket);
    
    FString JsonPayload = FString::Printf(TEXT("{\"uuid\":\"%s\",\"appId\":10011,\"projectNo\":\"%s\",\"groupStatus\":%d}"), *Uuid, *ProjectNo, GroupStatus);
    Request->SetContentAsString(JsonPayload);
    Request->OnProcessRequestComplete().BindUObject(this, &UGetAudioAssetLibraryTagListApi::OnResponseReceived);
    Request->ProcessRequest();
}

FString UGetAudioAssetLibraryTagListApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\""));
    return ProcessedString;
}

void UGetAudioAssetLibraryTagListApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
       // FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetAudioAssetLibraryTagListApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        FGetAudioAssetLibraryTagListResponseData ResponseData;
        if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &ResponseData, 0, 0))
        {
            if (OnResponseDelegate.IsBound())
            {
                OnResponseDelegate.Execute(ResponseData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse GetAudioAssetLibraryTagListApi response"));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send GetAudioAssetLibraryTagListApi request"));
    }
}
