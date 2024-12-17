// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetVideoFileInfoData.generated.h"

USTRUCT(BlueprintType)
struct FVideoFileInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString FileNo = ""; 

	UPROPERTY()
	FString FileName = "";

	UPROPERTY()
	FString Uuid = "";  

	UPROPERTY()
	FString CreateUserName = ""; 

	UPROPERTY()
	FString FileSize = "";  

	UPROPERTY()
	FString CreateTime = "";  

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
	int32 AudioChannel = -1;

	UPROPERTY()
	FString AudioEncoder = "";

	UPROPERTY()
	int32 AttachmentType = -1;  

	UPROPERTY()
	FString FileMd5 = "";  
};

UCLASS(Blueprintable)
class RSPACEASSETLIBAPI_API UGetVideoFileInfoData : public UObject
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
	FVideoFileInfo data; 
};
