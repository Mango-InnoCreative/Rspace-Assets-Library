// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ConceptDesignLibrary/GetConceptDesignPictureCommentApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

void UGetConceptDesignPictureCommentApi::SendGetConceptDesignPictureCommentRequest(
    const FString& Ticket, 
    const FString& Uuid, 
    int32 PicId, 
    int32 CurrentPage, 
    int32 PageSize, 
    FOnGetConceptDesignPictureCommentResponse InCommentResponseDelegate)
{
    this->OnCommentResponseDelegate = InCommentResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/project/painting/comment/findPaintingComment"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), Ticket);
    
    FString JsonPayload = FString::Printf(TEXT("{\"uuid\":\"%s\",\"appId\":10011,\"currentPage\":%d,\"pageSize\":%d,\"paintingId\":%d}"), 
                                          *Uuid, CurrentPage, PageSize, PicId);
    Request->SetContentAsString(JsonPayload);
    
    Request->OnProcessRequestComplete().BindUObject(this, &UGetConceptDesignPictureCommentApi::OnResponseReceived);

    Request->ProcessRequest();
}

FString UGetConceptDesignPictureCommentApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UGetConceptDesignPictureCommentApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetConceptDesignPictureCommentApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        UGetConceptDesignPictureCommentData* CommentData = NewObject<UGetConceptDesignPictureCommentData>();
        
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            CommentData->status = JsonObject->GetStringField("status");
            CommentData->code = JsonObject->GetStringField("code");
            CommentData->message = JsonObject->GetStringField("message");
            
            TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField("data");
            CommentData->total = DataObject->GetIntegerField("total");
            
            const TArray<TSharedPtr<FJsonValue>> ItemsArray = DataObject->GetArrayField("items");
            for (const TSharedPtr<FJsonValue>& ItemValue : ItemsArray)
            {
                const TSharedPtr<FJsonObject> ItemObject = ItemValue->AsObject();

                FPictureCommentItem CommentItem;
                CommentItem.id = ItemObject->GetIntegerField("id");
                CommentItem.paintingId = ItemObject->GetIntegerField("paintingId");
                CommentItem.commentUserNo = ItemObject->GetStringField("commentUserNo");
                CommentItem.commentContent = ItemObject->GetStringField("commentContent");
                CommentItem.commentImg = ItemObject->GetStringField("commentImg");
                CommentItem.createTime = ItemObject->GetStringField("createTime");
                CommentItem.updateTime = ItemObject->GetStringField("updateTime");
                CommentItem.memberName = ItemObject->GetStringField("memberName");
                CommentItem.memberAccount = ItemObject->GetStringField("memberAccount");
                CommentItem.memberAvatar = ItemObject->GetStringField("memberAvatar");
                CommentItem.memberId = ItemObject->GetStringField("memberId");
                
                CommentData->items.Add(CommentItem);
            }
            
            if (OnCommentResponseDelegate.IsBound())
            {
                OnCommentResponseDelegate.Execute(CommentData);
            }
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send GetConceptDesignPictureCommentApi request"));
    }
}
