// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "AudioLibrary/GetAudioCommentApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

void UGetAudioCommentApi::SendGetAudioCommentRequest(const FString& Ticket, const FString& Uuid, const FString& FileNo, int32 CurrentPage, int32 PageSize, FOnGetAudioCommentResponse InResponseDelegate)
{
    this->OnGetAudioCommentResponseDelegate = InResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/audio/file/comment/getList"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), Ticket);
    
    FString JsonPayload = FString::Printf(TEXT("{\"uuid\":\"%s\",\"appId\":10011,\"currentPage\":%d,\"fileNo\":\"%s\",\"pageSize\":%d}"), *Uuid, CurrentPage, *FileNo, PageSize);
    Request->SetContentAsString(JsonPayload);
    
    Request->OnProcessRequestComplete().BindUObject(this, &UGetAudioCommentApi::OnResponseReceived);

    Request->ProcessRequest();
}

FString UGetAudioCommentApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UGetAudioCommentApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetAudioCommentApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            UGetAudioCommentResponseData* CommentData = NewObject<UGetAudioCommentResponseData>();
            
            CommentData->Status = JsonObject->GetStringField("status");
            CommentData->Code = JsonObject->GetStringField("code");
            CommentData->Message = JsonObject->GetStringField("message");
            
            TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField("data");
            CommentData->CurPage = DataObject->GetIntegerField("curPage");
            CommentData->Total = DataObject->GetIntegerField("total");
            CommentData->TotalPage = DataObject->GetIntegerField("totalPage");
            CommentData->Limit = DataObject->GetIntegerField("limit");
            CommentData->LastPage = DataObject->GetBoolField("lastPage");

            const TArray<TSharedPtr<FJsonValue>> DataListArray = DataObject->GetArrayField("dataList");
            for (const TSharedPtr<FJsonValue>& Value : DataListArray)
            {
                TSharedPtr<FJsonObject> ItemObject = Value->AsObject();
                FAudioCommentItem CommentItem;

                CommentItem.AppId = ItemObject->GetStringField("appId");
                CommentItem.Uuid = ItemObject->GetStringField("uuid");
                CommentItem.CurrentPage = ItemObject->GetIntegerField("currentPage");
                CommentItem.PageSize = ItemObject->GetIntegerField("pageSize");
                CommentItem.FileNo = ItemObject->GetStringField("fileNo");
                CommentItem.Content = ItemObject->GetStringField("content");
                CommentItem.MemberUuid = ItemObject->GetStringField("memberUuid");
                CommentItem.MemberName = ItemObject->GetStringField("memberName");
                CommentItem.CreateTime = ItemObject->GetStringField("createTime");
                CommentItem.UserAvatar = ItemObject->GetStringField("userAvatar");

                CommentData->DataList.Add(CommentItem);
            }
            
            if (OnGetAudioCommentResponseDelegate.IsBound())
            {
                OnGetAudioCommentResponseDelegate.Execute(CommentData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response in GetAudioCommentApi"));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to receive GetAudioCommentApi response"));
    }
}

