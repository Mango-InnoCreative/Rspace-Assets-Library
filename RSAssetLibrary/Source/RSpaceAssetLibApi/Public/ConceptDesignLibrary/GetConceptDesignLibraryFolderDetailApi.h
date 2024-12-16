// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetConceptDesignLibraryFolderDetailData.h"
#include "GetConceptDesignLibraryFolderDetailApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetConceptDesignFolderDetailResponse, UGetConceptDesignLibraryFolderDetailData* /* FolderDetailData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetConceptDesignLibraryFolderDetailApi : public UObject
{
	GENERATED_BODY()

public:
	
	void SendGetFolderDetailRequest(const FString& Ticket, const FString& Uuid, const FString& PaintingName, int32 FolderId, int32 CurrentPage, int32 PageSize, FString TagId,FOnGetConceptDesignFolderDetailResponse InFolderDetailResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FOnGetConceptDesignFolderDetailResponse OnFolderDetailResponseDelegate;
};
