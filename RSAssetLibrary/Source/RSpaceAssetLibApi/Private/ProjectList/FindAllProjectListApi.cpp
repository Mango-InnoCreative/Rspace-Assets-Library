// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectList/FindAllProjectListApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

void UFindAllProjectListApi::SendFindAllProjectListRequest(const FString& Ticket, const FString& Uuid, FOnFindAllProjectListResponse InFindAllProjectListResponseDelegate)
{
    // 将委托赋值给成员变量
    this->OnFindAllProjectListResponseDelegate = InFindAllProjectListResponseDelegate;

    // 构造 HTTP 请求
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/project/info/findAllProjectList"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("authorization"), Ticket);

    // 构造 JSON payload
    FString JsonPayload = FString::Printf(TEXT("{\"appId\":\"10011\",\"uuid\":\"%s\"}"), *Uuid);
    Request->SetContentAsString(JsonPayload);

    // 绑定响应处理器
    Request->OnProcessRequestComplete().BindUObject(this, &UFindAllProjectListApi::OnResponseReceived);

    Request->ProcessRequest();
}

FString UFindAllProjectListApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

// 处理接口响应
void UFindAllProjectListApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        // // UE_LOG(LogTemp, Log, TEXT("FindAllProjectListApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            UFindAllProjectListResponseData* ProjectListData = NewObject<UFindAllProjectListResponseData>();

            ProjectListData->status = JsonObject->GetStringField("status");
            ProjectListData->code = JsonObject->GetStringField("code");
            ProjectListData->message = JsonObject->GetStringField("message");

            //const TArray<TSharedPtr<FJsonValue>> ItemsArray = JsonObject->GetArrayField("data");

            if (JsonObject->HasTypedField<EJson::Array>("data"))
            {
                const TArray<TSharedPtr<FJsonValue>> ItemsArray = JsonObject->GetArrayField("data");
                for (const TSharedPtr<FJsonValue>& ItemValue : ItemsArray)
                {
                    const TSharedPtr<FJsonObject> ItemObject = ItemValue->AsObject();

                    FAllProjectItem ProjectItem;
                    ProjectItem.projectNo = ItemObject->GetStringField("projectNo");
                    ProjectItem.dramaNo = ItemObject->GetStringField("dramaNo");
                    ProjectItem.projectName = ItemObject->GetStringField("projectName");
                    ProjectItem.projectImg = ItemObject->GetStringField("projectImg");

                    ProjectListData->data.Add(ProjectItem);
                }
            }
            else
            {
                //// UE_LOG(LogTemp, Warning, TEXT("还没有完成登录"));
            }
            

            // for (const TSharedPtr<FJsonValue>& ItemValue : ItemsArray)
            // {
            //     const TSharedPtr<FJsonObject> ItemObject = ItemValue->AsObject();
            //
            //     FAllProjectItem ProjectItem;
            //     ProjectItem.projectNo = ItemObject->GetStringField("projectNo");
            //     ProjectItem.dramaNo = ItemObject->GetStringField("dramaNo");
            //     ProjectItem.projectName = ItemObject->GetStringField("projectName");
            //     ProjectItem.projectImg = ItemObject->GetStringField("projectImg");
            //
            //     ProjectListData->data.Add(ProjectItem);
            // }


            if (OnFindAllProjectListResponseDelegate.IsBound())
            {
                OnFindAllProjectListResponseDelegate.Execute(ProjectListData);
            }
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send FindAllProjectListApi request"));
    }
}
