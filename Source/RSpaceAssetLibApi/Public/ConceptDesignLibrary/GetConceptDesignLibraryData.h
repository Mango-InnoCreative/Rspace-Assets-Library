// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetConceptDesignLibraryData.generated.h"

USTRUCT(BlueprintType)
struct FConceptDesignFolderItem
{
    GENERATED_BODY()

    UPROPERTY()
    int32 id = 0; 

    UPROPERTY()
    FString folderName = "";

    UPROPERTY()
    FString createTime = ""; 

    UPROPERTY()
    FString updateTime = "";

    UPROPERTY()
    FString remark = ""; 

    UPROPERTY()
    int32 paintingCount = 0; 
};


USTRUCT(BlueprintType)
struct FGetConceptDesignLibraryResponseStruct
{
    GENERATED_BODY()

    UPROPERTY()
    FString status = ""; 

    UPROPERTY()
    FString code = ""; 

    UPROPERTY()
    FString message = ""; 

    UPROPERTY()
    TArray<FConceptDesignFolderItem> data; 
};

UCLASS(Blueprintable)
class RSPACEASSETLIBAPI_API UGetConceptDesignLibraryResponseData : public UObject
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
    TArray<FConceptDesignFolderItem> data;  

    void ConvertFromStruct(const FGetConceptDesignLibraryResponseStruct& StructData)
    {
        status = StructData.status;
        code = StructData.code;
        message = StructData.message;

        data.Empty();
        for (const FConceptDesignFolderItem& FolderItemStruct : StructData.data)
        {
            FConceptDesignFolderItem FolderItem;
            FolderItem.id = FolderItemStruct.id;
            FolderItem.folderName = FolderItemStruct.folderName;
            FolderItem.createTime = FolderItemStruct.createTime;
            FolderItem.updateTime = FolderItemStruct.updateTime;
            FolderItem.remark = FolderItemStruct.remark;
            FolderItem.paintingCount = FolderItemStruct.paintingCount;

            data.Add(FolderItem);
        }
    }
};
