// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetVideoCommentListData.generated.h"

USTRUCT(BlueprintType)
struct FVideoComment
{
	GENERATED_BODY()

	UPROPERTY()
	FString AuditNo = "";  

	UPROPERTY()
	FString CommentContent = "";

	UPROPERTY()
	FString CommentImg = "";  

	UPROPERTY()
	FString PlayBackTime = "";

	UPROPERTY()
	FString Attachment = ""; 

	UPROPERTY()
	FString UserName = ""; 

	UPROPERTY()
	FString UserImg = ""; 

	UPROPERTY()
	FString AttachmentName = ""; 

	UPROPERTY()
	int32 IsLook = -1;  

	UPROPERTY()
	FString InformUserName = "";  

	UPROPERTY()
	FString CreateTime = "";  

	UPROPERTY()
	FString CommentUserNo = "";  

	UPROPERTY()
	int32 Id = -1; 
};

USTRUCT(BlueprintType)
struct FGetVideoCommentListResponseData
{
	GENERATED_BODY()

	UPROPERTY()
	FString Status = ""; 

	UPROPERTY()
	FString Code = "";  

	UPROPERTY()
	FString Message = ""; 

	UPROPERTY()
	TArray<FVideoComment> Data = {}; 
};
