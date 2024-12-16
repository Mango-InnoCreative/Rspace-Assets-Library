// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetConceptDesignLibraryData.h"
#include "GetConceptDesignLibraryApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetConceptDesignLibraryResponse, UGetConceptDesignLibraryResponseData* /* DesignLibraryData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetConceptDesignLibraryApi : public UObject
{
	GENERATED_BODY()

public:
	
	void SendGetConceptDesignLibraryRequest(const FString& Ticket, const FString& Uuid, const FString& ProjectNo, FOnGetConceptDesignLibraryResponse InConceptDesignLibraryResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FOnGetConceptDesignLibraryResponse OnGetConceptDesignLibraryResponseDelegate;
};
