// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.


#include "ProjectContent/AssetDownloader/SAssetDownloadWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "AssetDownloader.h"
#include "Modules/ModuleManager.h"
#include "RSAssetLibrary.h"
#include "RSAssetLibraryStyle.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "SAssetDownloadWidget"

static const int32 MaxConcurrentDownloads = 10;
FCriticalSection SAssetDownloadWidget::ActiveTasksMutex;
FCriticalSection SAssetDownloadWidget::PendingTasksMutex;
FCriticalSection SAssetDownloadWidget::PausedTasksMutex;

TQueue<TSharedPtr<SAssetDownloadWidget>> SAssetDownloadWidget::PendingTasks;
TArray<TSharedPtr<SAssetDownloadWidget>> SAssetDownloadWidget::ActiveTasks;
TArray<TWeakPtr<SAssetDownloadWidget>> SAssetDownloadWidget::PausedTasks;
TArray<TSharedPtr<SAssetDownloadWidget>> SAssetDownloadWidget::FailedTasks;

void SAssetDownloadWidget::Construct(const FArguments& InArgs)
{
    DownloadCompleteWidget = InArgs._DownloadCompleteWidget;

    if (DownloadCompleteWidget.IsValid())
    {
        // UE_LOG(LogTemp, Log, TEXT("DownloadCompleteWidget is valid in Construct."));
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("DownloadCompleteWidget is invalid in Construct!"));
    }
    
    AssetDownloader = NewObject<UAssetDownloader>();
    AssetDownloader->SetOnDownloadProgress(FOnDownloadProgress::CreateSP(this, &SAssetDownloadWidget::OnDownloadProgress));
    AssetDownloader->SetOnDownloadComplete(FOnDownloadComplete::CreateSP(this, &SAssetDownloadWidget::OnDownloadComplete));
    AssetDownloader->SetOnDownloadError(FOnDownloadError::CreateSP(this, &SAssetDownloadWidget::OnDownloadError));

    AssetURL = InArgs._URL;
    AssetFileName = InArgs._FileName;
    AssetMD5 = InArgs._MD5;
    ParentContainer = InArgs._ParentContainer; 
    
    ChildSlot
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .Padding(10, 5, 5, 5)
        .FillWidth(1.0f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TruncateText(AssetFileName, 20)))
            ]
        ]
         
          + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(70, 5, 5, 5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .Padding(10, 5, 5, 5)
            [
                SAssignNew(StartOrResumeButton, SButton)
                //.Text(this, &SAssetDownloadWidget::GetStartOrResumeButtonText)
                .Cursor(EMouseCursor::Hand)
                .ButtonStyle(&BeginButtonStyle) 
                .ContentPadding(0) 
                .ToolTipText(LOCTEXT("DownloadTooltip", "下载"))
                .OnClicked(this, &SAssetDownloadWidget::OnStartOrResumeClicked)
                .IsEnabled_Lambda([this]() { return bIsPaused; }) 
            ]
            
            + SHorizontalBox::Slot()
            .Padding(10, 5, 5, 5)
            [
                SNew(SButton)
                .Cursor(EMouseCursor::Hand)
                .ButtonStyle(&PauseButtonStyle) 
                .ContentPadding(0) 
                .ToolTipText(LOCTEXT("PauseTooltip", "暂停"))
                .OnClicked(this, &SAssetDownloadWidget::OnPauseClicked)
                .IsEnabled_Lambda([this]() { return !bIsPaused; }) 
            ]
            
            + SHorizontalBox::Slot()
            .Padding(10, 5, 5, 5)
            [
                SNew(SButton)
                .Cursor(EMouseCursor::Hand)
                .ButtonStyle(&CancelButtonStyle)
                .ContentPadding(0) 
                .ToolTipText(LOCTEXT("CancelTooltip", "取消"))
                .OnClicked(this, &SAssetDownloadWidget::OnCancelClicked)
            ]
        ]
        
        + SHorizontalBox::Slot()
            .Padding(20, 5, 5, 5)
          .FillWidth(1.0f)
          .VAlign(VAlign_Center)
        [
            SAssignNew(DownloadStatusText, STextBlock)
            .Text(LOCTEXT("WaitingForDownload", "等待中..."))
        ]
        
        + SHorizontalBox::Slot()
        .Padding(20, 5, 5, 5)
        .AutoWidth() 
        .VAlign(VAlign_Center) 
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .VAlign(VAlign_Center)
            [
                SNew(SBox)
                .HeightOverride(4.0f)
                .WidthOverride(106.0f)
                [
                    SAssignNew(DownloadProgressBar, SProgressBar)
                    .Style(&FRSAssetLibraryStyle::Get().GetWidgetStyle<FProgressBarStyle>("RSAssetLibrary.CustomProgressBar"))
                    .Percent(0.0f)
                ]
            ]
        ]
      
    ];
    
    BeginButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
    BeginButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
    BeginButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));

    PauseButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseIcon"));
    PauseButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"));
    PauseButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"));

    CancelButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.CancelIcon"));
    CancelButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.CancelClicked"));
    CancelButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.CancelClicked"));
}

