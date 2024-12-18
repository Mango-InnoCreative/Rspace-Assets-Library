// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectContent/AssetDownloader/SDownloadCompleteWidget.h"

#include "RSAssetLibraryStyle.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/FileManager.h"


#define LOCTEXT_NAMESPACE "SDownloadCompleteWidget"

void SDownloadCompleteWidget::Construct(const FArguments& InArgs)
{
    // Create the vertical box to hold the completed download entries
    ChildSlot
    [
        SNew(SScrollBox)
        + SScrollBox::Slot()
        [
            SAssignNew(CompletedDownloadsContainer, SVerticalBox)
        ]
    ];
    
    // Load the downloaded files in the folder each time you open the plugin 每次打开插件时加载文件夹中的已下载文件
    LoadCompletedDownloadsFromFolder();
}

FString SDownloadCompleteWidget::TruncateText(const FString& OriginalText, int32 MaxLength)
{
    int32 Length = 0;  
    FString Result;

    for (int32 Index = 0; Index < OriginalText.Len(); ++Index)
    {
        TCHAR Character = OriginalText[Index];

        // Determine whether it is Chinese character 判断是否是汉字（中文字符的范围通常在 Unicode 的0x4E00 - 0x9FFF）
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

void SDownloadCompleteWidget::AddCompletedDownload(const FString& FileName, const FString& FilePath, const FString& FileSizeText)
{
    TSharedRef<SHorizontalBox> CompletedDownloadHorizontalBox = SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .Padding(10, 5, 5, 5)
        .FillWidth(1.0f) 
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TruncateText(FileName, 20)))
                .ColorAndOpacity(FSlateColor(FLinearColor::White))
            ]
        ]
    
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(70, 5, 5, 5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(10, 5, 5, 5)
            [
                SNew(SButton)
                .Cursor(EMouseCursor::Hand)
                .ButtonStyle(&SearchButtonStyle)
                .ContentPadding(0) 
                .ToolTipText(LOCTEXT("ViewLocalCacheToolTip", "查看本地缓存"))
                .OnClicked_Lambda([FilePath]() -> FReply
                {
                    // Default path: The RspaceAssetsLibrary folder in the project Saved folder 默认路径：项目 Saved 文件夹下的 RspaceAssetsLibrary 文件夹
                    FString DefaultPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"));

                    // If FilePath does not exist or is empty, the default path is used 如果 FilePath 不存在或为空，则使用默认路径 
                    FString PathToExplore = (!FPaths::FileExists(FilePath)) ? DefaultPath : FilePath;

                    // Convert to an absolute path 转换为绝对路径 
                    FString AbsolutePath = FPaths::ConvertRelativePathToFull(PathToExplore);

                    // Open folder 打开文件夹
                    FPlatformProcess::ExploreFolder(*AbsolutePath);

                    return FReply::Handled();
                })

            ]
        ]
    
        + SHorizontalBox::Slot()
         .Padding(98, 5, 5, 5)
         .FillWidth(1.0f) 
         .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(FText::FromString(FileSizeText))
            .ColorAndOpacity(FSlateColor(FLinearColor::Gray))
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
                   SNew(SImage)
                   .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadComplete"))
               ]
           ]
       ];
    
    if (CompletedDownloadsContainer.IsValid())
    {
        CompletedDownloadsContainer->InsertSlot(0)
        .AutoHeight()
        .Padding(5)
        [
            CompletedDownloadHorizontalBox
        ];
    }

    // Store item information 存储条目信息
    FCompletedDownloadEntry NewEntry = {CompletedDownloadHorizontalBox, FileName, FilePath};
    CompletedDownloads.Add(NewEntry);
    
    DeleteButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Deleteiconnormal"));
    DeleteButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Deleteiconclicked"));
    DeleteButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Deleteiconclicked"));

    SearchButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Serchiconnormal"));
    SearchButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Serchiconclicked"));
    SearchButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Serchiconclicked"));
}


void SDownloadCompleteWidget::LoadCompletedDownloadsFromFolder()
{
    // Define folder path 定义文件夹路径
    FString FolderPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"));

    // Check whether the folder exists 检查文件夹是否存在
    if (!FPaths::DirectoryExists(FolderPath))
    {
        return; // 如果文件夹不存在，直接返回
    }

    // Gets all the files in the folder 获取文件夹中的所有文件
    IFileManager& FileManager = IFileManager::Get();
    TArray<FString> FileNames;
    FileManager.FindFiles(FileNames, *FolderPath, TEXT("*.*")); // Matches all file types 匹配所有文件类型

    // Traverse the files in the folder 遍历文件夹中的文件
    for (const FString& FileName : FileNames)
    {
        if (FileName.Equals(TEXT("AssetsManagement.csv"), ESearchCase::IgnoreCase))
        {
            continue;
        }

        // Construct the complete file path 构造完整的文件路径
        FString FilePath = FPaths::Combine(FolderPath, FileName);
        
        int64 FileSize = FileManager.FileSize(*FilePath);
        FString FileSizeText;
        
        if (FileSize < 1024)
        {
            FileSizeText = FString::Printf(TEXT("%d B"), FileSize);
        }
        else if (FileSize < 1024 * 1024)
        {
            FileSizeText = FString::Printf(TEXT("%.2f KB"), FileSize / 1024.0f);
        }
        else if (FileSize < 1024 * 1024 * 1024)
        {
            FileSizeText = FString::Printf(TEXT("%.2f MB"), FileSize / (1024.0f * 1024.0f));
        }
        else
        {
            FileSizeText = FString::Printf(TEXT("%.2f GB"), FileSize / (1024.0f * 1024.0f * 1024.0f));
        }

        // Call AddCompletedDownload to add the file information to the UI 调用 AddCompletedDownload 将文件信息添加到 UI 中
        AddCompletedDownload(FileName, FilePath, FileSizeText);
    }
}


#undef LOCTEXT_NAMESPACE