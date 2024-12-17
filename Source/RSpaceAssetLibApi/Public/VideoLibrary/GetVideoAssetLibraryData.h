// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetVideoAssetLibraryData.generated.h"

USTRUCT(BlueprintType)
struct FVideoAssetLibraryInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString fileNo = "";  

	UPROPERTY()
	FString fileName = ""; 

	UPROPERTY()
	FString uuid = "";  

	UPROPERTY()
	FString createUserName = "";  

	UPROPERTY()
	FString spaceSize = ""; 

	UPROPERTY()
	FString createTime = "";  
};

// 视频资产库接口响应数据的结构体
USTRUCT(BlueprintType)
struct FGetVideoAssetLibraryResponseData
{
	GENERATED_BODY()

	UPROPERTY()
	FString status = "";  

	UPROPERTY()
	FString code = "";  

	UPROPERTY()
	FString message = "";  

	UPROPERTY()
	FVideoAssetLibraryInfo data = {};  
};
