// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LoginApiResponseData.generated.h"

USTRUCT(BlueprintType)
struct FUserInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FString uuid = "";

    UPROPERTY()
    int32 id = -1;

    UPROPERTY()
    FString nickName = "";

    UPROPERTY()
    FString mobile = "";

    UPROPERTY()
    FString email = "";

    UPROPERTY()
    FString trade = "";

    UPROPERTY()
    FString qq = "";

    UPROPERTY()
    FString wx = "";

    UPROPERTY()
    FString zfb = "";

    UPROPERTY()
    FString avatar = "";

    UPROPERTY()
    FString introduction = "";

    UPROPERTY()
    FString createTime = "";

    UPROPERTY()
    FString uname = "";

    UPROPERTY()
    TArray<FString> badges;
};

USTRUCT(BlueprintType)
struct FMbTokenInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FString ticket = "";  

    UPROPERTY()
    int32 expire = -1;    
};

USTRUCT(BlueprintType)
struct FLoginApiResponseData
{
    GENERATED_BODY()

    UPROPERTY()
    FMbTokenInfo tokenInfo; 

    UPROPERTY()
    FUserInfo userInfo;  
};

USTRUCT(BlueprintType)
struct FLoginApiResponse
{
    GENERATED_BODY()

    UPROPERTY()
    FString status = "";  

    UPROPERTY()
    FString code = ""; 

    UPROPERTY()
    FString message = "";  

    UPROPERTY()
    FLoginApiResponseData data;  

    UPROPERTY()
    int32 groupId = -1;  

    UPROPERTY()
    bool mainAccount = false;  

    UPROPERTY()
    FString accountType = "";  
};
