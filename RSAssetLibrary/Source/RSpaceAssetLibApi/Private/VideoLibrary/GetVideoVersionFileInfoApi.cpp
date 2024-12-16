// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "VideoLibrary/GetVideoVersionFileInfoApi.h"
#include "HttpModule.h"
#include "Json.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"

void UGetVideoVersionFileInfoApi::SendGetVideoVersionFileInfoRequest(const FString& Ticket, const FString& Uuid, const FString& AuditNo, FOnGetVideoVersionFileInfoResponse
                                                                     InResponseDelegate)
{
	OnResponseDelegate = InResponseDelegate;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://api.meta.mg.xyz/spaceapi/video/audit/version/selectAuditDetailInfo"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("authorization"), Ticket);
    
	FString JsonPayload = FString::Printf(TEXT("{\"uuid\":\"%s\",\"appId\":10011,\"auditNo\":\"%s\"}"), *Uuid, *AuditNo);
	Request->SetContentAsString(JsonPayload);
	Request->OnProcessRequestComplete().BindUObject(this, &UGetVideoVersionFileInfoApi::OnResponseReceived);
	Request->ProcessRequest();
}

FString UGetVideoVersionFileInfoApi::PreprocessJsonString(const FString& JsonString)
{
    FString ProcessedString = JsonString;
    ProcessedString = ProcessedString.Replace(TEXT(":null"), TEXT(":\"\""));
    return ProcessedString;
}

void UGetVideoVersionFileInfoApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        //FString ResponseContent = Response->GetContentAsString();
        // UE_LOG(LogTemp, Log, TEXT("GetVideoVersionFileInfoApi Response: %s"), *ResponseContent);
        FString ResponseContent = PreprocessJsonString(Response->GetContentAsString());
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            UGetVideoVersionFileInfoData* VideoFileInfoData = NewObject<UGetVideoVersionFileInfoData>();

            VideoFileInfoData->status = JsonObject->GetStringField("status");
            VideoFileInfoData->code = JsonObject->GetStringField("code");
            VideoFileInfoData->message = JsonObject->GetStringField("message");

            const TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField("data");
            FVideoVersionFileInfo& VersionFileInfo = VideoFileInfoData->data;

            VersionFileInfo.AuditNo = DataObject->GetStringField("auditNo");
            VersionFileInfo.VersionNo = DataObject->GetStringField("versionNo");
            VersionFileInfo.VersionName = DataObject->GetStringField("versionName");
            VersionFileInfo.AuditStatus = DataObject->GetIntegerField("auditStatus");
            VersionFileInfo.CoverImg = DataObject->GetStringField("coverImg");
            VersionFileInfo.VideoFileName = DataObject->GetStringField("videoFileName");
            VersionFileInfo.VideoFilePath = DataObject->GetStringField("videoFilePath");
            VersionFileInfo.CreateTime = DataObject->GetStringField("createTime");

       
            if (DataObject->HasTypedField<EJson::Object>("auditMemberlist"))
            {
                const TSharedPtr<FJsonObject> AuditMemberListObject = DataObject->GetObjectField("auditMemberlist");
                FVersionFileAuditMemberList& VersionFileAuditMemberList = VideoFileInfoData->data.AuditMemberList;

                if (AuditMemberListObject->HasField("memberAuditStatus"))
                {
                    VersionFileAuditMemberList.MemberAuditStatus = AuditMemberListObject->GetIntegerField("memberAuditStatus");
                }
                else
                {
                    VersionFileAuditMemberList.MemberAuditStatus = *"";
                }

                if (AuditMemberListObject->HasField("memberName"))
                {
                    VersionFileAuditMemberList.MemberName = AuditMemberListObject->GetStringField("memberName");
                }
                else
                {
                    VersionFileAuditMemberList.MemberName = "";
                }

                if (AuditMemberListObject->HasField("userNo"))
                {
                    VersionFileAuditMemberList.UserNo = AuditMemberListObject->GetStringField("userNo");
                }
                else
                {
                    VersionFileAuditMemberList.UserNo = ""; 
                }
            }
            else
            {
                FVersionFileAuditMemberList& VersionFileAuditMemberList = VideoFileInfoData->data.AuditMemberList;
                VersionFileAuditMemberList.MemberAuditStatus = *"";
                VersionFileAuditMemberList.MemberName = "";
                VersionFileAuditMemberList.UserNo = "";
            }


            const TSharedPtr<FJsonObject> FileInfoObject = DataObject->GetObjectField("fileInfo");
            if (FileInfoObject.IsValid())
            {
                FVersionFileInfo& VersionFileDetail = VideoFileInfoData->data.FileInfo;
                VersionFileDetail.CreateUserName = FileInfoObject->GetStringField("createUserName");
                VersionFileDetail.FileSize = FileInfoObject->GetStringField("fileSize");
                VersionFileDetail.FileFormat = FileInfoObject->GetStringField("fileFormat");
                VersionFileDetail.SampleRate = FileInfoObject->GetStringField("sampleRate");
                VersionFileDetail.FrameRate = FileInfoObject->GetStringField("frameRate");
                VersionFileDetail.VideoBiteRate = FileInfoObject->GetStringField("videoBiteRate");
                VersionFileDetail.VideoEncoder = FileInfoObject->GetStringField("videoEncoder");
                VersionFileDetail.FileTime = FileInfoObject->GetStringField("fileTime");
                VersionFileDetail.AudioBiteRate = FileInfoObject->GetStringField("audioBiteRate");
                VersionFileDetail.AudioHarvestRate = FileInfoObject->GetStringField("audioHarvestRate");
                VersionFileDetail.AudioChannel = FileInfoObject->GetIntegerField("audioChannel");
                VersionFileDetail.AudioEncoder = FileInfoObject->GetStringField("audioEncoder");
                VersionFileDetail.FileCreateTime = FileInfoObject->GetStringField("fileCreateTime");
                VersionFileDetail.FileType = FileInfoObject->GetIntegerField("fileType");
                VersionFileDetail.CodeId = FileInfoObject->GetIntegerField("codeId");
                VersionFileDetail.FileMd5 = FileInfoObject->GetStringField("fileMd5");
            }

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
