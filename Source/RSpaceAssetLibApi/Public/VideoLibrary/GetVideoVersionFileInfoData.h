// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetVideoVersionFileInfoData.generated.h"

USTRUCT(BlueprintType)
struct FVersionFileAuditMemberList
{
	GENERATED_BODY()

	UPROPERTY()
	int32 MemberAuditStatus = 0; 

	UPROPERTY()
	FString MemberName = ""; 

	UPROPERTY()
	FString UserNo = ""; 
};

USTRUCT(BlueprintType)
struct FVersionFileInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString CreateUserName = ""; 

	UPROPERTY()
	FString FileSize = ""; 

	UPROPERTY()
	FString FileFormat = "";

	UPROPERTY()
	FString SampleRate = ""; 

	UPROPERTY()
	FString FrameRate = ""; 

	UPROPERTY()
	FString VideoBiteRate = ""; 

	UPROPERTY()
	FString VideoEncoder = ""; 

	UPROPERTY()
	FString FileTime = ""; 

	UPROPERTY()
	FString AudioBiteRate = ""; 

	UPROPERTY()
	FString AudioHarvestRate = ""; 

	UPROPERTY()
	int32 AudioChannel = 0; 

	UPROPERTY()
	FString AudioEncoder = "";

	UPROPERTY()
	FString FileCreateTime = ""; 

	UPROPERTY()
	int32 FileType = 0; 

	UPROPERTY()
	int64 CodeId = 0; 

	UPROPERTY()
	FString FileMd5 = ""; 
};

USTRUCT(BlueprintType)
struct FVideoVersionFileInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString AuditNo = ""; 

	UPROPERTY()
	FString VersionNo = ""; 

	UPROPERTY()
	FString VersionName = ""; 

	UPROPERTY()
	int32 AuditStatus = 1; 

	UPROPERTY()
	FString CoverImg = ""; 

	UPROPERTY()
	FString VideoFileName = "";

	UPROPERTY()
	FString VideoFilePath = "";

	UPROPERTY()
	FVersionFileAuditMemberList AuditMemberList;

	UPROPERTY()
	FVersionFileInfo FileInfo; 

	UPROPERTY()
	FString CreateTime = ""; 
};

UCLASS(Blueprintable)
class RSPACEASSETLIBAPI_API UGetVideoVersionFileInfoData : public UObject
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
	FVideoVersionFileInfo data;  
};
