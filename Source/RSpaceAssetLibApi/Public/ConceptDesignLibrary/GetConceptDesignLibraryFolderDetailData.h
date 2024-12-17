// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GetConceptDesignLibraryFolderDetailData.generated.h"


USTRUCT(BlueprintType)
struct FConceptDesignFileItem
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Id = -1; 


	UPROPERTY()
	FString UserNo = ""; 


	UPROPERTY()
	FString FolderId = ""; 

	UPROPERTY()
	FString Name = ""; 


	UPROPERTY()
	int32 TagCount = -1;

	
	UPROPERTY()
	FString RelativePatch = ""; 


	UPROPERTY()
	FString ThumRelativePatch = ""; 

	UPROPERTY()
	int32 DeleteStatus = -1; 

	UPROPERTY()
	int64 FileLength = -1; 

	UPROPERTY()
	FString FileSize = ""; 

	UPROPERTY()
	FString FileSuffix = ""; 

	UPROPERTY()
	FString FileMd5 = "";

	UPROPERTY()
	int32 FileMd5GetStatus = -1; 

	UPROPERTY()
	FString CreateTime = ""; 

	UPROPERTY()
	FString UpdateTime = "";

	UPROPERTY()
	FString ProjectNo = "";
};

UCLASS(Blueprintable)
class RSPACEASSETLIBAPI_API UGetConceptDesignLibraryFolderDetailData : public UObject
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
	int32 Total = -1; 

	UPROPERTY()
	TArray<FConceptDesignFileItem> Items; 
};
