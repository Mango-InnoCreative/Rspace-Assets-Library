// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "AudioLibrary/GetAudioFileDetailApi.h"
#include "HttpModule.h"
#include "Json.h"
#include "JsonUtilities.h"

void UGetAudioFileDetailApi::SendGetAudioFileDetailRequest(const FString& Ticket, const FString& FileNo, FOnGetAudioFileDetailResponse InResponseDelegate)
{

	this->OnResponseDelegate = InResponseDelegate;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString Url = FString::Printf(TEXT("https://api.meta.mg.xyz/spaceapi/audio/file/getFileDetails/%s"), *FileNo);
	Request->SetURL(Url);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), Ticket);
	Request->OnProcessRequestComplete().BindUObject(this, &UGetAudioFileDetailApi::OnResponseReceived);
	Request->ProcessRequest();
}

FString UGetAudioFileDetailApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\"")); 
    return ProcessedString;
}

void UGetAudioFileDetailApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        //// UE_LOG(LogTemp, Log, TEXT("GetAudioFileDetailApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            const TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField("data");

            FAudioFileDetailData AudioFileData;


            AudioFileData.FileNo = DataObject->GetStringField("fileNo");
            AudioFileData.FileName = DataObject->GetStringField("fileName");
            AudioFileData.FileCoverPath = DataObject->GetStringField("fileCoverPath");
            AudioFileData.RelativePath = DataObject->GetStringField("relativePath");
            AudioFileData.FileSize = DataObject->GetStringField("fileSize");
            AudioFileData.FileMd5 = DataObject->GetStringField("fileMd5");
            AudioFileData.FileFormat = DataObject->GetStringField("fileFormat");
            AudioFileData.Bpm = DataObject->GetStringField("bpm");
            AudioFileData.FileTime = DataObject->GetStringField("fileTime");
            AudioFileData.AudioEncoder = DataObject->GetStringField("audioEncoder");
            AudioFileData.AudioChannel = DataObject->GetStringField("audioChannel");
            AudioFileData.AudioHarvestRate = DataObject->GetStringField("audioHarvestRate");
            AudioFileData.AudioBiteRate = DataObject->GetStringField("audioBiteRate");
            AudioFileData.AudioHarvestBits = DataObject->GetStringField("audioHarvestBits");


            const TArray<TSharedPtr<FJsonValue>> GroupInfoArray = DataObject->GetArrayField("groupInfoList");
            for (const TSharedPtr<FJsonValue>& GroupValue : GroupInfoArray)
            {
                FGroupInfo GroupInfo;
                const TSharedPtr<FJsonObject> GroupObject = GroupValue->AsObject();
                GroupInfo.GroupId = GroupObject->GetIntegerField("groupId");
                GroupInfo.GroupName = GroupObject->GetStringField("groupName");
                AudioFileData.GroupInfoList.Add(GroupInfo);
            }


            const TArray<TSharedPtr<FJsonValue>> TagInfoArray = DataObject->GetArrayField("tagInfoList");
            for (const TSharedPtr<FJsonValue>& TagValue : TagInfoArray)
            {
                FTagInfo TagInfo;
                const TSharedPtr<FJsonObject> TagObject = TagValue->AsObject();
                TagInfo.TagId = TagObject->GetIntegerField("tagId");
                TagInfo.TagName = TagObject->GetStringField("tagName");
                AudioFileData.TagInfoList.Add(TagInfo);
            }

            AudioFileData.UserNo = DataObject->GetStringField("userNo");
            AudioFileData.CreateName = DataObject->GetStringField("createName");
            AudioFileData.CreateTime = DataObject->GetStringField("createTime");
        	
            if (OnResponseDelegate.IsBound())
            {
                OnResponseDelegate.Execute(AudioFileData);
            }
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response."));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to send GetAudioFileDetailApi request."));
    }
}
