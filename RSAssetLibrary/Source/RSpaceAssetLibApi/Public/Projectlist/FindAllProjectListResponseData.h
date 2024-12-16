// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FindAllProjectListResponseData.generated.h"


USTRUCT(BlueprintType)
struct FAllProjectItem
{
	GENERATED_BODY()

	UPROPERTY()
	FString projectNo = "";  

	UPROPERTY()
	FString dramaNo = "";  

	UPROPERTY()
	FString projectName = "";

	UPROPERTY()
	FString projectImg = "";  
};


UCLASS(Blueprintable)
class RSPACEASSETLIBAPI_API UFindAllProjectListResponseData : public UObject
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
	TArray<FAllProjectItem> data = {};
};
