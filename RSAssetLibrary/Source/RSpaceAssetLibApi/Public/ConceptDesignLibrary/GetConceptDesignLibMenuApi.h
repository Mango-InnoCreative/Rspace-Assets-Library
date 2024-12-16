// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "GetConceptDesignLibMenuData.h"
#include "GetConceptDesignLibMenuApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetConceptDesignLibMenuResponse, FGetConceptDesignLibMenuData*);

UCLASS()
class RSPACEASSETLIBAPI_API UGetConceptDesignLibMenuApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetConceptDesignLibMenuRequest(const FString& Ticket, const int32& CurrentPage, const FString& FolderId, const int32& MenuType, const int32& PageSize,
	                                        const FString& PaintingName, const FString& ProjectNo, const FString& TagId, const FString& TagName, const FString&
	                                        Uuid, FOnGetConceptDesignLibMenuResponse
	                                        InOnGetConceptDesignLibMenuResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	
	FOnGetConceptDesignLibMenuResponse OnGetConceptDesignLibMenuResponseDelegate;
};
