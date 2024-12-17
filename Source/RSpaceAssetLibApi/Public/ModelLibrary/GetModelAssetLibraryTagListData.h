// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetModelAssetLibraryTagListData.generated.h"


USTRUCT(BlueprintType)
struct FModelTagInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int64 Id = -1; 

	UPROPERTY()
	FString UserUuid = ""; 

	UPROPERTY()
	FString ProjectNo = "";  

	UPROPERTY()
	FString TagName = "";  

	UPROPERTY()
	FString CreateTime = "";  
};


USTRUCT(BlueprintType)
struct FGetModelAssetLibraryTagListResponseData
{
	GENERATED_BODY()

	UPROPERTY()
	FString Status = "";  

	UPROPERTY()
	FString Code = "";  

	UPROPERTY()
	FString Message = "";  

	UPROPERTY()
	TArray<FModelTagInfo> Data;  
};
