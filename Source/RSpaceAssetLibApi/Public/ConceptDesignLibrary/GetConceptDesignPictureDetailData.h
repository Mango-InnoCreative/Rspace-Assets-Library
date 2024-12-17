// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetConceptDesignPictureDetailData.generated.h"

USTRUCT(BlueprintType)
struct FPicTagGroups
{
	GENERATED_BODY()

	UPROPERTY()
	FString createTime = "";

	UPROPERTY()
	FString groupName = "";

	UPROPERTY()
	int64 id = -1;

	UPROPERTY()
	FString projectNo = "";

	UPROPERTY()
	FString updateTime = "";

	UPROPERTY()
	FString remark = "";
};

USTRUCT(BlueprintType)
struct FTags
{
	GENERATED_BODY()

	UPROPERTY()
	int64 id = -1;

	UPROPERTY()
	FString userNo = "";

	UPROPERTY()
	FString tagName = "";

	UPROPERTY()
	int32 tagCount = -1;

	UPROPERTY()
	FString projectNo = "";

	UPROPERTY()
	int32 commonStatus = -1;

	UPROPERTY()
	FString createTime = "";

	UPROPERTY()
	FString updateTime = "";

	UPROPERTY()
	FString remark = "";

	UPROPERTY()
	TArray<FPicTagGroups> groups;
};

USTRUCT(BlueprintType)
struct FConceptDesignPictureDetail
{
	GENERATED_BODY()

	UPROPERTY()
	int32 id = -1;

	UPROPERTY()
	FString userNo = "";

	UPROPERTY()
	FString folderId = "";

	UPROPERTY()
	FString name = "";

	UPROPERTY()
	int32 tagCount = -1;

	UPROPERTY()
	FString relativePatch = "";

	UPROPERTY()
	FString thumRelativePatch = "";

	UPROPERTY()
	int32 deleteStatus = -1;

	UPROPERTY()
	int32 fileLength = -1;

	UPROPERTY()
	FString fileSize = "";

	UPROPERTY()
	FString fileSuffix = "";

	UPROPERTY()
	FString fileMd5 = "";

	UPROPERTY()
	int32 fileMd5GetStatus = -1;

	UPROPERTY()
	FString createTime = "";

	UPROPERTY()
	FString updateTime = "";

	UPROPERTY()
	FString projectNo = "";

	UPROPERTY()
	TArray<FTags> tags;
};

USTRUCT(BlueprintType)
struct FGetConceptDesignPictureDetailData
{
	GENERATED_BODY()

	UPROPERTY()
	FString status = "";

	UPROPERTY()
	FString code = "";

	UPROPERTY()
	FString message = "";

	UPROPERTY()
	FConceptDesignPictureDetail data;
};
