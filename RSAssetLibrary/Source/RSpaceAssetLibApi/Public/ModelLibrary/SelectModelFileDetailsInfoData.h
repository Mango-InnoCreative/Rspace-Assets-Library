// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SelectModelFileDetailsInfoData.generated.h"

// 模型文件详情结构体
USTRUCT(BlueprintType)
struct FModelFileDetails
{
	GENERATED_BODY()

	UPROPERTY()
	int32 id = -1;  

	UPROPERTY()
	int32 parentId = -1;  

	UPROPERTY()
	FString userNo = "";

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
	FString fileStatus = "";  

	UPROPERTY()
	int32 version = -1;  

	UPROPERTY()
	FString remark = ""; 

	UPROPERTY()
	FString createTime = ""; 

	UPROPERTY()
	FString createrBy = ""; 

	UPROPERTY()
	FString updateTime = "";

	UPROPERTY()
	FString updateBy = ""; 

	UPROPERTY()
	FString gifUrl = ""; 

	UPROPERTY()
	FString gifFirstImg = ""; 
};


UCLASS(Blueprintable)
class RSPACEASSETLIBAPI_API USelectModelFileDetailsInfoData : public UObject
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
	FModelFileDetails data; 
};
