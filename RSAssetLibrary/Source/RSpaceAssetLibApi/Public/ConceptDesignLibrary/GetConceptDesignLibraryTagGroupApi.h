// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "GetConceptDesignLibraryTagGroupData.h"
#include "GetConceptDesignLibraryTagGroupApi.generated.h"

DECLARE_DELEGATE_OneParam(FOnGetConceptDesignLibraryTagGroupResponse, UGetConceptDesignLibraryTagGroupResponseData*);

UCLASS()
class RSPACEASSETLIBAPI_API UGetConceptDesignLibraryTagGroupApi : public UObject
{
	GENERATED_BODY()

public:
	void SendConceptDesignLibraryTagGroupRequest(const FString& Ticket, const FString& ProjectNo, const FString& Uuid, FOnGetConceptDesignLibraryTagGroupResponse InOnGetConceptDesignLibraryTagGroupResponseDelegate);
	FString PreprocessJsonString(const FString& JsonString);

private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	FOnGetConceptDesignLibraryTagGroupResponse OnGetConceptDesignLibraryTagGroupResponseDelegate;
};
