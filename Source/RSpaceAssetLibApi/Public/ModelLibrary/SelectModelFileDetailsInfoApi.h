// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SelectModelFileDetailsInfoData.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "SelectModelFileDetailsInfoApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnSelectModelFileDetailsInfoResponse, USelectModelFileDetailsInfoData* /* ModelFileDetailsData */);

UCLASS()
class RSPACEASSETLIBAPI_API USelectModelFileDetailsInfoApi : public UObject
{
    GENERATED_BODY()

public:
  
    void SendSelectModelFileDetailsInfoRequest(const FString& Ticket, const FString& FileNo, FOnSelectModelFileDetailsInfoResponse InResponseDelegate);
    FString PreprocessJsonString(const FString& JsonString);

private:
   
    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

   
    FOnSelectModelFileDetailsInfoResponse OnSelectModelFileDetailsInfoResponseDelegate;
};
