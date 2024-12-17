// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FindProjectListResponseData.generated.h"

USTRUCT(BlueprintType)
struct FAdminInfo
{
    GENERATED_BODY()

    UPROPERTY()
    int32 id = -1; 

    UPROPERTY()
    FString userUuid = ""; 

    UPROPERTY()
    int32 userLevel = -1;  

    UPROPERTY()
    FString createTime = "";  

    UPROPERTY()
    FString updateTime = "";  

    UPROPERTY()
    FString accountType = ""; 

    UPROPERTY()
    FString identityType = ""; 
    
    UPROPERTY()
    FString userPhone = ""; 

    UPROPERTY()
    FString userMail = "";  

    UPROPERTY()
    FString userName = "";  

    UPROPERTY()
    FString userAccount = "";  

    UPROPERTY()
    FString userAvatar = "";  
};


USTRUCT(BlueprintType)
struct FProjectItem
{
    GENERATED_BODY()

    UPROPERTY()
    int32 id = -1;  

    UPROPERTY()
    FString projectNo = "";  

    UPROPERTY()
    FString dramaName = ""; 
    
    UPROPERTY()
    FString dramaImg = "";  

    UPROPERTY()
    FString projectUserNo = "";  

    UPROPERTY()
    FString projectDescribe = ""; 

    UPROPERTY()
    int32 projectStatus = -1;  

    UPROPERTY()
    FString createTime = "";  

    UPROPERTY()
    FString updateTime = "";  

    UPROPERTY()
    FString projectSvn = ""; 
    
    UPROPERTY()
    FString projectName = "";  

    UPROPERTY()
    FString projectImg = "";  
    
    UPROPERTY()
    FString applicationId = "";  

    UPROPERTY()
    int32 projectTemplateId = -1;  
    
    UPROPERTY()
    TArray<FAdminInfo> adminList = {}; 
};

// Data部分的结构体，包含 `total` 和 `items`
USTRUCT(BlueprintType)
struct FFindProjectListData
{
    GENERATED_BODY()

    UPROPERTY()
    int32 total = -1;  

    UPROPERTY()
    TArray<FProjectItem> items = {};  
};


UCLASS(BlueprintType)
class RSPACEASSETLIBAPI_API UFindProjectListResponseData : public UObject
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
    FFindProjectListData data = {};  

    void AddProjectItem(int32 ProjectID, const FString& ProjectNo, const FString& ProjectName, const FString& ProjectSvn, const FString& ProjectImg, int32 ProjectStatus, const FString& CreateTime, const FString& UpdateTime, const FString& ProjectDescribe, const TArray<FAdminInfo>& AdminList)
    {
        FProjectItem NewItem;
        NewItem.id = ProjectID;
        NewItem.projectNo = ProjectNo;
        NewItem.projectName = ProjectName;
        NewItem.projectSvn = ProjectSvn;
        NewItem.projectImg = ProjectImg;
        NewItem.projectStatus = ProjectStatus;
        NewItem.createTime = CreateTime;
        NewItem.updateTime = UpdateTime;
        NewItem.projectDescribe = ProjectDescribe;
        NewItem.adminList = AdminList;
        data.items.Add(NewItem);
    }
};
