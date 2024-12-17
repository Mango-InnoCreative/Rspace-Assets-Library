// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectContent/Imageload/FImageLoader.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Engine/Texture2D.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "Async/Async.h"

TMap<FString, FHttpRequestPtr> FImageLoader::ActiveRequests;

static TQueue<TFunction<void()>> ImageRequestQueue; // Request task queue 请求任务队列
static FThreadSafeCounter ActiveImageRequests;      // The number of requests currently active 当前活动的请求数
static const int32 MaxConcurrentImageRequests = 5; // Maximum number of concurrent requests 最大并发请求数

void FImageLoader::LoadImageFromUrl(const FString& Url, FOnProjectImageReady OnImageReadyDelegate)
{
    // Add the request to the task queue 将请求加入任务队列
    ImageRequestQueue.Enqueue([Url, OnImageReadyDelegate]()
    {
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
        HttpRequest->SetURL(Url);
        HttpRequest->SetVerb(TEXT("GET"));

        // Save the request for subsequent cancellation 保存请求以便后续取消
        ActiveRequests.Add(Url, HttpRequest);

        // Callback when the binding request completes 绑定请求完成时的回调
        HttpRequest->OnProcessRequestComplete().BindStatic(&FImageLoader::OnImageRequestComplete, OnImageReadyDelegate, Url);
        HttpRequest->ProcessRequest();
    });

    // Check if you can handle the next task 检查是否可以处理下一个任务
    ProcessNextImageRequest();
}

void FImageLoader::ProcessNextImageRequest()
{
    if (ActiveImageRequests.GetValue() >= MaxConcurrentImageRequests)
    {
        return; // Reach the maximum number of concurrent requests and wait for idle 达到最大并发数，等待空闲
    }

    TFunction<void()> Task;
    if (ImageRequestQueue.Dequeue(Task))
    {
        ActiveImageRequests.Increment(); // Increase the active request count 增加活动请求计数
        Task(); // Execute request 执行请求
    }
}

void FImageLoader::OnImageRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnProjectImageReady OnImageReadyDelegate, FString Url)
{
    // Removed from the active request list 从活动请求列表中移除
    ActiveRequests.Remove(Url);
    ActiveImageRequests.Decrement(); // Reduce the active request count 减少活动请求计数

    if (bWasSuccessful && Response.IsValid())
    {
        const TArray<uint8>& ImageData = Response->GetContent();
        // UE_LOG(LogTemp, Log, TEXT("Image download successful for URL: %s. Data size: %d"), *Url, ImageData.Num());
        
        if (OnImageReadyDelegate.IsBound())
        {
            Async(EAsyncExecution::TaskGraphMainThread, [OnImageReadyDelegate, ImageData]()
            {
                OnImageReadyDelegate.Execute(ImageData);
            });
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to load image from URL: %s"), *Url);
    }

    // Try to tackle the next task 尝试处理下一个任务
    ProcessNextImageRequest();
}

void FImageLoader::CancelImageRequest(const FString& Url)
{
    TSharedPtr<IHttpRequest> HttpRequest = ActiveRequests.FindRef(Url);
    if (HttpRequest.IsValid())
    {
        HttpRequest->CancelRequest();
        ActiveRequests.Remove(Url);
        ActiveImageRequests.Decrement(); // Reduce the active request count 减少活动请求计数
        // UE_LOG(LogTemp, Log, TEXT("Cancelled image request for URL: %s"), *Url);
    }
}

void FImageLoader::CancelAllImageRequests()
{
    // Cancel a request in progress 取消正在进行的请求
    for (auto& Pair : ActiveRequests)
    {
        if (Pair.Value.IsValid())
        {
            Pair.Value->CancelRequest();
        }
    }

    // Clear the activity request mapping table 清空活动请求映射表
    ActiveRequests.Empty();

    // Stop all queued requests 停止所有排队的请求
    TFunction<void()> Dummy;
    while (ImageRequestQueue.Dequeue(Dummy)) {}

    // Reset count 重置计数
    ActiveImageRequests.Reset();
}




UTexture2D* FImageLoader::CreateTextureFromBytes(const TArray<uint8>& ImageData)
{
    if (ImageData.Num() == 0)
    {
        // UE_LOG(LogTemp, Error, TEXT("Image data is empty, cannot create texture."));
        return nullptr;
    }

    // Get the ImageWrapper module 获取 ImageWrapper 模块
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
    EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());
    if (ImageFormat == EImageFormat::Invalid)
    {
        // UE_LOG(LogTemp, Error, TEXT("Unrecognized image format."));
        return nullptr;
    }

    // Create an ImageWrapper 创建 ImageWrapper
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
    if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to create or set compressed data for ImageWrapper."));
        return nullptr;
    }

    // Get raw image data 获取原始图像数据
    TArray<uint8> RawData;
    if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to get raw image data."));
        return nullptr;
    }

    // Create a UTexture2D object 创建 UTexture2D 对象
    UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
    if (!Texture)
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to create transient texture."));
        return nullptr;
    }

    // Lock texture data 锁定纹理数据
    void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(TextureData, RawData.GetData(), RawData.Num());
    Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

    // Update texture resources 更新纹理资源
    Texture->UpdateResource();

    // 设置纹理属性
    Texture->SRGB = true;
    Texture->CompressionSettings = TC_Default;
    Texture->MipGenSettings = TMGS_NoMipmaps;

    return Texture;
}

UTexture2D* FImageLoader::LoadTextureFromBytes(const TArray<uint8>& ImageData)
{
    UTexture2D* Texture = nullptr;

    // Does not rely on URL suffixes, but identifies the format by content 不依赖URL后缀，而是通过内容识别格式
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    EImageFormat DetectedFormat = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());
    
    if (DetectedFormat == EImageFormat::Invalid)
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to detect image format URL"));
        return nullptr;
    }

    // Create an appropriate ImageWrapper based on the detected image format 根据检测到的图片格式创建合适的 ImageWrapper
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(DetectedFormat);
    if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
    {
        TArray<uint8> UncompressedRGBA;
        if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
        {
            Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

            if (Texture && Texture->GetPlatformData())
            {
                void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
                if (TextureData)
                {
                    FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
                    Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
                    Texture->UpdateResource();
                }
                else
                {
                    // UE_LOG(LogTemp, Error, TEXT("Failed to lock texture data for writing."));
                }
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to uncompress image data."));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Invalid image wrapper or failed to set compressed data."));
    }

    return Texture;
}


