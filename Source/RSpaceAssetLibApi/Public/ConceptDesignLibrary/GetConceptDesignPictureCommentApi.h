// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetConceptDesignPictureCommentData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetConceptDesignPictureCommentApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetConceptDesignPictureCommentResponse, UGetConceptDesignPictureCommentData* /* CommentData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetConceptDesignPictureCommentApi : public UObject
{
	GENERATED_BODY()

public:
	
	void SendGetConceptDesignPictureCommentRequest(
		const FString& Ticket, 
		const FString& Uuid, 
		int32 PicId, 
		int32 CurrentPage, 
		int32 PageSize, 
		FOnGetConceptDesignPictureCommentResponse InCommentResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	FOnGetConceptDesignPictureCommentResponse OnCommentResponseDelegate;
};
