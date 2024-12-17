// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetModelLibraryData.h"
#include "GetModelLibrary.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetModelLibraryResponse, UGetModelLibraryResponseData* /* ModelLibraryData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetModelLibrary : public UObject
{
	GENERATED_BODY()

public:
	
	void SendGetModelLibraryRequest(const FString& Ticket, const FString& Uuid, int32& FileId, const FString& ProjectNo, const FString& fileName, const
	                                int64& tagId, const FString& tagName, FOnGetModelLibraryResponse InOnGetModelLibraryResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void OnResponseReceivedWithCleanup(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString RequestKey);


	FOnGetModelLibraryResponse OnGetModelLibraryResponseDelegate;
};
