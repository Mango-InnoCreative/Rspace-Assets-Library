// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Subsystem/USMSubsystem.h" // 包含 FUserAndProjectInfo 定义的头文件
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

DECLARE_DELEGATE(FOnProjectSwitchedDelegate);


class SProjectlistWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SProjectlistWidget) {}
	SLATE_ARGUMENT(FUserAndProjectInfo, UserAndProjectInfo)
	SLATE_EVENT(FOnProjectSwitchedDelegate, OnProjectSwitched)
	SLATE_END_ARGS()

void Construct(const FArguments& InArgs);

private:

	FUserAndProjectInfo UserAndProjectInfo;


	TSharedRef<SWidget> GenerateProjectList(const TArray<FProjectItem>& ProjectItems);


	FReply OnProjectClicked(FProjectItem ClickedProject);

	FOnProjectSwitchedDelegate OnProjectSwitchedDelegate;

	FString SelectedProjectName;

	FString TruncateText(const FString& OriginalText, int32 MaxLength);
	

};
