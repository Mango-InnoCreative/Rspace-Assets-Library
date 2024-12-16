// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetAudioCommentData.generated.h"

USTRUCT(BlueprintType)
struct FAudioCommentItem
{
	GENERATED_BODY()

	UPROPERTY()
	FString AppId = "";

	UPROPERTY()
	FString Uuid = "";

	UPROPERTY()
	int32 CurrentPage = -1;

	UPROPERTY()
	int32 PageSize = -1;

	UPROPERTY()
	FString FileNo = "";

	UPROPERTY()
	FString Content = "";

	UPROPERTY()
	FString MemberUuid = "";

	UPROPERTY()
	FString MemberName = "";

	UPROPERTY()
	FString CreateTime = "";

	UPROPERTY()
	FString UserAvatar = "";
};

UCLASS(Blueprintable)
class UGetAudioCommentResponseData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString Status = "";

	UPROPERTY()
	FString Code = "";

	UPROPERTY()
	FString Message = "";

	UPROPERTY()
	int32 CurPage = -1;

	UPROPERTY()
	int32 Total = -1;

	UPROPERTY()
	int32 TotalPage = -1;

	UPROPERTY()
	int32 Limit = -1;

	UPROPERTY()
	bool LastPage = false;

	UPROPERTY()
	TArray<FAudioCommentItem> DataList;
};
