// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ConceptDesignLibrary/GetConceptDesignLibraryTagGroupApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

void UGetConceptDesignLibraryTagGroupApi::SendConceptDesignLibraryTagGroupRequest(const FString& Ticket, const FString& ProjectNo,const FString& Uuid, FOnGetConceptDesignLibraryTagGroupResponse InOnGetConceptDesignLibraryTagGroupResponseDelegate)
{
    this->OnGetConceptDesignLibraryTagGroupResponseDelegate = InOnGetConceptDesignLibraryTagGroupResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/project/painting/group/findListByParam"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("authorization"), Ticket);
    
    FString JsonPayload = FString::Printf(TEXT("{\"appId\":\"10011\",\"projectNo\":\"%s\",\"uuid\":\"%s\"}"), *ProjectNo, *Uuid);
    Request->SetContentAsString(JsonPayload);
    
    Request->OnProcessRequestComplete().BindUObject(this, &UGetConceptDesignLibraryTagGroupApi::OnResponseReceived);

    Request->ProcessRequest();
}

FString UGetConceptDesignLibraryTagGroupApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\""));
    return ProcessedString;
}

void UGetConceptDesignLibraryTagGroupApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        //// UE_LOG(LogTemp, Error, TEXT("GetConceptDesignLibraryTagGroupApi Response: %s"), *ResponseContent);
        
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            UGetConceptDesignLibraryTagGroupResponseData* ConceptDesignLibraryTagGroup = NewObject<UGetConceptDesignLibraryTagGroupResponseData>();
            ConceptDesignLibraryTagGroup->status = JsonObject->GetStringField("status");
            ConceptDesignLibraryTagGroup->code = JsonObject->GetStringField("code");
            ConceptDesignLibraryTagGroup->message = JsonObject->GetStringField("message");

            const TArray<TSharedPtr<FJsonValue>> ItemsArray = JsonObject->GetArrayField("data");

            for (const TSharedPtr<FJsonValue>& ItemValue : ItemsArray)
            {
                const TSharedPtr<FJsonObject> ItemObject = ItemValue->AsObject();

                FConceptDesignTagGroup TagGroup;
                TagGroup.id = ItemObject->GetIntegerField("id");
                TagGroup.projectNo = ItemObject->GetStringField("projectNo");
                TagGroup.groupName = ItemObject->GetStringField("groupName");
                TagGroup.createTime = ItemObject->GetStringField("createTime");
                TagGroup.updateTime = ItemObject->GetStringField("updateTime");
                TagGroup.remark = ItemObject->GetStringField("remark");
                ConceptDesignLibraryTagGroup->data.Add(TagGroup);
            }
            if (OnGetConceptDesignLibraryTagGroupResponseDelegate.IsBound())
            {
                OnGetConceptDesignLibraryTagGroupResponseDelegate.Execute(ConceptDesignLibraryTagGroup);
            }
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to Send ConceptDesignLibrary TagGroup Request"));
    }
}
