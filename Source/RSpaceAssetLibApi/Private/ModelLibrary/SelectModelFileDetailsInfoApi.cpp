// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ModelLibrary/SelectModelFileDetailsInfoApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

void USelectModelFileDetailsInfoApi::SendSelectModelFileDetailsInfoRequest(const FString& Ticket, const FString& FileNo, FOnSelectModelFileDetailsInfoResponse InResponseDelegate)
{
    this->OnSelectModelFileDetailsInfoResponseDelegate = InResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    FString Url = FString::Printf(TEXT("https://api.meta.mg.xyz/spaceapi/space/project/model/folder/selectModelFileDetailsInfo?fileNo=%s"), *FileNo);
    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Authorization"), Ticket);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    
    Request->OnProcessRequestComplete().BindUObject(this, &USelectModelFileDetailsInfoApi::OnResponseReceived);

    Request->ProcessRequest();
}

FString USelectModelFileDetailsInfoApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\""));
    return ProcessedString;
}

void USelectModelFileDetailsInfoApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("SelectModelFileDetailsInfoApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            USelectModelFileDetailsInfoData* ModelFileDetailsData = NewObject<USelectModelFileDetailsInfoData>();

            ModelFileDetailsData->status = JsonObject->GetStringField("status");
            ModelFileDetailsData->code = JsonObject->GetStringField("code");
            ModelFileDetailsData->message = JsonObject->GetStringField("message");

            const TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField("data");
            FModelFileDetails& FileDetails = ModelFileDetailsData->data;

            FileDetails.id = DataObject->GetIntegerField("id");
            FileDetails.parentId = DataObject->GetIntegerField("parentId");
            FileDetails.fileNo = DataObject->GetStringField("fileNo");
            FileDetails.fileName = DataObject->GetStringField("fileName");
            FileDetails.filePath = DataObject->GetStringField("filePath");
            FileDetails.relativePath = DataObject->GetStringField("relativePath");
            FileDetails.fileSize = DataObject->GetIntegerField("fileSize");
            FileDetails.fileMd5 = DataObject->GetStringField("fileMd5");
            FileDetails.version = DataObject->GetIntegerField("version");
            FileDetails.createTime = DataObject->GetStringField("createTime");
            FileDetails.createrBy = DataObject->GetStringField("createrBy");
            FileDetails.updateTime = DataObject->GetStringField("updateTime");
            FileDetails.updateBy = DataObject->GetStringField("updateBy");
            FileDetails.gifUrl = DataObject->GetStringField("gifUrl");
            FileDetails.gifFirstImg = DataObject->GetStringField("gifFirstImg");
            
            if (OnSelectModelFileDetailsInfoResponseDelegate.IsBound())
            {
                OnSelectModelFileDetailsInfoResponseDelegate.Execute(ModelFileDetailsData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response for SelectModelFileDetailsInfoApi."));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send SelectModelFileDetailsInfoApi request"));
    }
}
