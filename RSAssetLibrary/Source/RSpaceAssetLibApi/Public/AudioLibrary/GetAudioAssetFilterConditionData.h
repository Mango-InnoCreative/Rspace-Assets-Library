// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetAudioAssetFilterConditionData.generated.h"


USTRUCT(BlueprintType)
struct FAudioConfigValue
{
	GENERATED_BODY()

	UPROPERTY()
	FString Id = "";

	UPROPERTY()
	FString Name = "";
};


USTRUCT(BlueprintType)
struct FAudioConfig
{
	GENERATED_BODY()

	UPROPERTY()
	FString ConfigKey = "";

	UPROPERTY()
	FString ConfigName = "";

	UPROPERTY()
	TArray<FAudioConfigValue> ConfigValue;
};

USTRUCT(BlueprintType)
struct FGetAudioAssetFilterConditionData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FAudioConfig> Data;
};


USTRUCT(BlueprintType)
struct FGetAudioAssetFilterConditionResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FString Status = "";

	UPROPERTY()
	FString Code = "";

	UPROPERTY()
	FString Message = "";

	UPROPERTY()
	FGetAudioAssetFilterConditionData Data;
};