FString SAssetDownloadWidget::TruncateText(const FString& OriginalText, int32 MaxLength)
{
    int32 Length = 0;  
    FString Result;

    for (int32 Index = 0; Index < OriginalText.Len(); ++Index)
    {
        TCHAR Character = OriginalText[Index];
        if (Character >= 0x4E00 && Character <= 0x9FFF)
        {
            Length += 2; 
        }
        else
        {
            Length += 1; 
        }
        if (Length > MaxLength)
        {
            Result += TEXT("...");
            break;
        }
        Result.AppendChar(Character);
    }

    return Result;
}


void SAssetDownloadWidget::SetDownloadParameters(const FString& InURL, const FString& InFileName, const FString& InMD5)
{
    OriginalURL = InURL;
    AssetFileName = InFileName;
    AssetMD5 = InMD5;
}

FReply SAssetDownloadWidget::OnStartOrResumeClicked()
{
    if (ActiveTasks.Num() >= MaxConcurrentDownloads)
    {
        FScopeLock Lock(&ActiveTasksMutex);
        PendingTasks.Enqueue(SharedThis(this));
        if (DownloadStatusText.IsValid())
        {
            DownloadStatusText->SetText(LOCTEXT("WaitingForDownload", "等待中..."));
        }
    }
    else
    {
        if (!ActiveTasks.Contains(SharedThis(this))) 
        {
            StartDownload();
        }
    }
    
    bIsPaused = false; 
    
    BeginButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
    PauseButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseIcon"));

    return FReply::Handled();
}


void SAssetDownloadWidget::StartDownload()
{
    if (AssetDownloader->IsPaused())
    {
        AssetDownloader->ResumeDownload();
        DownloadStatusText->SetText(LOCTEXT("ContinueDownload", "继续下载..."));
    }
    else
    {
        int32 LastSlashIndex;
        if (OriginalURL.FindLastChar('/', LastSlashIndex))
        {
            FString BaseUrl = OriginalURL.Left(LastSlashIndex + 1);
            FString FileName = OriginalURL.Mid(LastSlashIndex + 1);
            FString EncodedFileName = FGenericPlatformHttp::UrlEncode(FileName);
            AssetURL = BaseUrl + EncodedFileName;
        }
        else
        {
            AssetURL = OriginalURL;
        }

        AssetDownloader->StartChunkDownload(AssetURL, AssetFileName, AssetMD5);
        DownloadStatusText->SetText(LOCTEXT("StartDownload", "开始下载..."));
    }
    {
        FScopeLock Lock(&ActiveTasksMutex);
        ActiveTasks.Add(SharedThis(this));

        // // UE_LOG(LogTemp, Warning, TEXT("当前下载的任务数量为： %d"), ActiveTasks.Num())
    }
}

FReply SAssetDownloadWidget::OnPauseClicked()
{
    if (AssetDownloader && !AssetDownloader->IsPaused())
    {
        AssetDownloader->PauseDownload();
        
        if (DownloadStatusText.IsValid())
        {
            DownloadStatusText->SetText(LOCTEXT("DownloadPaused", "下载已暂停"));
        }
        {
            FScopeLock Lock(&ActiveTasksMutex);
            ActiveTasks.Remove(SharedThis(this));
        }
        {
            FScopeLock Lock(&PendingTasksMutex);
            
            ProcessPendingTasks();
            
            PausedTasks.Add(SharedThis(this));
        }
        ProcessPendingTasks();
    }

    bIsPaused = true; 
    
    BeginButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginIcon"));
    PauseButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"));

    return FReply::Handled();
}

