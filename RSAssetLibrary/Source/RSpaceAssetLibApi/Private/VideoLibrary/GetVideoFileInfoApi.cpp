// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "VideoLibrary/GetVideoFileInfoApi.h"
#include "HttpModule.h"
#include "Json.h"
#include "JsonObjectConverter.h"

void UGetVideoFileInfoApi::SendGetVideoFileInfoRequest(const FString& Ticket, const FString& FileNo, FOnGetVideoFileInfoResponse InResponseDelegate)
{
	OnResponseDelegate = InResponseDelegate;
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString Url = FString::Printf(TEXT("https://api.meta.mg.xyz/spaceapi/video/file/selectFileInfo/%s"), *FileNo);
	Request->SetURL(Url);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Authorization"), Ticket);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	Request->OnProcessRequestComplete().BindUObject(this, &UGetVideoFileInfoApi::OnResponseReceived);
	
	Request->ProcessRequest();
}

FString UGetVideoFileInfoApi::PreprocessJsonString(const FString& JsonString)
{
	FString ProcessedString = JsonString;
	ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
	return ProcessedString;
}

void UGetVideoFileInfoApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		//FString ResponseContent = Response->GetContentAsString();
		//// UE_LOG(LogTemp, Log, TEXT("GetVideoFileInfoApi Response: %s"), *ResponseContent);
		FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			UGetVideoFileInfoData* VideoFileInfoData = NewObject<UGetVideoFileInfoData>();

			VideoFileInfoData->status = JsonObject->GetStringField("status");
			VideoFileInfoData->code = JsonObject->GetStringField("code");
			VideoFileInfoData->message = JsonObject->GetStringField("message");

			const TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField("data");
			FVideoFileInfo& VideoFileInfo = VideoFileInfoData->data;

			VideoFileInfo.FileNo = DataObject->GetStringField("fileNo");
			VideoFileInfo.FileName = DataObject->GetStringField("fileName");
			VideoFileInfo.Uuid = DataObject->GetStringField("uuid");
			VideoFileInfo.CreateUserName = DataObject->GetStringField("createUserName");
			VideoFileInfo.FileSize = DataObject->GetStringField("fileSize");
			VideoFileInfo.CreateTime = DataObject->GetStringField("createTime");
			VideoFileInfo.FileFormat = DataObject->GetStringField("fileFormat");
			VideoFileInfo.SampleRate = DataObject->GetStringField("sampleRate");
			VideoFileInfo.FrameRate = DataObject->GetStringField("frameRate");
			VideoFileInfo.VideoBiteRate = DataObject->GetStringField("videoBiteRate");
			VideoFileInfo.VideoEncoder = DataObject->GetStringField("videoEncoder");
			VideoFileInfo.FileTime = DataObject->GetStringField("fileTime");
			VideoFileInfo.AudioBiteRate = DataObject->GetStringField("audioBiteRate");
			VideoFileInfo.AudioHarvestRate = DataObject->GetStringField("audioHarvestRate");
			VideoFileInfo.AudioChannel = DataObject->GetIntegerField("audioChannel");
			VideoFileInfo.AudioEncoder = DataObject->GetStringField("audioEncoder");
			VideoFileInfo.AttachmentType = DataObject->GetIntegerField("attachmentType");
			VideoFileInfo.FileMd5 = DataObject->GetStringField("fileMd5");

			if (OnResponseDelegate.IsBound())
			{
				OnResponseDelegate.Execute(VideoFileInfoData);
			}
		}
		else
		{
			// UE_LOG(LogTemp, Error, TEXT("Failed to receive GetVideoFileInfoApi response."));
		}
	}
}
