// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectList/FindProjectListApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"

void UFindProjectListApi::SendFindProjectListRequest(const FString& Ticket, const FString& Uuid, FOnFindProjectListResponse InFindProjectListResponseDelegate)
{
    this->OnFindProjectListResponseDelegate = InFindProjectListResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/project/info/findProjectList"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("authorization"), Ticket);

    FString JsonPayload = FString::Printf(TEXT("{\"appId\":\"10011\",\"currentPage\":1,\"pageSize\":100,\"uuid\":\"%s\"}"), *Uuid);
    Request->SetContentAsString(JsonPayload);

    Request->OnProcessRequestComplete().BindUObject(this, &UFindProjectListApi::OnResponseReceived);
    Request->ProcessRequest();
}

bool operator==(const FString& Lhs, int RHS);

FString UFindProjectListApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\""));
    return ProcessedString;
}

void UFindProjectListApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        // // UE_LOG(LogTemp, Log, TEXT("FindProjectListApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            UFindProjectListResponseData* ProjectListData = NewObject<UFindProjectListResponseData>();

            ProjectListData->status = JsonObject->GetStringField("status");
            ProjectListData->code = JsonObject->GetStringField("code");
            ProjectListData->message = JsonObject->GetStringField("message");

            if (JsonObject->GetStringField("code") == "200")
            {
                TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField("data");
                ProjectListData->data.total = DataObject->GetIntegerField("total");
                
                const TArray<TSharedPtr<FJsonValue>> ItemsArray = DataObject->GetArrayField("items");
                for (const TSharedPtr<FJsonValue>& ItemValue : ItemsArray)
                {
                    const TSharedPtr<FJsonObject> ItemObject = ItemValue->AsObject();

                    int32 ProjectID = ItemObject->GetIntegerField("id");
                    FString ProjectNo = ItemObject->GetStringField("projectNo");
                    FString ProjectName = ItemObject->GetStringField("projectName");
                    FString ProjectSvn = ItemObject->GetStringField("projectSvn");
                    FString ProjectImg = ItemObject->GetStringField("projectImg");
                    int32 ProjectStatus = ItemObject->GetIntegerField("projectStatus");
                    FString CreateTime = ItemObject->GetStringField("createTime");
                    FString UpdateTime = ItemObject->GetStringField("updateTime");
                    FString ProjectDescribe = ItemObject->GetStringField("projectDescribe");
                    TArray<FAdminInfo> AdminList;
                    const TArray<TSharedPtr<FJsonValue>> AdminArray = ItemObject->GetArrayField("adminList");
                    for (const TSharedPtr<FJsonValue>& AdminValue : AdminArray)
                    {
                        const TSharedPtr<FJsonObject> AdminObject = AdminValue->AsObject();

                        FAdminInfo AdminInfo;
                        AdminInfo.id = AdminObject->GetIntegerField("id");
                        AdminInfo.userUuid = AdminObject->GetStringField("userUuid");
                        AdminInfo.userLevel = AdminObject->GetIntegerField("userLevel");
                        AdminInfo.createTime = AdminObject->GetStringField("createTime");
                        AdminInfo.updateTime = AdminObject->GetStringField("updateTime");
                        AdminInfo.userPhone = AdminObject->GetStringField("userPhone");
                        AdminInfo.userMail = AdminObject->GetStringField("userMail");
                        AdminInfo.userName = AdminObject->GetStringField("userName");
                        AdminInfo.userAccount = AdminObject->GetStringField("userAccount");
                        AdminInfo.userAvatar = AdminObject->GetStringField("userAvatar");

                        AdminList.Add(AdminInfo);
                    }
           
                    ProjectListData->AddProjectItem(ProjectID, ProjectNo, ProjectName, ProjectSvn, ProjectImg, ProjectStatus, CreateTime, UpdateTime, ProjectDescribe, AdminList);
                }
            }
            else
            {
                //// UE_LOG(LogTemp, Error, TEXT("登录还没成功！！！！！！！！！~~~~~~~~~~~~~~~~~~~~~"));
            }

     
            if (OnFindProjectListResponseDelegate.IsBound())
            {
                OnFindProjectListResponseDelegate.Execute(ProjectListData);
            }
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send FindProjectListApi request"));
    }
}
