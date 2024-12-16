// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ModelLibrary/GetModelFileHistoryApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

void UGetModelFileHistoryApi::SendGetModelFileHistoryRequest(const FString& FileNo, const FString& Ticket, FOnGetModelFileHistoryResponse InResponseDelegate)
{
  
    this->OnGetModelFileHistoryResponseDelegate = InResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    FString Url = FString::Printf(TEXT("https://api.meta.mg.xyz/spaceapi/model/history/files?fileNo=%s"), *FileNo);
    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Authorization"), Ticket);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->OnProcessRequestComplete().BindUObject(this, &UGetModelFileHistoryApi::OnResponseReceived);
    Request->ProcessRequest();
}

FString UGetModelFileHistoryApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UGetModelFileHistoryApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetModelFileHistoryApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());

        UGetModelFileHistoryResponseData* ModelFileHistoryData = NewObject<UGetModelFileHistoryResponseData>();

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            ModelFileHistoryData->status = JsonObject->GetStringField("status");
            ModelFileHistoryData->code = JsonObject->GetStringField("code");
            ModelFileHistoryData->message = JsonObject->GetStringField("message");

            const TArray<TSharedPtr<FJsonValue>> DataArray = JsonObject->GetArrayField("data");
            for (const TSharedPtr<FJsonValue>& ItemValue : DataArray)
            {
                const TSharedPtr<FJsonObject> ItemObject = ItemValue->AsObject();
                FModelFileHistoryItem Item;

                Item.id = ItemObject->GetIntegerField("id");
                Item.projectNo = ItemObject->GetStringField("projectNo");
                Item.fileNo = ItemObject->GetStringField("fileNo");
                Item.fileName = ItemObject->GetStringField("fileName");
                Item.filePath = ItemObject->GetStringField("filePath");
                Item.relativePath = ItemObject->GetStringField("relativePath");
                Item.version = ItemObject->GetIntegerField("version");
                Item.fileStatus = ItemObject->GetIntegerField("fileStatus");
                Item.remark = ItemObject->GetStringField("remark");
                Item.createTime = ItemObject->GetStringField("createTime");
                Item.createrBy = ItemObject->GetStringField("createrBy");
                Item.updateTime = ItemObject->GetStringField("updateTime");
                Item.updateBy = ItemObject->GetStringField("updateBy");

                ModelFileHistoryData->data.Add(Item);
            }

    
            if (OnGetModelFileHistoryResponseDelegate.IsBound())
            {
                OnGetModelFileHistoryResponseDelegate.Execute(ModelFileHistoryData);
            }
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send GetModelFileHistoryApi request"));
    }
}
