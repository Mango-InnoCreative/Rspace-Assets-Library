// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetAudioAssetLibraryTagListData.generated.h"

USTRUCT(BlueprintType)
struct FAudioTagInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int64 Id = -1;

	UPROPERTY()
	FString ProjectNo = "";

	UPROPERTY()
	FString TagName = "";

	UPROPERTY()
	int32 TagCount = *"";

	UPROPERTY()
	int32 CommonStatus = -1;

	UPROPERTY()
	int32 TagGroupCount = -1;

	UPROPERTY()
	int32 IsDel = -1;

	UPROPERTY()
	FString CreateTime = "";

	UPROPERTY()
	FString UpdateTime = "";

	UPROPERTY()
	FString Remark = "";

	UPROPERTY()
	TArray<FString> GroupIds;

	UPROPERTY()
	TArray<FString> AudioNos;
};

USTRUCT(BlueprintType)
struct FGetAudioAssetLibraryTagListResponseData
{
	GENERATED_BODY()

	UPROPERTY()
	FString Status = "";

	UPROPERTY()
	FString Code = "";

	UPROPERTY()
	FString Message = "";

	UPROPERTY()
	TArray<FAudioTagInfo> Data;
};
