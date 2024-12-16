// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IImageWrapper.h"
#include "Interfaces/IHttpRequest.h"


DECLARE_DELEGATE_OneParam(FOnProjectImageReady, const TArray<uint8>&);

class FImageLoader
{
public:

	static void LoadImageFromUrl(const FString& Url, FOnProjectImageReady OnImageReadyDelegate);

	static void CancelImageRequest(const FString& Url);
	
	static void CancelAllImageRequests();

	static void ProcessNextImageRequest();
	

	static TMap<FString, FHttpRequestPtr> ActiveRequests;
	

	static void OnImageRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnProjectImageReady OnImageReadyDelegate, FString Url);

	static UTexture2D* CreateTextureFromBytes(const TArray<uint8>& ImageData);


	static UTexture2D* LoadTextureFromBytes(const TArray<uint8>& ImageData);


};
