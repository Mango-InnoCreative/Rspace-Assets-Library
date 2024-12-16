// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "QrLoginApiResponseData.h"
#include "QrLoginApi.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnQrCodeStateChanged, const FQrLoginResponseData&);
DECLARE_DELEGATE_OneParam(FOnQrCodeImageReady, const TArray<uint8>&);

UCLASS()
class RSPACEASSETLIBAPI_API UQrLoginApi : public UObject
{
	GENERATED_BODY()

public:
	UQrLoginApi();
   
	void Initialize(UWorld* InWorldContext); 

	void SendCreateQrRequest();

	void SetOnQrCodeImageReady(const FOnQrCodeImageReady& InOnQrCodeImageReady);

	void StartPollingGetInfo(const FString& QrCodeId, UObject* InWorldContext);

	void SetOnQrCodeStateChanged(FOnQrCodeStateChanged InOnQrCodeStateChanged);
	FString PreprocessJsonString(const FString& JsonString);

	void SetExternalState(int32 NewState); 

	void StopPolling();

	FString GetQrCodeId() const { return PreQrCodeId; }

	FOnQrCodeImageReady OnQrCodeImageReady;
	FOnQrCodeStateChanged OnQrCodeStateChanged;

private:
	
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void GetQrCodeImage(const FString& QrCodeId);
 
	void OnQrCodeImageReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void SendGetInfoRequest(FString QrCodeId); 

	void OnGetInfoResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FTimerHandle TimerHandle;

	FString PreQrCodeId;
	const FString AppId = "1101";

	int32 ExternalState;
	

	UObject* WorldContext;
};
