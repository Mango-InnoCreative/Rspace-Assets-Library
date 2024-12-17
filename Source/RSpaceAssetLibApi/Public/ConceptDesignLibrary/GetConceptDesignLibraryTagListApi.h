// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetConceptDesignLibraryTagListData.h"
#include "GetConceptDesignLibraryTagListApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetConceptDesignLibraryTagListResponse, UGetConceptDesignLibraryTagListResponseData* /* ModelLibraryData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetConceptDesignLibraryTagListApi : public UObject
{
	GENERATED_BODY()

public:
	void SendConceptDesignLibraryTagListRequest(const FString& Ticket, const FString& ProjectNo, int32& Type, const FString& Uuid, FOnGetConceptDesignLibraryTagListResponse InOnGetConceptDesignLibraryTagListResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	FOnGetConceptDesignLibraryTagListResponse OnGetConceptDesignLibraryTagListResponseDelegate;
};
