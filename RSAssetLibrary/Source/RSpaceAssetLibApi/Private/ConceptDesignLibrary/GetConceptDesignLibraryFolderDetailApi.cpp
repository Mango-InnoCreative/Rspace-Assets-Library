// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ConceptDesignLibrary/GetConceptDesignLibraryFolderDetailApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

void UGetConceptDesignLibraryFolderDetailApi::SendGetFolderDetailRequest(const FString& Ticket, const FString& Uuid, const FString& PaintingName, int32 FolderId, int32 CurrentPage, int32 PageSize, FString TagId,FOnGetConceptDesignFolderDetailResponse InFolderDetailResponseDelegate)
{
    this->OnFolderDetailResponseDelegate = InFolderDetailResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/project/painting/findFolderPaintingList"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("authorization"), Ticket); 
    
    FString JsonPayload = FString::Printf(TEXT("{\"appId\":10011,\"currentPage\":%d,\"pageSize\":%d,\"folderId\":%d,\"paintingName\":\"%s\",\"tagId\":\"%s\",\"uuid\":\"%s\"}"),
                                          CurrentPage, PageSize, FolderId, *PaintingName, *TagId, *Uuid);
    Request->SetContentAsString(JsonPayload);
    
    Request->OnProcessRequestComplete().BindUObject(this, &UGetConceptDesignLibraryFolderDetailApi::OnResponseReceived);

    Request->ProcessRequest();
}

FString UGetConceptDesignLibraryFolderDetailApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\""));
    return ProcessedString;
}

void UGetConceptDesignLibraryFolderDetailApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetConceptDesignLibraryFolderDetailApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            UGetConceptDesignLibraryFolderDetailData* FolderDetailData = NewObject<UGetConceptDesignLibraryFolderDetailData>();
            
            FolderDetailData->Status = JsonObject->GetStringField("status");
            FolderDetailData->Code = JsonObject->GetStringField("code");
            FolderDetailData->Message = JsonObject->GetStringField("message");
            
            FolderDetailData->Total = JsonObject->GetObjectField("data")->GetIntegerField("total");

            // 解析items
            const TArray<TSharedPtr<FJsonValue>> ItemsArray = JsonObject->GetObjectField("data")->GetArrayField("items");

            for (const TSharedPtr<FJsonValue>& ItemValue : ItemsArray)
            {
                const TSharedPtr<FJsonObject> ItemObject = ItemValue->AsObject();

                FConceptDesignFileItem FileItem;
                FileItem.Id = ItemObject->GetIntegerField("id");
                FileItem.UserNo = ItemObject->GetStringField("userNo");
                FileItem.Name = ItemObject->GetStringField("name");
                FileItem.RelativePatch = ItemObject->GetStringField("relativePatch");
                FileItem.ThumRelativePatch = ItemObject->GetStringField("thumRelativePatch");
                FileItem.DeleteStatus = ItemObject->GetIntegerField("deleteStatus");
                FileItem.FileLength = ItemObject->GetIntegerField("fileLength");
                FileItem.FileSize = ItemObject->GetStringField("fileSize");
                FileItem.FileSuffix = ItemObject->GetStringField("fileSuffix");
                FileItem.FileMd5 = ItemObject->GetStringField("fileMd5");
                FileItem.FileMd5GetStatus = ItemObject->GetIntegerField("fileMd5GetStatus");
                FileItem.CreateTime = ItemObject->GetStringField("createTime");
                FileItem.UpdateTime = ItemObject->GetStringField("updateTime");
                FileItem.ProjectNo = ItemObject->GetStringField("projectNo");

          
                FolderDetailData->Items.Add(FileItem);
            }

          
            if (OnFolderDetailResponseDelegate.IsBound())
            {
                OnFolderDetailResponseDelegate.Execute(FolderDetailData);
            }
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to receive GetConceptDesignLibraryFolderDetailApi response"));
    }
}
