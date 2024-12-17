// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GetConceptDesignPictureDetailData.h"
#include "GetConceptDesignPictureDetailApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnConceptDesignPictureDetailResponse, FGetConceptDesignPictureDetailData* /* PictureDetailData */);

UCLASS()
class RSPACEASSETLIBAPI_API UGetConceptDesignPictureDetailApi : public UObject
{
	GENERATED_BODY()

public:

	void SendGetPictureDetailRequest(const FString& Ticket, int32 PicId, FOnConceptDesignPictureDetailResponse InConceptDesignPictureDetailResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	FOnConceptDesignPictureDetailResponse OnConceptDesignPictureDetailResponseDelegate;
};
