// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetVideoFileVersionInfoData.generated.h"

USTRUCT(BlueprintType)
struct FVideoFileVersionInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString AuditNo = ""; 

	UPROPERTY()
	FString VersionNo = "";

	UPROPERTY()
	FString VersionName = ""; 

	UPROPERTY()
	FString CoverImg = ""; 

	UPROPERTY()
	FString CreateTime = "";  

	UPROPERTY()
	int32 AuditStatus = -1; 

	UPROPERTY()
	FString VideoFileName = "";  

	UPROPERTY()
	FString VideoFilePath = "";  

	UPROPERTY()
	int32 IsChecked = -1; 

	UPROPERTY()
	int32 FileType = -1; 
};


USTRUCT(BlueprintType)
struct FGetVideoFileVersionInfoData
{
	GENERATED_BODY()

	UPROPERTY()
	FString Status = ""; 

	UPROPERTY()
	FString Code = ""; 
	UPROPERTY()
	FString Message = "";  

	UPROPERTY()
	TArray<FVideoFileVersionInfo> Data; 
};
