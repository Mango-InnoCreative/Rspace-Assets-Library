// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ConceptDesignLibrary/GetConceptDesignLibMenuApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonUtilities.h"

void UGetConceptDesignLibMenuApi::SendGetConceptDesignLibMenuRequest(const FString& Ticket, const int32& CurrentPage, const FString&  FolderId, const int32& MenuType, const int32& PageSize, const FString& PaintingName, const FString& ProjectNo, const FString& TagId, const FString& TagName, const FString& Uuid, FOnGetConceptDesignLibMenuResponse InOnGetConceptDesignLibMenuResponseDelegate)
{
	this->OnGetConceptDesignLibMenuResponseDelegate = InOnGetConceptDesignLibMenuResponseDelegate;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/space/project/painting/findMenuPaintingList"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), Ticket);
	FString JsonPayload = FString::Printf(TEXT("{\"appId\":\"10011\",\"currentPage\":\"%d\",\"folderId\":\"%s\", \"menuType\":\"%d\",\"pageSize\":\"%d\",\"paintingName\":\"%s\",\"projectNo\":\"%s\",\"tagId\":\"%s\",\"tagName\":\"%s\",\"uuid\":\"%s\"}"), CurrentPage, *FolderId, MenuType, PageSize, *PaintingName, *ProjectNo, *TagId, *TagName, *Uuid);
	Request->SetContentAsString(JsonPayload);

	Request->OnProcessRequestComplete().BindUObject(this, &UGetConceptDesignLibMenuApi::OnResponseReceived);
	Request->ProcessRequest();
}

FString UGetConceptDesignLibMenuApi::PreprocessJsonString(const FString& JsonString)
{
	FString ProcessedString = JsonString;
	ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
	return ProcessedString;
}

void UGetConceptDesignLibMenuApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		//FString ResponseContent = Response->GetContentAsString();
		// UE_LOG(LogTemp, Log, TEXT("GetConceptDesignLibMenuApi Response: %s"), *ResponseContent);
		FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
		FGetConceptDesignLibMenuData ConceptDesignMenuData;
        
		if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &ConceptDesignMenuData, 0, 0))
		{
			if (OnGetConceptDesignLibMenuResponseDelegate.IsBound())
			{
				OnGetConceptDesignLibMenuResponseDelegate.Execute(&ConceptDesignMenuData);
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
