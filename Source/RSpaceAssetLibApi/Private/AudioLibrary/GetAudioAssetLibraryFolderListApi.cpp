// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "AudioLibrary/GetAudioAssetLibraryFolderListApi.h"
#include "HttpModule.h"
#include "Json.h"
#include "JsonObjectConverter.h"

void UGetAudioAssetLibraryFolderListApi::SendGetAudioAssetLibraryFolderListRequest(const FString& Ticket, const FString& Uuid, const FString& ProjectNo, const FString& GroupName, FOnGetAudioAssetLibraryFolderListResponse InResponseDelegate)
{
    this->OnResponseDelegate = InResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/audio/group/findAllListByParam"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), Ticket);
    
    TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject);
    RequestBody->SetStringField(TEXT("uuid"), Uuid);
    RequestBody->SetStringField(TEXT("appId"), TEXT("10011"));
    RequestBody->SetStringField(TEXT("groupName"), GroupName);
    RequestBody->SetStringField(TEXT("projectNo"), ProjectNo);

    FString RequestBodyString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBodyString);
    FJsonSerializer::Serialize(RequestBody.ToSharedRef(), Writer);

    Request->SetContentAsString(RequestBodyString);
    Request->OnProcessRequestComplete().BindUObject(this, &UGetAudioAssetLibraryFolderListApi::OnResponseReceived);
    Request->ProcessRequest();
}

FString UGetAudioAssetLibraryFolderListApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UGetAudioAssetLibraryFolderListApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetAudioAssetLibraryFolderListApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        FGetAudioAssetLibraryFolderListData* ParsedData = new FGetAudioAssetLibraryFolderListData;
        
        if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, ParsedData, 0, 0))
        {
            if (OnResponseDelegate.IsBound())
            {
                OnResponseDelegate.Execute(ParsedData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse response to FGetAudioAssetLibraryFolderListData"));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send request or receive valid response for GetAudioAssetLibraryFolderListApi"));
    }
}
