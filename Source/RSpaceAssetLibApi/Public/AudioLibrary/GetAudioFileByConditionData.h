// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetAudioFileByConditionData.generated.h"


USTRUCT(BlueprintType)
struct FAudioFileData
{
	GENERATED_BODY()

	UPROPERTY()
	FString FileNo = "";

	UPROPERTY()
	FString FileName = "";

	UPROPERTY()
	FString RelativePath = "";

	UPROPERTY()
	FString FileSize = "";

	UPROPERTY()
	FString FileFormat = "";

	UPROPERTY()
	FString FileTime = "";

	UPROPERTY()
	FString AudioBiteRate = "";

	UPROPERTY()
	FString AudioScorePath = "";

	UPROPERTY()
	TArray<int32> GroupIdList;
};

// 定义响应中的 data 部分的数据结构
USTRUCT(BlueprintType)
struct FAudioFileResponseData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 CurPage = -1;

	UPROPERTY()
	int32 Total = -1;

	UPROPERTY()
	int32 TotalPage = -1;

	UPROPERTY()
	int32 Limit = -1;

	UPROPERTY()
	bool LastPage = false;

	UPROPERTY()
	TArray<FAudioFileData> dataList;
};

// 定义整个接口响应的数据结构
USTRUCT(BlueprintType)
struct FGetAudioFileByConditionResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FString Status = "";

	UPROPERTY()
	FString Code = "";

	UPROPERTY()
	FString Message = "";

	UPROPERTY()
	FAudioFileResponseData data;
};
