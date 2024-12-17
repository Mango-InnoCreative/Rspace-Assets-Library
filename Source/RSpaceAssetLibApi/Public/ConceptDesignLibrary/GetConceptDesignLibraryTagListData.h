// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetConceptDesignLibraryTagListData.generated.h"

USTRUCT(BlueprintType)
struct FGroups
{
	GENERATED_BODY()

	UPROPERTY()
	FString createTime = "";

	UPROPERTY()
	FString groupName = "";

	UPROPERTY()
	int64 id = -1;

	UPROPERTY()
	FString projectNo = "";

	UPROPERTY()
	FString updateTime = "";

	UPROPERTY()
	FString remark = "";
};

USTRUCT(BlueprintType)
struct FConceptDesignFileItemTagList
{
	GENERATED_BODY()

	UPROPERTY()
	int64 id = -1;

	UPROPERTY()
	FString userNo = "";

	UPROPERTY()
	FString tagName = "";

	UPROPERTY()
	int32 tagCount = -1;

	UPROPERTY()
	FString projectNo = "";

	UPROPERTY()
	int32 commonStatus = -1;

	UPROPERTY()
	FString createTime = "";

	UPROPERTY()
	FString updateTime = "";

	UPROPERTY()
	FString remark = "";

	UPROPERTY()
	TArray<FGroups> groups;
};

UCLASS(Blueprintable)
class RSPACEASSETLIBAPI_API UGetConceptDesignLibraryTagListResponseData : public UObject
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
	TArray<FConceptDesignFileItemTagList> data;
};
