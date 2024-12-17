// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetModelLibraryData.generated.h"

USTRUCT(BlueprintType)
struct FModelFileItem
{
	GENERATED_BODY()

	UPROPERTY()
	int32 id = -1; 

	UPROPERTY()
	int32 parentId = -1;

	UPROPERTY()
	FString uuid = ""; 

	UPROPERTY()
	FString projectNo = ""; 

	UPROPERTY()
	FString fileNo = "";  

	UPROPERTY()
	FString fileName = "";

	UPROPERTY()
	FString filePath = ""; 

	UPROPERTY()
	FString thumRelativePath = ""; 

	UPROPERTY()
	FString relativePath = "";  

	UPROPERTY()
	int64 fileSize = -1;  

	UPROPERTY()
	FString fileMd5 = "";  

	UPROPERTY()
	int32 fileType = -1; 

	UPROPERTY()
	int32 fileStatus = -1;  

	UPROPERTY()
	int32 version = -1; 

	UPROPERTY()
	FString remark = ""; 

	UPROPERTY()
	FString gifUrl = "";  

	UPROPERTY()
	FString gifFirstImg = "";  

	UPROPERTY()
	FString createTime = "";  

	UPROPERTY()
	FString createrBy = ""; 

	UPROPERTY()
	FString updateTime = "";

	UPROPERTY()
	FString updateBy = ""; 
};

UCLASS(Blueprintable)
class RSPACEASSETLIBAPI_API UGetModelLibraryResponseData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString status = ""; 

	UPROPERTY()
	FString code = "";  

	UPROPERTY()
	FString message = "";

	UPROPERTY()
	TArray<FModelFileItem> data;  
};
