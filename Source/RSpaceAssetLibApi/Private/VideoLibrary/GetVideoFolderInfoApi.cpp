// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "VideoLibrary/GetVideoFolderInfoApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"


void UGetVideoFolderInfoApi::SendGetVideoFolderInfoRequest(const FString& Ticket, const FString& Uuid, const FString& FileNo, const FString& ProjectNo, FOnGetVideoFolderInfoResponse InResponseDelegate)
{

    OnResponseDelegate = InResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/video/file/selectFileFolderInfo"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), Ticket);

    TSharedPtr<FJsonObject> RequestJson = MakeShareable(new FJsonObject());
    RequestJson->SetStringField(TEXT("uuid"), Uuid);
    RequestJson->SetStringField(TEXT("appId"), TEXT("10011"));
    RequestJson->SetStringField(TEXT("fileNo"), FileNo);
    RequestJson->SetStringField(TEXT("projectNo"), ProjectNo);
    
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RequestJson.ToSharedRef(), Writer);

    
    Request->SetContentAsString(RequestBody);

   
    Request->OnProcessRequestComplete().BindUObject(this, &UGetVideoFolderInfoApi::OnResponseReceived);
    
  
    Request->ProcessRequest();
}

FString UGetVideoFolderInfoApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}


void UGetVideoFolderInfoApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetVideoFolderInfoApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
 
        FGetVideoFolderInfoData* FolderInfoData = new FGetVideoFolderInfoData();


        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField("data");

            FolderInfoData->FileNo = DataObject->GetStringField("fileNo");
            FolderInfoData->FileName = DataObject->GetStringField("fileName");
            FolderInfoData->Uuid = DataObject->GetStringField("uuid");
            FolderInfoData->CreateUserName = DataObject->GetStringField("createUserName");
            FolderInfoData->SpaceSize = DataObject->GetStringField("spaceSize");
            FolderInfoData->CreateTime = DataObject->GetStringField("createTime");

        
            if (OnResponseDelegate.IsBound())
            {
                OnResponseDelegate.Execute(FolderInfoData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse GetVideoFolderInfoApi response JSON"));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send GetVideoFolderInfoApi request"));
    }
}