FReply SAssetDownloadWidget::OnCancelClicked()
{
    if (AssetDownloader)
    {
        AssetDownloader->CancelDownload();
        
        if (DownloadStatusText.IsValid())
        {
            DownloadStatusText->SetText(LOCTEXT("DownloadCancelled", "下载已取消"));
        }
        
        if (DownloadProgressBar.IsValid())
        {
            DownloadProgressBar->SetPercent(0.0f);
        }
        
        {
            FScopeLock Lock(&ActiveTasksMutex);
            ActiveTasks.Remove(SharedThis(this)); 
        }
        
        if (ParentContainer.IsValid())
        {
            ParentContainer.Pin()->RemoveSlot(SharedThis(this));
        }

        {
            FScopeLock Lock(&PendingTasksMutex);
            ProcessPendingTasks();
        }
    }
    return FReply::Handled();
}

void SAssetDownloadWidget::OnDownloadProgress(float Progress, int64 BytesDownloaded, int64 InTotalBytes)
{
    TotalBytes = InTotalBytes; 
    
    if (DownloadProgressBar.IsValid())
    {
        DownloadProgressBar->SetPercent(Progress);
    }

    if (DownloadStatusText.IsValid())
    {
        // 本地化部分 - 只本地化 "下载中："
        FText DownloadingText = LOCTEXT("Downloading", "下载中：");

        // 进度计算部分
        FString Status;
        if (TotalBytes >= 1024 * 1024 * 1024)
        {
            Status = FString::Printf(TEXT("%.2f/%.2f GB"),
                                     BytesDownloaded / 1024.0 / 1024.0 / 1024.0,
                                     TotalBytes / 1024.0 / 1024.0 / 1024.0);
        }
        else if (TotalBytes >= 1024 * 1024)
        {
            Status = FString::Printf(TEXT("%.2f/%.2f MB"),
                                     BytesDownloaded / 1024.0 / 1024.0,
                                     TotalBytes / 1024.0 / 1024.0);
        }
        else
        {
            Status = FString::Printf(TEXT("%.2f/%.2f KB"),
                                     BytesDownloaded / 1024.0,
                                     TotalBytes / 1024.0);
        }

        // 组合本地化文本和进度
        FText FinalStatus = FText::Format(FText::FromString("{0} {1}"), DownloadingText, FText::FromString(Status));

        // 更新文本
        DownloadStatusText->SetText(FinalStatus);
    }

}


void SAssetDownloadWidget::OnDownloadComplete()
{
    if (DownloadStatusText.IsValid())
    {
        DownloadStatusText->SetText(LOCTEXT("DownloadCompleted", "下载完成！"));
    }
    if (ActiveTasks.Contains(SharedThis(this)))
    {
        ActiveTasks.Remove(SharedThis(this));
    }
    if (ParentContainer.IsValid())
    {
        ParentContainer.Pin()->RemoveSlot(SharedThis(this));
    }
    {
        FScopeLock Lock(&PendingTasksMutex);
        ProcessPendingTasks();
    }
    

    if (OnDownloadCompleted.IsBound())
    {
        OnDownloadCompleted.Execute(AssetFileName);
    }
    
    FString SavePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), AssetFileName);
    
    FString FileSizeText;
    if (TotalBytes < 1024)
    {
        FileSizeText = FString::Printf(TEXT("%d B"), TotalBytes);
    }
    else if (TotalBytes < 1024 * 1024)
    {
        FileSizeText = FString::Printf(TEXT("%.2f KB"), TotalBytes / 1024.0f);
    }
    else if (TotalBytes < 1024 * 1024 * 1024)
    {
        FileSizeText = FString::Printf(TEXT("%.2f MB"), TotalBytes / (1024.0f * 1024.0f));
    }
    else
    {
        FileSizeText = FString::Printf(TEXT("%.2f GB"), TotalBytes / (1024.0f * 1024.0f * 1024.0f));
    }
    
    if (DownloadCompleteWidget.IsValid())
    {
        // UE_LOG(LogTemp, Log, TEXT("DownloadCompleteWidget is valid. Adding completed download..."));
        DownloadCompleteWidget.Pin()->AddCompletedDownload(AssetFileName, SavePath, FileSizeText);
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("DownloadCompleteWidget is invalid!"));
    }
    
    FNotificationInfo Info(LOCTEXT("DownloadComplete", "下载已完成！点击打开保存位置"));
    Info.bFireAndForget = true; 
    Info.FadeOutDuration = 2.0f; 
    Info.ExpireDuration = 5.0f; 
    // Set the hyperlink to open the saved path  设置超链接以打开保存路径
    Info.Hyperlink = FSimpleDelegate::CreateLambda([SavePath]()
    {
        FPlatformProcess::ExploreFolder(*SavePath);
    });
    Info.HyperlinkText = FText::FromString(SavePath);

    //Pop-up notification 弹出通知
    TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
    if (NotificationItem.IsValid())
    {
        NotificationItem->SetCompletionState(SNotificationItem::CS_Success);
    }
}


