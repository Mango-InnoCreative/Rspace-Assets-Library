// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "Login/QrLoginApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "TimerManager.h"

UQrLoginApi::UQrLoginApi()
    : PreQrCodeId(TEXT("")), ExternalState(0), WorldContext(nullptr)
{
}

void UQrLoginApi::Initialize(UWorld* InWorldContext)
{
    WorldContext = InWorldContext;
}

void UQrLoginApi::SendCreateQrRequest()
{
    StopPolling(); 

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://api.open.mg.xyz/uc/qrLogin/createQr"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
    
    FString FormParams = FString::Printf(TEXT("appId=%s&preQrCodeId=%s"), *AppId, *PreQrCodeId);
    Request->SetContentAsString(FormParams);
    Request->OnProcessRequestComplete().BindUObject(this, &UQrLoginApi::OnResponseReceived);
    Request->ProcessRequest();
}

void UQrLoginApi::SetOnQrCodeImageReady(const FOnQrCodeImageReady& InOnQrCodeImageReady)
{
    StopPolling();  
    OnQrCodeImageReady = InOnQrCodeImageReady;
}

void UQrLoginApi::SetOnQrCodeStateChanged(FOnQrCodeStateChanged InOnQrCodeStateChanged)
{
    OnQrCodeStateChanged = InOnQrCodeStateChanged;
}

FString UQrLoginApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UQrLoginApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            if (JsonObject->HasField("data") && JsonObject->GetObjectField("data").IsValid())
            {
                FString QrCodeId = JsonObject->GetObjectField("data")->GetStringField("qrCodeId");
                PreQrCodeId = QrCodeId;
                GetQrCodeImage(QrCodeId);
                UWorld* World = nullptr;

                if (GEngine)
                {
                    for (const FWorldContext& Context : GEngine->GetWorldContexts())
                    {
                        if (Context.World() != nullptr)
                        {
                            World = Context.World();
                            break;
                        }
                    }
                }
                StartPollingGetInfo(QrCodeId, World);
            }
            else
            {
                // UE_LOG(LogTemp, Error, TEXT("Failed to retrieve qrCodeId from response: %s"), *ResponseContent);
            }
        }
    }
    else
    {
        FString ResponseContent = Response.IsValid() ? Response->GetContentAsString() : TEXT("No Response");
        // UE_LOG(LogTemp, Error, TEXT("Failed to send CreateQrApi request, response: %s"), *ResponseContent);
    }
}

void UQrLoginApi::GetQrCodeImage(const FString& QrCodeId)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    FString Url = FString::Printf(TEXT("https://api.open.mg.xyz/spaceapi/am/user/loginQR?qrCodeId=%s"), *QrCodeId);
    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));
    Request->OnProcessRequestComplete().BindUObject(this, &UQrLoginApi::OnQrCodeImageReceived);
    Request->ProcessRequest();
}

void UQrLoginApi::OnQrCodeImageReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        const TArray<uint8>& ImageData = Response->GetContent();
        if (OnQrCodeImageReady.IsBound())
        {
            OnQrCodeImageReady.Execute(ImageData);
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to retrieve QR Code image"));
    }
}

void UQrLoginApi::StartPollingGetInfo(const FString& QrCodeId, UObject* InWorldContext)
{
    WorldContext = InWorldContext;

    if (WorldContext)
    {
        if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull))
        {
            if (World->GetTimerManager().IsTimerActive(TimerHandle))
            {
                World->GetTimerManager().ClearTimer(TimerHandle);
                //// UE_LOG(LogTemp, Log, TEXT("Clearing existing polling timer."));
            }

            World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &UQrLoginApi::SendGetInfoRequest, QrCodeId), 0.5f, true);
            //// UE_LOG(LogTemp, Log, TEXT("Started polling for QrCodeId: %s"), *QrCodeId);
        }
    }
}

void UQrLoginApi::SendGetInfoRequest(FString QrCodeId)
{
    FString Url = FString::Printf(TEXT("https://api.open.mg.xyz/uc/qrLogin/getInfo?qrCodeId=%s"), *QrCodeId);
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));
    Request->OnProcessRequestComplete().BindUObject(this, &UQrLoginApi::OnGetInfoResponseReceived);
    Request->ProcessRequest();
}

void UQrLoginApi::OnGetInfoResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        FString ResponseContent = Response->GetContentAsString();
        FQrLoginApiResponse QrCodeResponse;
        
        if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseContent, &QrCodeResponse, 0, 0))
        {
            const FQrLoginResponseData& QrCodeState = QrCodeResponse.data;

            if (QrCodeState.state == 2 || QrCodeState.state == 3 || QrCodeState.state == 4 || ExternalState == 6 || ExternalState == 7)
            {
                if (WorldContext)
                {
                    UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
                    if (World && World->GetTimerManager().IsTimerActive(TimerHandle))
                    {
                        World->GetTimerManager().ClearTimer(TimerHandle);
                        //// UE_LOG(LogTemp, Log, TEXT("Timer successfully cleared."));
                    }
                }
            }

            if (OnQrCodeStateChanged.IsBound())
            {
                OnQrCodeStateChanged.Broadcast(QrCodeState);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse GetInfo Response to FQrLoginApiResponse."));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to receive GetInfo response or response is invalid."));
    }
}

void UQrLoginApi::SetExternalState(int32 NewState)
{
    ExternalState = NewState;
    if (ExternalState == 6 || ExternalState == 7)
    {
        if (WorldContext)
        {
            UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
            if (World && World->GetTimerManager().IsTimerActive(TimerHandle))
            {
                World->GetTimerManager().ClearTimer(TimerHandle);
                //// UE_LOG(LogTemp, Log, TEXT("Polling stopped due to external state: %d"), ExternalState);
            }
        }
    }
}

void UQrLoginApi::StopPolling()
{
    if (WorldContext)
    {
        UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
        if (World && World->GetTimerManager().IsTimerActive(TimerHandle))
        {
            World->GetTimerManager().ClearTimer(TimerHandle);
            //// UE_LOG(LogTemp, Log, TEXT("Polling stopped"));
        }
    }
}
