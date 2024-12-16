// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ConceptDesignLibrary/GetConceptDesignLibraryApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonObjectConverter.h"

// 发送请求
void UGetConceptDesignLibraryApi::SendGetConceptDesignLibraryRequest(const FString& Ticket, const FString& Uuid, const FString& ProjectNo, FOnGetConceptDesignLibraryResponse InConceptDesignLibraryResponseDelegate)
{
    // 保存委托
    this->OnGetConceptDesignLibraryResponseDelegate = InConceptDesignLibraryResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/project/painting/folder/findListByParam"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("authorization"), Ticket);

    // 创建 JSON payload
    FString JsonPayload = FString::Printf(TEXT("{\"appId\":\"10011\",\"folderName\":\"\",\"projectNo\":\"%s\",\"uuid\":\"%s\"}"), *ProjectNo, *Uuid);
    Request->SetContentAsString(JsonPayload);

    // 绑定响应处理器
    Request->OnProcessRequestComplete().BindUObject(this, &UGetConceptDesignLibraryApi::OnResponseReceived);

    Request->ProcessRequest();
}

FString UGetConceptDesignLibraryApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

// 处理响应
void UGetConceptDesignLibraryApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetConceptDesignLibraryApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        // 普通结构体来解析 JSON
        FGetConceptDesignLibraryResponseStruct ResponseStruct;

        if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &ResponseStruct, 0, 0))
        {
            // 创建 UObject 数据并进行转换
            UGetConceptDesignLibraryResponseData* ConceptLibraryData = NewObject<UGetConceptDesignLibraryResponseData>();
            ConceptLibraryData->ConvertFromStruct(ResponseStruct);

            // 将解析后的数据传递给回调
            if (OnGetConceptDesignLibraryResponseDelegate.IsBound())
            {
                OnGetConceptDesignLibraryResponseDelegate.Execute(ConceptLibraryData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response"));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send GetConceptDesignLibraryApi request"));
    }
}
