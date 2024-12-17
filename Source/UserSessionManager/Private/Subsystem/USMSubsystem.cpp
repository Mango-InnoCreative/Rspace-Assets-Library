// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "Subsystem/USMSubsystem.h"
#include "EditorSubsystem.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/ConfigCacheIni.h"

void UUSMSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

  
    InitializeConfigFilePath();


    if (!CurrentSessionInfo.UserPhoneNumber.IsEmpty())
    {
        LoadUserSession(CurrentSessionInfo.UserPhoneNumber);
    }

    // UE_LOG(LogTemp, Warning, TEXT("UserSessionManager initialized"));
}


void UUSMSubsystem::Deinitialize()
{
  
    ClearCurrentSession();
    ClearCurrentUserAndProjectInfo();

    // UE_LOG(LogTemp, Log, TEXT("UserSessionManager deinitialized"));

    Super::Deinitialize();
}

void UUSMSubsystem::InitializeConfigFilePath()
{
    ConfigFilePath = FPaths::ProjectSavedDir() / TEXT("UserSessionConfig.ini");
}

void UUSMSubsystem::SaveUserSession(const FUserSessionInfo& UserSessionInfo)
{
    CurrentSessionInfo = UserSessionInfo;

    FString UserFolder = FPaths::ProjectSavedDir() / TEXT("UserData") / UserSessionInfo.UserPhoneNumber;
    FString IniFilePath = UserFolder / TEXT("UserSession.ini");

    if (!IFileManager::Get().DirectoryExists(*UserFolder))
    {
        IFileManager::Get().MakeDirectory(*UserFolder);
    }

    FString SessionData;
    SessionData += TEXT("[UserSession]\n");
    SessionData += FString::Printf(TEXT("UserPhoneNumber=%s\n"), *UserSessionInfo.UserPhoneNumber);
    SessionData += FString::Printf(TEXT("UserName=%s\n"), *UserSessionInfo.UserName);
    SessionData += FString::Printf(TEXT("SessionExpireTime=%d\n"), UserSessionInfo.SessionExpireTime);

    if (FFileHelper::SaveStringToFile(SessionData, *IniFilePath))
    {
        // UE_LOG(LogTemp, Log, TEXT("用户会话信息已保存到：%s"), *IniFilePath);
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("无法将用户会话信息保存到：%s"), *IniFilePath);
    }
}

FUserSessionInfo UUSMSubsystem::GetSavedUserSession() const
{
    return CurrentSessionInfo;
}

bool UUSMSubsystem::IsSessionValid() const
{
    int32 CurrentTime = FDateTime::UtcNow().ToUnixTimestamp();
    return CurrentTime < CurrentSessionInfo.SessionExpireTime;
}

FString UUSMSubsystem::GetCurrentPhoneNumber() const
{
    return CurrentSessionInfo.UserPhoneNumber;
}

bool UUSMSubsystem::IsSessionValidForUser(const FString& InUserPhoneNumber) const
{
    if (InUserPhoneNumber == CurrentSessionInfo.UserPhoneNumber)
    {
        int32 CurrentTime = FDateTime::Now().ToUnixTimestamp();
        return CurrentTime < CurrentSessionInfo.SessionExpireTime;
    }
    return false;
}

void UUSMSubsystem::SetCurrentUserAndProjectInfo(const FUserAndProjectInfo& NewInfo)
{
    CurrentUserAndProjectInfo = NewInfo;
}

void UUSMSubsystem::SetSelectedProject(const FProjectItem& NewProject)
{
    SelectedProject = NewProject;
    // UE_LOG(LogTemp, Warning, TEXT("Selected Project Is : %s"), *SelectedProject.projectName)
}

FProjectItem UUSMSubsystem::GetSelectedProject() const
{
    return SelectedProject;
}

FUserAndProjectInfo UUSMSubsystem::GetCurrentUserAndProjectInfo()
{
    return CurrentUserAndProjectInfo;
}

void UUSMSubsystem::ClearSelectedProject()
{
    SelectedProject = FProjectItem(); 
    // UE_LOG(LogTemp, Warning, TEXT("Selected Project has been cleared"));
}


void UUSMSubsystem::SetUpdatePrjectItems(FFindProjectListData NewProjectList)
{
    FUserAndProjectInfo OldProject = GetCurrentUserAndProjectInfo();
    OldProject.ProjectItems = NewProjectList;
    SetCurrentUserAndProjectInfo(OldProject);
}

void UUSMSubsystem::ClearCurrentSession()
{
    // UE_LOG(LogTemp, Warning, TEXT("CurrentSessionInfo Is Clear !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
    CurrentSessionInfo = FUserSessionInfo();
}

void UUSMSubsystem::ClearCurrentUserAndProjectInfo()
{
    // UE_LOG(LogTemp, Warning, TEXT("CurrentUserAndProjectInfo Is Clear !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
    CurrentUserAndProjectInfo = FUserAndProjectInfo(); 
}

bool UUSMSubsystem::LoadUserSession(const FString& PhoneNumber)
{
    FString UserFolder = FPaths::ProjectSavedDir() / TEXT("UserData") / PhoneNumber;
    FString IniFilePath = UserFolder / TEXT("UserSession.ini");

    if (FPaths::FileExists(IniFilePath))
    {
        FString LoadedData;
        FFileHelper::LoadFileToString(LoadedData, *IniFilePath);

        FString UserPhoneNumber, UserName;
        int32 SessionExpireTime;

        if (FParse::Value(*LoadedData, TEXT("UserPhoneNumber="), UserPhoneNumber) &&
            FParse::Value(*LoadedData, TEXT("UserName="), UserName) &&
            FParse::Value(*LoadedData, TEXT("SessionExpireTime="), SessionExpireTime))
        {
            CurrentSessionInfo.UserPhoneNumber = UserPhoneNumber;
            CurrentSessionInfo.UserName = UserName;
            CurrentSessionInfo.SessionExpireTime = SessionExpireTime;

            return true;
        }
    }

    return false;
}
