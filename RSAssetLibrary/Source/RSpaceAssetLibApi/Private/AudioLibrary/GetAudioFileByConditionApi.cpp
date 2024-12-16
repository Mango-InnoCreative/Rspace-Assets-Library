// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "AudioLibrary/GetAudioFileByConditionApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

void UGetAudioFileByConditionApi::SendGetAudioFileByConditionRequest(
    const FString& Ticket,
    const FString& Uuid,
    const FString& ProjectNo,
    const FString& GroupId, 

    const FString& AudioChannel, 
    const FString& AudioHarvestBits, 
    const FString& AudioHarvestRate, 
    const int32& BpmBegin, 
    const int32& BpmEnd, 
    const int32& CurrentPage,
    const FString& FileFormat, 
    const int32& MenuType, 
    const int32& PageSize, 
    const FString& Search, 
    const FString& Sort, 
    const FString& SortType, 
    const int64& TagId, 
    FOnGetAudioFileByConditionResponse InResponseDelegate)
{
    this->OnResponseDelegate = InResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/audio/file/getList"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), Ticket);

    FString JsonPayload = FString::Printf(
        TEXT("{\"uuid\":\"%s\",\"appId\":10011,\"audioChannel\":\"%s\",\"audioHarvestBits\":\"%s\",\"audioHarvestRate\":\"%s\",\"bpmBegin\":\"%d\",\"bpmEnd\":\"%d\",\"currentPage\":\"%d\",\"fileFormat\":\"%s\",\"groupId\":\"%s\",\"menuType\":\"%d\",\"pageSize\":\"%d\",\"search\":\"%s\",\"sortType\":\"%s\",\"sort\":\"%s\",\"projectNo\":\"%s\",\"tagId\":\"%lld\"}"),
        *Uuid, *AudioChannel,*AudioHarvestBits, *AudioHarvestRate, BpmBegin, BpmEnd, CurrentPage, *FileFormat, *GroupId, MenuType, PageSize, *Search, *SortType, *Sort,*ProjectNo, TagId);
    
    Request->SetContentAsString(JsonPayload);

    Request->OnProcessRequestComplete().BindUObject(this, &UGetAudioFileByConditionApi::OnResponseReceived);

    Request->ProcessRequest();
}

FString UGetAudioFileByConditionApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UGetAudioFileByConditionApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetAudioFileByConditionApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        FGetAudioFileByConditionResponse ApiResponse;

        if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &ApiResponse, 0, 0))
        {
            if (OnResponseDelegate.IsBound())
            {
                OnResponseDelegate.Execute(ApiResponse);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse GetAudioFileByConditionApi response to struct."));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to receive GetAudioFileByConditionApi response."));
    }
}
