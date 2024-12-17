// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ModelLibrary/GetModelAssetLibraryTagListApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"

void UGetModelAssetLibraryTagListApi::SendGetModelAssetLibraryTagListRequest(const FString& Ticket, const FString& ProjectNo, FOnGetModelAssetLibraryTagListResponse InResponseDelegate)
{
    this->OnResponseDelegate = InResponseDelegate;
    
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    FString Url = FString::Printf(TEXT("https://api.open.mg.xyz/spaceapi/model/tags/listTags?projectNo=%s"), *ProjectNo);
    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Authorization"), Ticket);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->OnProcessRequestComplete().BindUObject(this, &UGetModelAssetLibraryTagListApi::OnResponseReceived);
    Request->ProcessRequest();
}

FString UGetModelAssetLibraryTagListApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\""));
    return ProcessedString;
}

void UGetModelAssetLibraryTagListApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetModelAssetLibraryTagListApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        FGetModelAssetLibraryTagListResponseData ResponseData;
        if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &ResponseData, 0, 0))
        {
            if (OnResponseDelegate.IsBound())
            {
                OnResponseDelegate.Execute(ResponseData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse GetModelAssetLibraryTagListApi response"));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send GetModelAssetLibraryTagListApi request"));
    }
}