void SAssetDownloadWidget::OnDownloadError()
{
    if (DownloadStatusText.IsValid())
    {
        DownloadStatusText->SetText(LOCTEXT("DownloadFailed", "下载失败！"));
    }
    ActiveTasks.Remove(SharedThis(this));
    FailedTasks.Add(SharedThis(this));
    {
        FScopeLock Lock(&PendingTasksMutex);
        ProcessPendingTasks();
    }
    
    FNotificationInfo Info(LOCTEXT("DownloadFailed", "下载失败！"));
    Info.bFireAndForget = true; 
    Info.FadeOutDuration = 2.0f;
    Info.ExpireDuration = 5.0f; 


    TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
    if (NotificationItem.IsValid())
    {
        NotificationItem->SetCompletionState(SNotificationItem::CS_Fail); 
    }
}

void SAssetDownloadWidget::SetPausedAllDownloads(bool bPaused)
{
    bIsPausedAllDownloads = bPaused;
}


void SAssetDownloadWidget::ProcessPendingTasks()
{
    if (bIsPausedAllDownloads) 
        {
            return;
        }
    
    // Use the FModuleManager to obtain module instances.使用 FModuleManager 来获取模块实例
    if (FModuleManager::Get().IsModuleLoaded("TestPluginUI"))
    {
        FRSAssetLibraryModule& PluginModule = FModuleManager::GetModuleChecked<FRSAssetLibraryModule>("RSAssetLibrary");
    }
    
    if (PendingTasks.IsEmpty() || ActiveTasks.Num() >= MaxConcurrentDownloads)
    {
        return;
    }

    TSharedPtr<SAssetDownloadWidget> NextTask;
    if (PendingTasks.Dequeue(NextTask))
    {
        if (NextTask.IsValid())
        {
            if (ActiveTasks.Num() < MaxConcurrentDownloads)
            {
                NextTask->StartDownload();
            }
            else
            {
                if (NextTask->DownloadStatusText.IsValid())
                {
                    NextTask->DownloadStatusText->SetText(LOCTEXT("WaitingForDownload", "等待中..."));
                }
                PendingTasks.Enqueue(NextTask);
            }
        }
    }
}



bool SAssetDownloadWidget::IsFileDownloading(const FString& FilePath)
{
    // Extract file name 提取文件名
    FString FileName = FPaths::GetCleanFilename(FilePath);

    // Check the active task queue 检查活跃任务队列
    {
        FScopeLock Lock(&ActiveTasksMutex);
        for (const TSharedPtr<SAssetDownloadWidget>& Task : ActiveTasks)
        {
            if (Task.IsValid() && Task->AssetFileName.Equals(FileName, ESearchCase::IgnoreCase))
            {
                return true; 
            }
        }
    }

    // Check the waiting task queue 检查等待任务队列
    {
        FScopeLock Lock(&PendingTasksMutex);
        TQueue<TSharedPtr<SAssetDownloadWidget>> TempPendingTasks;
        TSharedPtr<SAssetDownloadWidget> Task;

        while (!PendingTasks.IsEmpty())
        {
            PendingTasks.Dequeue(Task);

            if (Task.IsValid() && Task->AssetFileName.Equals(FileName, ESearchCase::IgnoreCase))
            {
                TempPendingTasks.Enqueue(Task); 
                PendingTasks.Enqueue(Task);   
                return true; 
            }

            TempPendingTasks.Enqueue(Task); // Save state 保存状态
        }

        // Recovery queue 恢复队列
        while (!TempPendingTasks.IsEmpty())
        {
            TempPendingTasks.Dequeue(Task);
            PendingTasks.Enqueue(Task);
        }
    }

    // Check the suspended task queue 检查暂停任务队列
    {
        FScopeLock Lock(&PausedTasksMutex);
        for (const TWeakPtr<SAssetDownloadWidget>& WeakTask : PausedTasks)
        {
            if (TSharedPtr<SAssetDownloadWidget> Task = WeakTask.Pin())
            {
                if (Task->AssetFileName.Equals(FileName, ESearchCase::IgnoreCase))
                {
                    return true; 
                }
            }
        }
    }

    return false; // The file is not in any queue 文件未在任何队列中
}



#undef LOCTEXT_NAMESPACE