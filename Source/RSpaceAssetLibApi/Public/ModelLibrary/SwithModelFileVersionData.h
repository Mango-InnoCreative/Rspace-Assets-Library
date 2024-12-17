// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SwithModelFileVersionData.generated.h"

USTRUCT(BlueprintType)
struct FSwithModelFileVersionData
{
	GENERATED_BODY()

	UPROPERTY()
	FString status = "";

	UPROPERTY()
	FString code = "";  

	UPROPERTY()
	FString message = ""; 

	UPROPERTY()
	FString data = "";  
};
