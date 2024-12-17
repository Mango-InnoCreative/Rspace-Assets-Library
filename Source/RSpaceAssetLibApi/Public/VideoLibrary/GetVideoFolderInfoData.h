// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GetVideoFolderInfoData.generated.h"

USTRUCT(BlueprintType)
struct FGetVideoFolderInfoData
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
	FString SpaceSize = "";

	UPROPERTY()
	FString CreateTime = "";
};
