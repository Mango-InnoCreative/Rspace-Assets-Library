// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetAudioFileDetailData.generated.h"


USTRUCT(BlueprintType)
struct FTagInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 TagId = -1;

	UPROPERTY()
	FString TagName = "";
};


USTRUCT(BlueprintType)
struct FGroupInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 GroupId = -1;

	UPROPERTY()
	FString GroupName = "";
};


USTRUCT(BlueprintType)
struct FAudioFileDetailData
{
	GENERATED_BODY()

	UPROPERTY()
	FString FileNo = "";

	UPROPERTY()
	FString FileName = "";

	UPROPERTY()
	FString FileCoverPath = "";

	UPROPERTY()
	FString RelativePath = "";

	UPROPERTY()
	FString FileSize = "";

	UPROPERTY()
	FString FileMd5 = "";

	UPROPERTY()
	FString FileFormat = "";

	UPROPERTY()
	FString Bpm = "";

	UPROPERTY()
	FString FileTime = "";

	UPROPERTY()
	FString AudioEncoder = "";

	UPROPERTY()
	FString AudioChannel = "";

	UPROPERTY()
	FString AudioHarvestRate = "";

	UPROPERTY()
	FString AudioBiteRate = "";

	UPROPERTY()
	FString AudioHarvestBits = "";

	UPROPERTY()
	TArray<FGroupInfo> GroupInfoList;

	UPROPERTY()
	TArray<FTagInfo> TagInfoList;

	UPROPERTY()
	FString UserNo = "";

	UPROPERTY()
	FString CreateName = "";

	UPROPERTY()
	FString CreateTime = "";
};
