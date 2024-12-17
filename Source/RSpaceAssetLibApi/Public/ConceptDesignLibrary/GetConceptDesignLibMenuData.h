// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetConceptDesignLibMenuData.generated.h"

USTRUCT(BlueprintType)
struct FFileItemDetails
{
	GENERATED_BODY()

	UPROPERTY()
	int32 id = 0; 

	UPROPERTY()
	FString userNo = "";

	UPROPERTY()
	FString name = ""; 

	UPROPERTY()
	int32 tagCount = 0;

	UPROPERTY()
	FString relativePatch = ""; 

	UPROPERTY()
	FString thumRelativePatch = ""; 

	UPROPERTY()
	int32 deleteStatus = 0; 

	UPROPERTY()
	int32 fileLength = 0; 

	UPROPERTY()
	FString fileSize = "";

	UPROPERTY()
	FString fileSuffix = ""; 

	UPROPERTY()
	FString fileMd5 = ""; 

	UPROPERTY()
	int32 fileMd5GetStatus = 0; 

	UPROPERTY()
	FString createTime = ""; 

	UPROPERTY()
	FString updateTime = ""; 

	UPROPERTY()
	FString projectNo = "";
};

USTRUCT(BlueprintType)
struct FConceptDesignMenuData
{
	GENERATED_BODY()

	UPROPERTY()
	int64 total = 0;

	UPROPERTY()
	TArray<FFileItemDetails> items; 
};

USTRUCT(BlueprintType)
struct FGetConceptDesignLibMenuData
{
	GENERATED_BODY()

	UPROPERTY()
	FString status = ""; 

	UPROPERTY()
	FString code = ""; 

	UPROPERTY()
	FString message = ""; 

	UPROPERTY()
	FConceptDesignMenuData data; //   
};
