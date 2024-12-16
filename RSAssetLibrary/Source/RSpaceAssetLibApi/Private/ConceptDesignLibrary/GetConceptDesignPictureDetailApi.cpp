// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ConceptDesignLibrary/GetConceptDesignPictureDetailApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

void UGetConceptDesignPictureDetailApi::SendGetPictureDetailRequest(const FString& Ticket, int32 PicId, FOnConceptDesignPictureDetailResponse InConceptDesignPictureDetailResponseDelegate)
{
    this->OnConceptDesignPictureDetailResponseDelegate = InConceptDesignPictureDetailResponseDelegate;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    FString Url = FString::Printf(TEXT("https://api.meta.mg.xyz/spaceapi/space/project/painting/findPaintingDetails/%d"), PicId);
    Request->SetURL(Url);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), Ticket);

    Request->OnProcessRequestComplete().BindUObject(this, &UGetConceptDesignPictureDetailApi::OnResponseReceived);
    Request->ProcessRequest();
}

FString UGetConceptDesignPictureDetailApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\""));
    return ProcessedString;
}

void UGetConceptDesignPictureDetailApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetConceptDesignPictureDetailApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        FGetConceptDesignPictureDetailData PictureDetailData;
        
        if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &PictureDetailData, 0, 0))
        {
            if (OnConceptDesignPictureDetailResponseDelegate.IsBound())
            {
                OnConceptDesignPictureDetailResponseDelegate.Execute(&PictureDetailData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response."));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send request."));
    }
}
