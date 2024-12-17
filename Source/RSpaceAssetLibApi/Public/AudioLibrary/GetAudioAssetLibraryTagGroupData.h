// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GetAudioAssetLibraryTagGroupData.generated.h"


USTRUCT(BlueprintType)
struct FAudioTagGroupInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int64 Id = -1;

	UPROPERTY()
	FString Name = "";

	UPROPERTY()
	FString ProjectNo = "";

	UPROPERTY()
	FString CreateTime = "";

	UPROPERTY()
	FString UpdateTime = "";

	UPROPERTY()
	FString Remark = "";
};

USTRUCT(BlueprintType)
struct FGetAudioAssetLibraryTagGroupResponseData
{
	GENERATED_BODY()

	UPROPERTY()
	FString Status = "";

	UPROPERTY()
	FString Code = "";

	UPROPERTY()
	FString Message = "";

	UPROPERTY()
	TArray<FAudioTagGroupInfo> Data;
};
