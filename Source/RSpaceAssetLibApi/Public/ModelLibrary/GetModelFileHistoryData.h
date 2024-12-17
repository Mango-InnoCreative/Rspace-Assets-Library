// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetModelFileHistoryData.generated.h"


USTRUCT(BlueprintType)
struct FModelFileHistoryItem
{
	GENERATED_BODY()

	UPROPERTY()
	int32 id = -1; 

	UPROPERTY()
	FString projectNo = ""; 

	UPROPERTY()
	FString fileNo = ""; 

	UPROPERTY()
	FString fileName = "";  

	UPROPERTY()
	FString filePath = ""; 

	UPROPERTY()
	FString relativePath = "";  

	UPROPERTY()
	int32 version = 0;  

	UPROPERTY()
	int32 fileStatus = 0;  

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
};


UCLASS(Blueprintable)
class RSPACEASSETLIBAPI_API UGetModelFileHistoryResponseData : public UObject
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
	TArray<FModelFileHistoryItem> data; 
};
