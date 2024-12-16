// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetAudioAssetLibraryFolderListData.generated.h"


USTRUCT(BlueprintType)
struct FAudioAssetLibraryFolderItem
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Id = -1;

	UPROPERTY()
	FString ProjectNo = "";

	UPROPERTY()
	FString GroupName = "";

	UPROPERTY()
	FString CreateTime = "";

	UPROPERTY()
	FString UpdateTime = "";

	UPROPERTY()
	FString Remark = "";
};


USTRUCT(BlueprintType)
struct FGetAudioAssetLibraryFolderListData
{
	GENERATED_BODY()

	UPROPERTY()
	FString Status = "";

	UPROPERTY()
	FString Code = "";

	UPROPERTY()
	FString Message = "";

	UPROPERTY()
	TArray<FAudioAssetLibraryFolderItem> Data;
};
