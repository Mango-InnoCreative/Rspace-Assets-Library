// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetConceptDesignLibraryTagGroupData.generated.h"

USTRUCT(BlueprintType)
struct FConceptDesignTagGroup
{
	GENERATED_BODY()

	UPROPERTY()
	int64 id = -1;

	UPROPERTY()
	FString projectNo = "";

	UPROPERTY()
	FString groupName = "";

	UPROPERTY()
	FString createTime = "";

	UPROPERTY()
	FString updateTime = "";

	UPROPERTY()
	FString remark = "";
};

UCLASS(Blueprintable)
class RSPACEASSETLIBAPI_API UGetConceptDesignLibraryTagGroupResponseData : public UObject
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
	TArray<FConceptDesignTagGroup> data;
};
