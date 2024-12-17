// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "QrLoginApiResponseData.generated.h"

USTRUCT(BlueprintType)
struct FQrUserInfoDetails
{
    GENERATED_BODY()

    UPROPERTY()
    FString uuid = "";

    UPROPERTY()
    FString id = "";

    UPROPERTY()
    FString nickname = "";

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
    FString updateTime = "";

    UPROPERTY()
    int32 state = -1;

    UPROPERTY()
    bool isDeleted = false;

    UPROPERTY()
    TArray<FString> badges;

    UPROPERTY()
    FString uname = "";
};

USTRUCT(BlueprintType)
struct FQrTokenInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FString ticket = "";

    UPROPERTY()
    int32 expire = -1;
};

USTRUCT(BlueprintType)
struct FQrUserLoginInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FQrUserInfoDetails userInfo;

    UPROPERTY()
    FQrTokenInfo tokenInfo;
};

USTRUCT(BlueprintType)
struct FQrLoginResponseData
{
    GENERATED_BODY()

    UPROPERTY()
    FString qrCodeId = "";

    UPROPERTY()
    int32 state = -1;

    UPROPERTY()
    int64 qrCreateTm = -1;

    UPROPERTY()
    FQrUserLoginInfo userInfo;
};

USTRUCT(BlueprintType)
struct FQrLoginApiResponse
{
    GENERATED_BODY()

    UPROPERTY()
    int32 code = -1;

    UPROPERTY()
    FString message = "";

    UPROPERTY()
    FString msg = "";

    UPROPERTY()
    FQrLoginResponseData data;
};
