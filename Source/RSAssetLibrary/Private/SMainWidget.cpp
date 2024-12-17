// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "SMainWidget.h"
#include "Login/SLoginWidget.h"
#include "ProjectContent/SProjectWidget.h"
#include "Subsystem/USMSubsystem.h"

#define LOCTEXT_NAMESPACE "MainWidget"

void SMainWidget::Construct(const FArguments& InArgs)
{
    // Gets the session manager instance 获取会话管理器实例
    UUSMSubsystem* SessionManager = GEditor->GetEditorSubsystem<UUSMSubsystem>();
    
    // Check whether the session is valid 检查会话是否有效
    FString CurrentPhoneNumber = SessionManager ? SessionManager->GetCurrentPhoneNumber() : TEXT("");
    bool bIsSessionValid = SessionManager && SessionManager->IsSessionValidForUser(CurrentPhoneNumber);

    if (bIsSessionValid)
    {
        ShowProjectWidget();
    }
    else
    {
        ShowLoginWidget();
    }

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            SAssignNew(ContentBox, SBox)
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            [
                CurrentWidget.IsValid() ? CurrentWidget.ToSharedRef() : SNullWidget::NullWidget
            ]
        ]
    ];
}

// Show project interface 显示项目界面
void SMainWidget::ShowProjectWidget()
{
    if (!CachedProjectWidget.IsValid())
    {
        FUserAndProjectInfo UserAndProjectInfo = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo();
        CachedProjectWidget = SNew(SProjectWidget)
            .UserAndProjectInfo(UserAndProjectInfo)
            .OnLogout(FOnLogoutDelegate::CreateSP(this, &SMainWidget::OnLogout));
    }

    if (ContentBox.IsValid())
    {
        ContentBox->SetContent(CachedProjectWidget.ToSharedRef());
    }

    CurrentWidget = CachedProjectWidget;
}

// Display login screen 显示登录界面
void SMainWidget::ShowLoginWidget()
{
    if (!CachedLoginWidget.IsValid())
    {
        CachedLoginWidget = SNew(SLoginWidget)
            .OnLoginSuccess(FOnLoginSuccess::CreateSP(this, &SMainWidget::ShowProjectWidget));
    }

    if (ContentBox.IsValid())
    {
        ContentBox->SetContent(CachedLoginWidget.ToSharedRef());
    }

    CurrentWidget = CachedLoginWidget;
}

// Process exit logic 处理退出逻辑
void SMainWidget::OnLogout()
{
    if (CachedProjectWidget.IsValid())
    {
        CachedProjectWidget->CancelAllDownloads();
        CachedProjectWidget->CloseAllOpenedWindows();
        CachedProjectWidget->ClearAllCachedTextures();
        CachedProjectWidget.Reset();
    }

    if (UUSMSubsystem* SessionManager = GEditor->GetEditorSubsystem<UUSMSubsystem>())
    {
        SessionManager->ClearCurrentSession();
        SessionManager->ClearCurrentUserAndProjectInfo();
    }

    if (CachedLoginWidget.IsValid())
    {
        CachedLoginWidget->RestartRolling();

         CachedLoginWidget->ResetLoginMsg();
    }

    ShowLoginWidget();
}


TSharedPtr<SCompoundWidget> SMainWidget::GetCurrentWidget() const
{
    return CurrentWidget;
}
