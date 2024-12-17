// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ModelLibrary/GetModelFileTagApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

void UGetModelFileTagApi::SendGetModelFileTagRequest(const FString& Ticket, const FString& FileNo, const FString& ProjectNo, FOnGetModelFileTagResponse InOnGetModelFileTagResponseDelegate)
{
    this->OnGetModelFileTagResponseDelegate = InOnGetModelFileTagResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    FString Url = FString::Printf(TEXT("https://api.meta.mg.xyz/spaceapi/model/tags/tags?fileNo=%s&projectNo=%s"), *FileNo, *ProjectNo);
    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Authorization"), Ticket);  
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->OnProcessRequestComplete().BindUObject(this, &UGetModelFileTagApi::OnResponseReceived);

    Request->ProcessRequest();
}

FString UGetModelFileTagApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UGetModelFileTagApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetModelFileTagApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        FModelFileTagData TagData;
        
        if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &TagData, 0, 0))
        {
            if (OnGetModelFileTagResponseDelegate.IsBound())
            {
                OnGetModelFileTagResponseDelegate.Execute(TagData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse GetModelFileTagApi response."));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send GetModelFileTagApi request."));
    }
}
