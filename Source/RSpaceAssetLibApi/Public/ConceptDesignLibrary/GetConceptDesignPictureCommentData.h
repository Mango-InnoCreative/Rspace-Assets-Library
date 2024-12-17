// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetConceptDesignPictureCommentData.generated.h"

USTRUCT(BlueprintType)
struct FPictureCommentItem
{
	GENERATED_BODY()

	UPROPERTY()
	int32 id = -1;

	UPROPERTY()
	int32 paintingId = -1;

	UPROPERTY()
	FString commentUserNo = "";

	UPROPERTY()
	FString commentContent = "";

	UPROPERTY()
	FString commentImg = "";

	UPROPERTY()
	FString createTime = "";

	UPROPERTY()
	FString updateTime = "";

	UPROPERTY()
	FString memberName = "";

	UPROPERTY()
	FString memberAccount = "";

	UPROPERTY()
	FString memberAvatar = "";

	UPROPERTY()
	FString memberId = "";
};

UCLASS(Blueprintable)
class RSPACEASSETLIBAPI_API UGetConceptDesignPictureCommentData : public UObject
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
	int32 total = -1;

	UPROPERTY()
	TArray<FPictureCommentItem> items;
};
