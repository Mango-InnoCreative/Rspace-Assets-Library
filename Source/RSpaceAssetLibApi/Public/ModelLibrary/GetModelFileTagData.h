// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetModelFileTagData.generated.h"

USTRUCT(BlueprintType)
struct FModelFileTagData
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
