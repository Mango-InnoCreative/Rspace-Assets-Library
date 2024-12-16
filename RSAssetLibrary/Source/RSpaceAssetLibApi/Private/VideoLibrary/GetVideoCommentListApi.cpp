// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "VideoLibrary/GetVideoCommentListApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"

void UGetVideoCommentListApi::SendGetVideoCommentListRequest(const FString& Ticket, const FString& Uuid, const FString& AuditNo, FOnGetVideoCommentListResponse InResponseDelegate)
{
    OnResponseDelegate = InResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/video/auditComment/getAuditCommentList"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("authorization"), Ticket);
    
    FString JsonPayload = FString::Printf(TEXT("{\"uuid\":\"%s\",\"appId\":10011,\"auditNo\":\"%s\"}"), *Uuid, *AuditNo);
    Request->SetContentAsString(JsonPayload);
    Request->OnProcessRequestComplete().BindUObject(this, &UGetVideoCommentListApi::OnResponseReceived);
    Request->ProcessRequest();
}

FString UGetVideoCommentListApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UGetVideoCommentListApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        /**************/
        //// UE_LOG(LogTemp, Log, TEXT("GetVideoCommentListApi Response: %s"), *ResponseContent);
        FGetVideoCommentListResponseData CommentListResponse;
        if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &CommentListResponse, 0, 0))
        {
            if (OnResponseDelegate.IsBound())
            {
                OnResponseDelegate.Execute(CommentListResponse);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse GetVideoCommentListApi response to FGetVideoCommentListResponseData."));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send GetVideoCommentListApi request."));
    }
}
