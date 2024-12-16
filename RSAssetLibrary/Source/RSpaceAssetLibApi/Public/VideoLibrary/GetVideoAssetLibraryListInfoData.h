// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetVideoAssetLibraryListInfoData.generated.h"

USTRUCT(BlueprintType)
struct FVideoAssetInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 id = -1; 

	UPROPERTY()
	FString parentId = ""; 

	UPROPERTY()
	FString userNo = ""; 

	UPROPERTY()
	FString fileName = ""; 

	UPROPERTY()
	int32 fileType = -1; 

	UPROPERTY()
	FString relativePatch = "";  

	UPROPERTY()
	FString thumRelativePatch = "";

	UPROPERTY()
	FString updateUserName = "";  

	UPROPERTY()
	FString createUserName = "";  

	UPROPERTY()
	FString createTime = "";

	UPROPERTY()
	FString updateTime = "";  

	UPROPERTY()
	FString fileNo = ""; 

	UPROPERTY()
	int32 isTop = -1; 

	UPROPERTY()
	FString fileSize = "";  

	UPROPERTY()
	FString versionName = ""; 

	UPROPERTY()
	FString auditNo = ""; 

	UPROPERTY()
	int32 auditStatus = -1;  

	UPROPERTY()
	int32 commentNum = -1; 

	UPROPERTY()
	FString hierarchyPath = "";  

	UPROPERTY()
	int32 attachmentType = -1;  
};

USTRUCT(BlueprintType)
struct FGetVideoAssetLibraryListInfoData
{
	GENERATED_BODY()

	UPROPERTY()
	FString status = "";  

	UPROPERTY()
	FString code = ""; 

	UPROPERTY()
	FString message = ""; 

	UPROPERTY()
	TArray<FVideoAssetInfo> data = {};  
};
