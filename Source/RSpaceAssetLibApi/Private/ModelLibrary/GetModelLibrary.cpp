// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ModelLibrary/GetModelLibrary.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

FCriticalSection ModelActiveRequestsLock;
TSet<FString> ModelActiveRequests;

void UGetModelLibrary::SendGetModelLibraryRequest(const FString& Ticket, const FString& Uuid, int32& FileId, const FString& ProjectNo, const FString& fileName, const int64& tagId, const FString& tagName, FOnGetModelLibraryResponse InOnGetModelLibraryResponseDelegate)
{
    this->OnGetModelLibraryResponseDelegate = InOnGetModelLibraryResponseDelegate;

    FString RequestKey = FString::Printf(TEXT("%s-%d"), *Uuid, FileId);

    {
        FScopeLock Lock(&ModelActiveRequestsLock);
        if (ModelActiveRequests.Contains(RequestKey))
        {
            UE_LOG(LogTemp, Warning, TEXT("Duplicate request detected, skipping: %s"), *RequestKey);
            return;
        }
 
        ModelActiveRequests.Add(RequestKey);
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/project/model/folder/getProjectModelFolderInnerListByParam"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("authorization"), Ticket);

    FString JsonPayload = FString::Printf(TEXT("{\"appId\":\"10011\",\"fileId\":\"%d\",\"fileName\":\"%s\",\"projectNo\":\"%s\",\"tagId\":\"%lld\",\"tagName\":\"%s\",\"uuid\":\"%s\"}"), FileId, *fileName, *ProjectNo, tagId, *tagName, *Uuid);
    Request->SetContentAsString(JsonPayload);

    Request->OnProcessRequestComplete().BindUObject(this, &UGetModelLibrary::OnResponseReceivedWithCleanup, RequestKey);

    Request->ProcessRequest();
}

void UGetModelLibrary::OnResponseReceivedWithCleanup(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString RequestKey)
{
  
    {
        FScopeLock Lock(&ModelActiveRequestsLock);
        ModelActiveRequests.Remove(RequestKey);
    }

 
    OnResponseReceived(Request, Response, bWasSuccessful);
}

FString UGetModelLibrary::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UGetModelLibrary::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            UGetModelLibraryResponseData* ModelLibraryData = NewObject<UGetModelLibraryResponseData>();
            ModelLibraryData->status = JsonObject->GetStringField("status");
            ModelLibraryData->code = JsonObject->GetStringField("code");
            ModelLibraryData->message = JsonObject->GetStringField("message");

            const TArray<TSharedPtr<FJsonValue>> ItemsArray = JsonObject->GetArrayField("data");

            for (const TSharedPtr<FJsonValue>& ItemValue : ItemsArray)
            {
                const TSharedPtr<FJsonObject> ItemObject = ItemValue->AsObject();

                FModelFileItem FileItem;
                FileItem.id = ItemObject->GetIntegerField("id");
                FileItem.parentId = ItemObject->GetIntegerField("parentId");
                FileItem.uuid = ItemObject->GetStringField("uuid");
                FileItem.projectNo = ItemObject->GetStringField("projectNo");
                FileItem.fileNo = ItemObject->GetStringField("fileNo");
                FileItem.fileName = ItemObject->GetStringField("fileName");
                FileItem.filePath = ItemObject->GetStringField("filePath");
                FileItem.thumRelativePath = ItemObject->GetStringField("thumRelativePath");
                FileItem.relativePath = ItemObject->GetStringField("relativePath");
                FileItem.fileSize = ItemObject->GetIntegerField("fileSize");
                FileItem.fileMd5 = ItemObject->GetStringField("fileMd5");
                FileItem.fileType = ItemObject->GetIntegerField("fileType");
                FileItem.fileStatus = ItemObject->GetIntegerField("fileStatus");
                FileItem.version = ItemObject->GetIntegerField("version");
                FileItem.remark = ItemObject->GetStringField("remark");
                FileItem.gifUrl = ItemObject->GetStringField("gifUrl");
                FileItem.gifFirstImg = ItemObject->GetStringField("gifFirstImg");
                FileItem.createTime = ItemObject->GetStringField("createTime");
                FileItem.createrBy = ItemObject->GetStringField("createrBy");
                FileItem.updateTime = ItemObject->GetStringField("updateTime");
                FileItem.updateBy = ItemObject->GetStringField("updateBy");

                ModelLibraryData->data.Add(FileItem);
            }


            if (OnGetModelLibraryResponseDelegate.IsBound())
            {
                OnGetModelLibraryResponseDelegate.Execute(ModelLibraryData);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to send GetModelLibrary request"));
    }
}
