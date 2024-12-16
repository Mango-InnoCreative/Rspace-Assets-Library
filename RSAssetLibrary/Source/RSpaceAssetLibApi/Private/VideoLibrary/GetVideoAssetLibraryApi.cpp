// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "VideoLibrary/GetVideoAssetLibraryApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"


void UGetVideoAssetLibraryApi::SendGetVideoAssetLibraryRequest(const FString& Ticket, const FString& Uuid, const FString& ProjectNo, const FString& FileNo, FOnGetVideoAssetLibraryResponse InResponseDelegate)
{
    this->OnGetVideoAssetLibraryResponseDelegate = InResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/video/file/selectFileFolderInfo"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), Ticket);


    FString JsonPayload = FString::Printf(TEXT("{\"uuid\":\"%s\",\"appId\":10011,\"fileNo\":\"%s\",\"projectNo\":\"%s\"}"), *Uuid, *FileNo, *ProjectNo);
    Request->SetContentAsString(JsonPayload);

    Request->OnProcessRequestComplete().BindUObject(this, &UGetVideoAssetLibraryApi::OnResponseReceived);

    Request->ProcessRequest();
}

FString UGetVideoAssetLibraryApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UGetVideoAssetLibraryApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetVideoAssetLibraryApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
  
        FGetVideoAssetLibraryResponseData* ResponseData = new FGetVideoAssetLibraryResponseData();

        if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, ResponseData, 0, 0))
        {
            
            if (OnGetVideoAssetLibraryResponseDelegate.IsBound())
            {
                OnGetVideoAssetLibraryResponseDelegate.Execute(ResponseData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse GetVideoAssetLibraryApi response to struct."));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send GetVideoAssetLibraryApi request."));
    }
}

