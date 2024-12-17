// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ConceptDesignLibrary/GetConceptDesignLibraryTagListApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

void UGetConceptDesignLibraryTagListApi::SendConceptDesignLibraryTagListRequest(const FString& Ticket, const FString& ProjectNo, int32& Type, const FString& Uuid, FOnGetConceptDesignLibraryTagListResponse InOnGetConceptDesignLibraryTagListResponseDelegate)
{
    this->OnGetConceptDesignLibraryTagListResponseDelegate = InOnGetConceptDesignLibraryTagListResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/project/painting/tag/findListByParam"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("authorization"), Ticket);
    
    FString JsonPayload = FString::Printf(TEXT("{\"uuid\":\"%s\", \"appId\":\"10011\",\"projectNo\":\"%s\",\"type\":\"%d\"}"), *Uuid, *ProjectNo, Type);
    Request->SetContentAsString(JsonPayload);
    
    Request->OnProcessRequestComplete().BindUObject(this, &UGetConceptDesignLibraryTagListApi::OnResponseReceived);

    Request->ProcessRequest();
}

FString UGetConceptDesignLibraryTagListApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\""));
    return ProcessedString;
}

void UGetConceptDesignLibraryTagListApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        //// UE_LOG(LogTemp, Error, TEXT("GetConceptDesignLibraryTagListApi Response: %s"), *ResponseContent);
        
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            UGetConceptDesignLibraryTagListResponseData* ConceptDesignLibraryTagList = NewObject<UGetConceptDesignLibraryTagListResponseData>();
            ConceptDesignLibraryTagList->status = JsonObject->GetStringField("status");
            ConceptDesignLibraryTagList->code = JsonObject->GetStringField("code");
            ConceptDesignLibraryTagList->message = JsonObject->GetStringField("message");

            const TArray<TSharedPtr<FJsonValue>> ItemsArray = JsonObject->GetArrayField("data");

            for (const TSharedPtr<FJsonValue>& ItemValue : ItemsArray)
            {
                const TSharedPtr<FJsonObject> ItemObject = ItemValue->AsObject();

                FConceptDesignFileItemTagList TagList;
                TagList.id = ItemObject->GetIntegerField("id");
                TagList.userNo = ItemObject->GetStringField("userNo");
                TagList.tagName = ItemObject->GetStringField("tagName");
                TagList.tagCount = ItemObject->GetIntegerField("tagCount");
                TagList.projectNo = ItemObject->GetStringField("projectNo");
                TagList.commonStatus = ItemObject->GetIntegerField("commonStatus");
                TagList.createTime = ItemObject->GetStringField("createTime");
                TagList.updateTime = ItemObject->GetStringField("updateTime");
                TagList.remark = ItemObject->GetStringField("remark");
          
                const TArray<TSharedPtr<FJsonValue>> GroupArray = ItemObject->GetArrayField("groups");
                for (const TSharedPtr<FJsonValue>& GroupValue : GroupArray)
                {
                    const TSharedPtr<FJsonObject> GroupObject = GroupValue->AsObject();
                    if (GroupObject.IsValid())
                    {
                        FGroups Group;
                        //const TSharedPtr<FJsonObject> GroupObject = GroupValue->AsObject();
                        Group.id = GroupObject->GetIntegerField("id");
                        Group.remark = GroupObject->GetStringField("remark");
                        Group.createTime = GroupObject->GetStringField("createTime");
                        Group.groupName = GroupObject->GetStringField("groupName");
                        Group.projectNo = GroupObject->GetStringField("projectNo");
                        Group.updateTime = GroupObject->GetStringField("updateTime");
                        
                        TagList.groups.Add(Group);
                    }
                }
                ConceptDesignLibraryTagList->data.Add(TagList);
            }
            
            if (OnGetConceptDesignLibraryTagListResponseDelegate.IsBound())
            {
                OnGetConceptDesignLibraryTagListResponseDelegate.Execute(ConceptDesignLibraryTagList);
            }
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to Send ConceptDesignLibrary TagList Request"));
    }
}
