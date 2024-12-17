// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ConceptDesignLibrary/GetConceptDesignLibraryData.h"
#include "AudioLibrary/GetAudioAssetLibraryFolderListData.h"
#include "AudioLibrary/GetAudioFileByConditionData.h"
#include "ConceptDesignLibrary/GetConceptDesignLibMenuData.h"
#include "ModelLibrary/GetModelLibraryData.h"
#include "RSpaceAssetLibApi/Public/Projectlist/FindProjectListResponseData.h"
#include "VideoLibrary/GetVideoAssetLibraryListInfoData.h"
#include "USMSubsystem.generated.h"


struct FUserSessionInfo
{
public:
	FString UserPhoneNumber;
	
	FString UserName;  
	
	int32 SessionExpireTime;  

	FUserSessionInfo()
		: UserPhoneNumber(TEXT("")), UserName(TEXT("")),SessionExpireTime(0) {}
};

struct FUserAndProjectInfo
{
public:


	FString Uuid;
	
	FString Ticket;  

	FString UserAvatarUrl;

	FFindProjectListData ProjectItems;

	FUserAndProjectInfo()
		:Uuid(TEXT("")),Ticket(TEXT("")),ProjectItems(){}
};


UCLASS()
class USERSESSIONMANAGER_API UUSMSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	virtual void Initialize(FSubsystemCollectionBase& Collection);

	virtual void Deinitialize();

	void SaveUserSession(const FUserSessionInfo& UserSessionInfo);

	FUserSessionInfo GetSavedUserSession() const;

	bool IsSessionValid() const;

	FString GetCurrentPhoneNumber() const;

	bool IsSessionValidForUser(const FString& UserName) const;

	void SetCurrentUserAndProjectInfo(const FUserAndProjectInfo& NewInfo);

	void SetSelectedProject(const FProjectItem& Project);

	FProjectItem GetSelectedProject() const;
	
	FUserAndProjectInfo GetCurrentUserAndProjectInfo();

	void ClearSelectedProject();

	
	TArray<FModelFileItem> GetCurrentModelItems() const { return CurrentModelItems; }

	void SetCurrentModelItems(TArray<FModelFileItem> InModelItems) { CurrentModelItems = InModelItems; }
	
	int32 GetCurrentModelRootID() const { return CurrentModelRootFileId; }

	void SetCurrentModelRootID(int32 InModelRootID) { CurrentModelRootFileId = InModelRootID; }

	void ResetCurrentModelItems(){ CurrentModelItems.Empty(); }


	TArray<FFileItemDetails> GetCurrentFirstPageConceptItems() const { return CurrentFirstPageConceptItems; }

	void SetCurrentFirstPageConceptItems(TArray<FFileItemDetails> InFirstPageConceptItems) { CurrentFirstPageConceptItems = InFirstPageConceptItems; }
	
	TArray<FConceptDesignFolderItem> GetCurrentConceptFolderItems() const { return CurrentConceptDesignFolderItems; }

	void SetCurrentConceptFolderItems(TArray<FConceptDesignFolderItem> InConceptFolderItems) { CurrentConceptDesignFolderItems = InConceptFolderItems; }

	void ResetCurrentConceptFolderItems(){ CurrentConceptDesignFolderItems.Empty(); }

	void SetCurrentConceptFolderID(FString InFolderID) { CurrentConceptFolderID = InFolderID; }

	FString GetCurrentConceptFolderID() { return CurrentConceptFolderID; }

	TArray<FAudioAssetLibraryFolderItem> GetCurrentAudioFolderItems() const { return CurrentAudioFolderItems; }

	void SetCurrentAudioFolderItems(TArray<FAudioAssetLibraryFolderItem> InAudioFolderItems) { CurrentAudioFolderItems = InAudioFolderItems; }

	void ResetCurrentAudioFolderItems(){ CurrentAudioFolderItems.Empty(); }


	TArray<FAudioFileData> GetCurrentFirstPageAudioFolderItems() const { return CurrentFirstPageAudioFolderItems; }

	void SetCurrentFirstPageAudioFolderItems(TArray<FAudioFileData> InAudioFirstPageFolderItems) { CurrentFirstPageAudioFolderItems = InAudioFirstPageFolderItems; }

	void ResetCurrentFirstPageAudioFolderItems(){ CurrentFirstPageAudioFolderItems.Empty(); }

	void SetCurrentVideoParentID(const FString& InID) { CurrentVideoParentId = InID; }

	FString GetCurrentVideoParentID(){ return CurrentVideoParentId; }

	TArray<FVideoAssetInfo> GetCurrentVideoFolderItems() const { return CurrentVideoFolderItems; }

	void SetCurrentVideoFolderItems(TArray<FVideoAssetInfo> InVideoFolderItems) { CurrentVideoFolderItems = InVideoFolderItems; }

	void ResetCurrentVideoFolderItems(){ CurrentVideoFolderItems.Empty(); }

	void SetUpdatePrjectItems(FFindProjectListData NewProjectList);

	void SetCurrentAudioGroupID(FString InGroupID) { CurrentAudioGroupID = InGroupID; }

	FString GetCurrentAudioGroupID() { return CurrentAudioGroupID; }

	void ClearCurrentSession();

	void ClearCurrentUserAndProjectInfo();

private:
	FUserSessionInfo CurrentSessionInfo; 

	FUserAndProjectInfo CurrentUserAndProjectInfo;

	FProjectItem SelectedProject;
	
	FString ConfigFilePath;

	void InitializeConfigFilePath();

	bool LoadUserSession(const FString& PhoneNumber);

	int32 CurrentModelRootFileId = 0;

	TArray<FConceptDesignFolderItem> CurrentConceptDesignFolderItems;

	TArray<FAudioAssetLibraryFolderItem> CurrentAudioFolderItems;

	TArray<FAudioFileData> CurrentFirstPageAudioFolderItems;

	TArray<FVideoAssetInfo> CurrentVideoFolderItems;

	FString CurrentVideoParentId = "";

	TArray<FModelFileItem> CurrentModelItems;

	TArray<FFileItemDetails> CurrentFirstPageConceptItems;

	FString CurrentConceptFolderID;

	FString CurrentAudioGroupID;
};
