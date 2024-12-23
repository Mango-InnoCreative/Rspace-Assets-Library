// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectContent/VideoAssets/VideoAssetsWidget.h"
#include "RSAssetLibraryStyle.h"
#include "ProjectContent/Imageload/FImageLoader.h"
#include "VideoLibrary/GetVideoCommentListApi.h"
#include "Widgets/Layout/SScrollBox.h"
#include "FileMediaSource.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "ProjectContent/MediaPlayer/SVideoPlayerWidget.h"
#include "VideoLibrary/GetVideoAssetLibraryListInfoApi.h"
#include "VideoLibrary/GetVideoFileVersionInfoApi.h"
#include "VideoLibrary/GetVideoVersionFileInfoApi.h"
#include "Widgets/Layout/SScaleBox.h"

#define LOCTEXT_NAMESPACE "SVideoAssetsWidget"

void SVideoAssetsWidget::Construct(const FArguments& InArgs)
{
    OnVideoAssetClicked = InArgs._OnVideoAssetClicked;
    OnSelectedVideoDownloadClicked = InArgs._OnVideoDownload;
    OnVideoNothingToShow = InArgs._OnVideoNothingToShow;
    
    AssetDownloadWidget = SNew(SAssetDownloadWidget);
    
    if (AssetDownloadWidget.IsValid())
    {
        AssetDownloadWidget->BindOnDownloadCompleted(FOnDownloadCompleted::CreateSP(this, &SVideoAssetsWidget::OnDownloadCompleted));
    }

    FSlateFontInfo DetailsTitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 12);
    FSlateFontInfo DetailsMsgFont = FCoreStyle::GetDefaultFontStyle("Regular", 12);
    
	ChildSlot
	[
	    SNew(SBorder)
	    .BorderImage(new FSlateRoundedBoxBrush(FLinearColor(0.011612, 0.011612, 0.011612, 1.0f), 6.0f)) 
        .Padding(FMargin(1)) 
        .Padding(2,2,2,0) 
        [
            SNew(SScrollBox)
           + SScrollBox::Slot()
           [
		     SAssignNew(VideoAssetsContainer, SVerticalBox)
		    ]   
	    ]
	];

    InitializeDetailButtonStyle();
}


void SVideoAssetsWidget::UpdateVideoAssets(const TArray<FVideoAssetInfo>& VideoAssetsData)
{
    VideoAssetsContainer->ClearChildren();

    TSharedPtr<SHorizontalBox> CurrentRow;
    int32 ItemCount = 0;
    
    int32 FileCount = 0;
    for (const FVideoAssetInfo& VideoFileItem : VideoAssetsData)
    {
        if (VideoFileItem.fileType == 1) // 1 Indicates the folder type 1 表示文件夹类型
        {
            FileCount++;
        }
    }
    
        for (const FVideoAssetInfo& VideoFileItem : VideoAssetsData)
        {
            TSharedRef<SWidget> IconWidget = SNew(SBox);

            TSharedPtr<FButtonStyle> FolderButtonStyle = MakeShareable(new FButtonStyle(DetailClickedButtonStyle));
            

            if (VideoFileItem.fileType != 1)
            {
                continue;
            }
            
            IconWidget = ConstructImageItem(VideoFileItem.thumRelativePatch);
            
            auto OnClicked = [this, VideoFileItem, FolderButtonStyle]() -> FReply
            {
                if (SelectedButtonStyle.IsValid())
                {
                    SelectedButtonStyle->SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ContentBorder"));
                    SelectedButtonStyle->SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButtonClicked"));
                    SelectedButtonStyle->SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButton"));
                }

                FolderButtonStyle->SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButton"));
                FolderButtonStyle->SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButtonClicked"));
                
                SelectedButtonStyle = FolderButtonStyle;
                
                if (OnVideoAssetClicked.IsBound())
                {
                    if(UGetVideoVersionFileInfoApi* GetVideoVersionFileInfoApi = NewObject<UGetVideoVersionFileInfoApi>())
                    {
                        FOnGetVideoVersionFileInfoResponse OnResponseDelegate;
                        OnResponseDelegate.BindLambda([this, VideoFileItem](UGetVideoVersionFileInfoData* VideoVersionFileDetail)
                        {
                            InitialVideoVersion.Empty();
                            if (VideoVersionFileDetail)
                            {
                               
                                InitialVideoVersion = VideoVersionFileDetail->data.VersionNo;
                                InitialURL = VideoVersionFileDetail->data.VideoFilePath;

                                TSharedRef<SWidget> DetailsWidget = GenerateDetailsWidget(VideoVersionFileDetail);
                                        
                                OnVideoAssetClicked.Execute(VideoFileItem, DetailsWidget);
                            }
                            else
                            {
                                // UE_LOG(LogTemp, Warning, TEXT("视频版本文件详情为空！"));
                            }
                        });
                        
                        SetUserAndProjectParams();
                        FString AuditNo = VideoFileItem.auditNo;
                        GetVideoVersionFileInfoApi->SendGetVideoVersionFileInfoRequest(Ticket, Uuid, AuditNo, OnResponseDelegate);
                    }
                    
                     if(UGetVideoFileVersionInfoApi* GetVideoFileVersionInfoApi = NewObject<UGetVideoFileVersionInfoApi>())
                        {
                            FOnGetVideoFileVersionInfoResponse OnResponseDelegate;
                            OnResponseDelegate.BindLambda([this, VideoFileItem](const FGetVideoFileVersionInfoData& VideoVersionList)
                            {
                                if (VideoVersionList.Data.Num() > 0)
                                {
                                    VersionOptions.Empty();
                        
                                    for (const FVideoFileVersionInfo& VideoVersion : VideoVersionList.Data)
                                    {
                                        VersionToAuditNoMap.Add(VideoVersion.VersionNo , VideoVersion.AuditNo);
                                        VersionOptions.Add(MakeShared<FString>(VideoVersion.VersionNo));
                                    }
                                    // Notify the drop-down box to update options 通知下拉框更新选项
                                    if (VersionComboBox.IsValid())
                                    {
                                        VersionComboBox->RefreshOptions();
                                    }
                                    
                                }
                            });
                            SetUserAndProjectParams();
                            FString FileNo = VideoFileItem.fileNo;
                            GetVideoFileVersionInfoApi->SendGetVideoFileVersionInfoRequest(Ticket, FileNo, OnResponseDelegate);
                        }
                
                }
                return FReply::Handled();
            };

            if (ItemCount % 5 == 0)
            {
                CurrentRow = SNew(SHorizontalBox);
                VideoAssetsContainer->AddSlot().AutoHeight().Padding(5)[ CurrentRow.ToSharedRef() ];
            }
            

            CurrentRow->AddSlot()
            .AutoWidth()
            .Padding(5, 10, 5, 0)
            [
                SNew(SButton)
                .ButtonStyle(FolderButtonStyle.Get())
                .Cursor(EMouseCursor::Hand)
                .OnClicked_Lambda(OnClicked)
                [
                     SNew(SBox)
                    .WidthOverride(160.0f)  
                    .HeightOverride(160.0f) 
                    [
                        SNew(SVerticalBox)
                        + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 16, 0, 0)
                        [
                            SNew(SBox)
                            .WidthOverride(150.0f).HeightOverride(100.0f)
                            [
                                IconWidget 
                            ]
                        ]
                        + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).VAlign(VAlign_Bottom).Padding(0, 0, 0, 0)
                        [
                            SNew(SBox).WidthOverride(150.0f).HeightOverride(30.0f)
                            [
                                SNew(SBorder)
                                .BorderImage(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.FileBorder"))
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center) 
                                [
                                    SNew(STextBlock)
                                    .Text(FText::FromString(TruncateText(VideoFileItem.fileName, 14)))
                                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                                    .Justification(ETextJustify::Center)
                                ]
                            ]
                        ]
                    ]
                ]
            ];

            ItemCount++;
        }
    
}

void SVideoAssetsWidget::InitializeDetailButtonStyle()
{
    DetailClickedButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ContentBorder"));
    DetailClickedButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButtonClicked"));
    DetailClickedButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButton"));
}


FString SVideoAssetsWidget::TruncateText(const FString& OriginalText, int32 MaxLength)
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

void SVideoAssetsWidget::SetUserAndProjectParams()
{
    USMSubsystem = GEditor->GetEditorSubsystem<UUSMSubsystem>();
    Ticket = USMSubsystem->GetCurrentUserAndProjectInfo().Ticket;
    Uuid = USMSubsystem->GetCurrentUserAndProjectInfo().Uuid;
    ProjectNo = USMSubsystem->GetSelectedProject().projectNo;
}


TSharedRef<SWidget> SVideoAssetsWidget::GenerateDetailsWidget(UGetVideoVersionFileInfoData* VideoVersionFileDetail)
{
    ImportButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadBarBorder"));
    ImportButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ImportHover"));
    ImportButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Importclicked"));

    PlayCircleButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayCircle"));
    PlayCircleButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayCircleHover"));
    PlayCircleButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayCircleHover"));
    
    Videourl = VideoVersionFileDetail->data.VideoFilePath;
   VideoImage = VideoVersionFileDetail->data.CoverImg;
    VideoNo = VideoVersionFileDetail->data.AuditNo;
    VideoFileName = VideoVersionFileDetail->data.VideoFileName;
    
    CommentContainer = SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("LoadingComments", "Loading comments..."))
        ];

    // Logic for requesting comment data 请求评论数据的逻辑
    if (UGetVideoCommentListApi* GetVideoCommentListApi = NewObject<UGetVideoCommentListApi>())
    {
        FOnGetVideoCommentListResponse OnGetVideoCommentListResponse;
        OnGetVideoCommentListResponse.BindLambda([this](const FGetVideoCommentListResponseData& VideoCommentList)
        {
            CommentContainer->ClearChildren();

            if (VideoCommentList.Data.Num() > 0)
            {
                int32 Index = 1;
                for (const FVideoComment& VideoComment : VideoCommentList.Data)
                {
                    CommentContainer->AddSlot()
                    .AutoHeight()
                    .Padding(0, 20, 0, 5)
                    [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew(SOverlay)
                        + SOverlay::Slot()
                        .HAlign(HAlign_Left)
                        .VAlign(VAlign_Top)
                        [
                            SNew(SImage)
                            .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.CommentCircle"))  
                        ]
                        + SOverlay::Slot()
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Top)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(FString::FromInt(Index)))
                            .ColorAndOpacity(FSlateColor(FLinearColor::White))
                        ]
                        
                    ]
                    + SHorizontalBox::Slot()
                    .FillWidth(1)
                    .Padding(FMargin(5.0, 0.0, 0.0, 0.0))
                    [
                        SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SHorizontalBox)
                            + SHorizontalBox::Slot()
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoComment.UserName))
                                .Justification(ETextJustify::Left)
                            ]
                            + SHorizontalBox::Slot()
                            [
                                SNew(STextBlock)
                               .Text(FText::FromString(VideoComment.CreateTime))
                               .Justification(ETextJustify::Right)
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .Padding(FMargin(0.0, 15.0, 0.0, 0.0)) 
                        [
                        SNew(STextBlock)
                        .Text(FText::FromString(VideoComment.CommentContent))
                        .ColorAndOpacity(FSlateColor(FLinearColor::White))
                        .AutoWrapText(true) 
                        .WrapTextAt(600.0f)
                        ]
                    ]
                    ];

                    // UE_LOG(LogTemp, Error, TEXT("视频评论接口响应成功：视频编号: %s; 评论内容：%s"), *VideoComment.AuditNo, *VideoComment.CommentContent);

                    Index++;
                }
            }
            else
            {
                CommentContainer->AddSlot()
                .AutoHeight()
                .Padding(0, 20, 0, 5)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("NoComments", "No comments"))
                    .AutoWrapText(true)
                ];
            }
        });

        FString InUuid = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Uuid;
        FString InTicket = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Ticket;
        GetVideoCommentListApi->SendGetVideoCommentListRequest(InTicket, InUuid, VideoNo, OnGetVideoCommentListResponse);
    }
    
    TSharedPtr<FSlateBrush> ItemBrush = MakeShareable(new FSlateBrush());
    
    SelectedVersion = MakeShared<FString>(InitialVideoVersion); // Initializes the selected item 初始化选中项

    bool bIsFileDownloaded = IsFileDownloaded(VideoFileName);

    return SNew(SVerticalBox)
    
    + SVerticalBox::Slot()
    .FillHeight(1.0)
    .Padding(5, 0, 0, 0)
    [
        SNew(SBorder)
        .Padding(0)
        .BorderImage(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DetailsBorder"))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0)
            [
                SNew(SBox)
                .WidthOverride(390) 
                .HeightOverride(240) 
                [
                    SNew(SOverlay)
                    
                    + SOverlay::Slot()
                    .HAlign(HAlign_Fill)
                    .VAlign(VAlign_Fill)
                    [
                        ConstructImageItem(VideoImage)
                    ]
                    
                    + SOverlay::Slot()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Bottom)
                    .Padding(10)  
                    [
                       SNew(SButton)
                       .Cursor(EMouseCursor::Hand)
                        .ButtonStyle(&PlayCircleButtonStyle) 
                        .ContentPadding(0)
                       .OnClicked_Lambda([this]() -> FReply {
                           //// UE_LOG(LogTemp, Log, TEXT("VideoImage: %s"), *VideoImage);
                           return PlayMediaWithSystemPlayer(Videourl);
                       })
                     
                    ]
                ]
            ]
                
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 0, 0, 0)
            [
                SNew(SBox)
               .HeightOverride(50)
               [
                   SNew(SBorder)
                   .Padding(FMargin(0))
                   .BorderImage(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DetailsNameBorder"))  
                   [
                       SNew(SBox)
                       .Padding(15, 15, 0, 0)
                       [
                           SNew(STextBlock)
                           .Text(FText::FromString(TruncateText(VideoFileName, 26)))
                           .Font(FCoreStyle::GetDefaultFontStyle("Bold", 15))
                           .Justification(ETextJustify::Left)
                           .ColorAndOpacity(FSlateColor(FLinearColor::White))
                       ]
                   ]
               ]
            ]
                    
                + SVerticalBox::Slot()
                .FillHeight(1)
                .Padding(0, 20, 0, 0)
                [
                    SNew(SScrollBox)
                    + SScrollBox::Slot()
                    [
                        SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 0, 0, 15)
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("BasicInfo", "Base Info"))
                            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                            .ColorAndOpacity(FSlateColor(FLinearColor::White))
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("Format", "Format"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f)  
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.FileInfo.FileFormat))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White)) 
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("Uploader", "Upload By"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f)  
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.FileInfo.CreateUserName))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("UploadTime", "Upload Time"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f)  
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.CreateTime))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("FileSize", "File Size"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f) 
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.FileInfo.FileSize))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("Resolution", "Resolution"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f)  
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.FileInfo.SampleRate))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("FrameRate", "Frame Rate"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f)  
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.FileInfo.FrameRate))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("VideoBitRate", "Bitrate"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f)  
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.FileInfo.VideoBiteRate))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("Duration", "Duration"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f) 
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.FileInfo.FileTime))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("Codec", "Video Encoding"))
                                .Justification(ETextJustify::Left)
                            ]

                               + SHorizontalBox::Slot()
                            .FillWidth(1.0f)
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.FileInfo.VideoEncoder))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("AudioChannel", "Channels"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f)  
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(FString::FromInt(VideoVersionFileDetail->data.FileInfo.AudioChannel)))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("AudioSampleRate", "Sampling Rate"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f) 
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.FileInfo.AudioHarvestRate))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("AudioBitRate", "Audio Bitrate"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f)  
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.FileInfo.AudioBiteRate))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SHorizontalBox)

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("AudioCodec", "Audio Encoding"))
                                .Justification(ETextJustify::Left)
                            ]

                            + SHorizontalBox::Slot()
                            .FillWidth(1.0f) 
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(VideoVersionFileDetail->data.FileInfo.AudioEncoder))
                                .Justification(ETextJustify::Right)
                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(FMargin(16.0f, 10.0f, 20.0f, 5.0f)) 
                        [
                            SNew(SBox)
                            [
                                SNew(SImage)
                                .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ProjectLine")) 
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(25, 15, 25, 15)
                        [
                            SNew(SBox)
                            .Padding(0)
                            [
                                SNew(SVerticalBox)
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("Comments", "Comments"))
                                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                    .Justification(ETextJustify::Left)
                                ]
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                [
                                    CommentContainer.ToSharedRef()
                                ]
                            ]
                        ]
                ]
            ]

             + SVerticalBox::Slot()
            .AutoHeight()
            .VAlign(VAlign_Bottom)
            .Padding(0)
            [
                SNew(SBorder)
                .BorderImage(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadBarBorder"))  
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                   .FillWidth(1.0f)
                   .HAlign(HAlign_Fill)
                   .Padding(10, 2, 10, 2)
                   [
                       SNew(SBox)
                       .HeightOverride(40)
                       .WidthOverride(210)
                       [
                            SAssignNew(VersionComboBox, SComboBox<TSharedPtr<FString>>)  
                            .OptionsSource(&VersionOptions)  // Set the option source for the drop-down box 设置下拉框的选项源
                            .OnGenerateWidget_Lambda([](TSharedPtr<FString> InOption)
                            {
                               // Precede the options with a "V" 在选项内容前面加上 "V"
                               FString DisplayText = FString::Printf(TEXT("%s"), **InOption);
                               return SNew(STextBlock)
                                   .Text(FText::FromString(DisplayText));  // Display content for each option 每个选项的显示内容
                            })
                            .OnSelectionChanged(this, &SVideoAssetsWidget::OnVersionSelected)  // Version selection event 版本选择事件
                            .InitiallySelectedItem(nullptr)  // InitiallySelectedItem is not dependent 不依赖 InitiallySelectedItem
                            .Content()
                            [
                               SNew(STextBlock)
                               .Text_Lambda([this]()
                                {
                                    // If any item is selected, the selected item is displayed. Otherwise, the default text is displayed 如果有选中项，则显示选中项；否则显示默认文字
                                   return FText::FromString(
                                        SelectedVersion.IsValid() ? **SelectedVersion : *InitialVideoVersion
                                    );
                                })
                            ]
                        ]
                   ]
                   
                    + SHorizontalBox::Slot()
                    .HAlign(HAlign_Right) 
                    .FillWidth(1.0f)
                    [
                         SNew(SHorizontalBox)
                         + SHorizontalBox::Slot()
                         .AutoWidth()
                         .Padding(5, 2, 5, 2)
                         [
                            SAssignNew(DownloadButton, SButton)  
                             .Cursor(EMouseCursor::Hand)
                             .ButtonStyle(&ImportButtonStyle) 
                             .ContentPadding(0) 
                             .IsEnabled_Lambda([this]() { return bIsButtonEnabled && !IsFileDownloaded(VideoFileName); })
                             .OnClicked_Lambda([this, VideoVersionFileDetail]()-> FReply
                            {
                               float CurrentTime = FPlatformTime::Seconds();
                                 
                               if (CurrentTime - LastClickTime < ClickCooldownTime)
                               {
                                   return FReply::Handled(); 
                               }
                                 
                               LastClickTime = CurrentTime;
                                 
                            bIsButtonEnabled = false;  
                                
                            AnimationProgress = 0.0f;
                            const float AnimationDuration = 1.0f;
                                 
                            FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this, AnimationDuration](float DeltaTime) -> bool
                            {
                                AnimationProgress += DeltaTime / AnimationDuration;

                                if (AnimationProgress >= 1.0f)
                                {
                                    AnimationProgress = 1.0f;

                                    return false; 
                                }

                                return true;
                            }));
                                 
                           SelectedVideoFileInfo.FileName = VideoVersionFileDetail->data.VideoFileName;
                             SelectedVideoFileInfo.FileUrl = VideoVersionFileDetail->data.VideoFilePath;
                           SelectedVideoFileInfo.FileMd5 = VideoVersionFileDetail->data.FileInfo.FileMd5;

                           if (VideoVersionFileDetail->data.FileInfo.FileSize == "0")
                           {
                               return FReply::Handled();
                           }
                                 
                           OnDownloadVideoButtonClicked(SelectedVideoFileInfo);

                           bool bIsFileDownloaded = IsFileDownloaded(VideoFileName);

                           if (bIsFileDownloaded)
                           {
                               bIsButtonEnabled = false;
                           }
                           else
                           {
                               bIsButtonEnabled = true;
                           }
                                 
                           return FReply::Handled();
                            
                        })
                             [
                                 SNew(SBox)
                                 .WidthOverride(40)
                                 .HeightOverride(40)
                                 [
                                     SNew(SHorizontalBox)
                                     + SHorizontalBox::Slot()
                                     .VAlign(VAlign_Center)
                                     .HAlign(HAlign_Center)
                                     .Padding(5, 5, 5, 5)
                                     [
                                         SNew(SOverlay)
                                        + SOverlay::Slot() 
                                        [
                                            SNew(SImage)
                                            .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DetailDownloadIcon"))
                                        ]
                                        + SOverlay::Slot()
                                        [
                                            SNew(SImage)
                                            .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DetailDownloadIcon"))
                                            .RenderTransform_Lambda([this]()
                                            {
                                                return FSlateRenderTransform(FVector2D(0.0f, -200.0f * AnimationProgress));
                                            })
                                            .ColorAndOpacity_Lambda([this]()
                                            {
                                                return FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f - AnimationProgress));
                                            })
                                        ]
                                     ]
                                     // + SHorizontalBox::Slot()
                                     // .VAlign(VAlign_Center)
                                     // .HAlign(HAlign_Center)
                                     // .Padding(0, 5, 5, 5)
                                     // [
                                     //     SNew(STextBlock)
                                     //     // .Text(LOCTEXT("Download", "下载"))
                                     //     .Justification(ETextJustify::Center)
                                     //     .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                                     //     .ColorAndOpacity(FSlateColor(FLinearColor(0.0f, 0.6f, 0.45f))) 
                                     // ]
                                 ]
                             ]
                         ]
                          + SHorizontalBox::Slot()
                          .HAlign(HAlign_Right)
                         .AutoWidth()
                         .Padding(5, 2, 5, 2)
                         [
                             SNew(SButton)
                             .Cursor(EMouseCursor::Hand)
                             .ButtonStyle(&ImportButtonStyle) 
                             .OnClicked(this, &SVideoAssetsWidget::OnImportVideoFileButtonClicked)
                             .ContentPadding(0) 
                             [
                                 SNew(SBox)
                                 .WidthOverride(40)
                                 .HeightOverride(40)
                                 [
                                     SNew(SHorizontalBox)
                                     + SHorizontalBox::Slot()
                                     .VAlign(VAlign_Center)
                                     .HAlign(HAlign_Center)
                                     .Padding(5, 5, 5, 5)
                                     [
                                         SNew(SImage)
                                         .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ImportIcon")) 
                                     ]
                                     // + SHorizontalBox::Slot()
                                     // .VAlign(VAlign_Center)
                                     // .HAlign(HAlign_Center)
                                     // .Padding(0, 5, 5, 5)
                                     // [
                                     //     SNew(STextBlock)
                                     //     // .Text(LOCTEXT("Import", "导入"))
                                     //     .Justification(ETextJustify::Center)
                                     //     .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                                     //     .ColorAndOpacity(FSlateColor(FLinearColor(0.0f, 0.6f, 0.45f)))
                                     // ]
                                 ]
                             ]
                         ]
                    ]
                ]
            ]
        ]
    ];
    
}

void SVideoAssetsWidget::OnVersionSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (NewSelection.IsValid())
    {
        SelectedVersion = NewSelection;

        FString SelectedVersionString = **NewSelection;
        
        // Obtain the corresponding relativePath from the VersionToRelativePathMap 从 VersionToRelativePathMap 中获取对应的 relativePath
         if (VersionToAuditNoMap.Contains(SelectedVersionString))
        {
            SelectedAuditNo = MakeShared<FString>(VersionToAuditNoMap[SelectedVersionString]);
           
             if(UGetVideoVersionFileInfoApi* GetVideoVersionFileInfoApi = NewObject<UGetVideoVersionFileInfoApi>())
             {
                 FOnGetVideoVersionFileInfoResponse OnResponseDelegate;
                 OnResponseDelegate.BindLambda([this](UGetVideoVersionFileInfoData* SelectedVideoVersionFileDetail)
                 {
                     if (SelectedVideoVersionFileDetail)
                     {
                         InitialVideoVersion = SelectedVideoVersionFileDetail->data.VersionNo;
                       TSharedRef<SWidget> SelectedDetailsWidget = GenerateDetailsWidget(SelectedVideoVersionFileDetail);
                        
                       // Passed to the parent class by delegation 通过委托传递给父类
                       if (OnVedioUpdateDetailsBar.IsBound())
                       {
                           // Update the version content selected in the drop-down box 更新下拉框选中的版本内容
                            SelectedVersion = MakeShared<FString>(InitialVideoVersion);
                           OnVedioUpdateDetailsBar.Execute(SelectedDetailsWidget); // Call the UpdateDetailsBar of the parent class 调用父类的 UpdateDetailsBar
                       }
                         
                     }
                     else
                     {
                         // UE_LOG(LogTemp, Warning, TEXT("视频版本文件详情为空！"));
                     }
                 });
                 
                 SetUserAndProjectParams();
                 // Dereference SelectedAuditNo to get the FString value 解引用 SelectedAuditNo 来获取 FString 值
                 FString SAuditNo = SelectedAuditNo.IsValid() ? *SelectedAuditNo : TEXT("");
                 GetVideoVersionFileInfoApi->SendGetVideoVersionFileInfoRequest(Ticket, Uuid, SAuditNo, OnResponseDelegate);
             }
        }
    }
}

bool SVideoAssetsWidget::IsFileDownloaded(const FString& ForbidFileName) const
{
    // Construct the full path to the file 构造文件的完整路径
    FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), ForbidFileName);
    
    // Check whether the file exists 检查文件是否存在
    return FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath);
}

void SVideoAssetsWidget::OnDownloadCompleted(const FString& AssetFileName)
{
    // If the downloaded file name matches the current file, the download button is disabled 如果下载的文件名与当前文件匹配，则禁用下载按钮
    if (AssetFileName == VideoFileName)
    {
        if (DownloadButton.IsValid())
        {
            DownloadButton->Invalidate(EInvalidateWidget::LayoutAndVolatility);
        }
    }
}


FReply SVideoAssetsWidget::PlayMediaWithSystemPlayer(const FString& InPath)
{
    FMediaPlayerManager::PlayMediaWithSystemPlayer(InPath); 
    
    return FReply::Handled();
}

FReply SVideoAssetsWidget::OnDownloadVideoButtonClicked(FSelectedVideoFileInfo VideoInfo)
{

    if (OnSelectedVideoDownloadClicked.IsBound())
    {
        OnSelectedVideoDownloadClicked.Execute(VideoInfo.FileName, VideoInfo.FileUrl, VideoInfo.FileMd5);
    }
    
    return FReply::Handled();
}

FReply SVideoAssetsWidget::OnImportVideoFileButtonClicked()
{
   
    FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), VideoFileName);

    // 调用导入函数
    ImportVideoFile(FilePath);

    return FReply::Handled();
}


void SVideoAssetsWidget::ImportVideoFile(const FString& FilePath)
{
    static TSharedPtr<SWindow> ExistingNotificationWindow;
    static TSharedPtr<SWindow> ExistingNotificationWindow2;
    
    // Check if the file is still being downloaded 检查文件是否仍在下载中
    if (AssetDownloadWidget.IsValid() && AssetDownloadWidget->IsFileDownloading(FilePath))
    {
        if (ExistingNotificationWindow.IsValid() && FSlateApplication::Get().FindWidgetWindow(ExistingNotificationWindow.ToSharedRef()))
        {
            ExistingNotificationWindow->BringToFront();
            return;
        }

        // An error message is displayed indicating that the file is still being downloaded 显示错误消息，提示文件仍在下载中
        ExistingNotificationWindow = SNew(SWindow)
            .Title(LOCTEXT("TipTitle", "Notification"))
            .ClientSize(FVector2D(200, 100))
            .FocusWhenFirstShown(true)
            .SupportsMaximize(false)
            .SupportsMinimize(false)
            .IsTopmostWindow(false)
            .Content()
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                .HAlign(HAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("ResourceDownloading", "Resource is downloading!"))
                    .Justification(ETextJustify::Center)
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10, 20, 10, 10)
                .HAlign(HAlign_Center)
                [
                    SNew(SButton)
                    .Text(LOCTEXT("Confirm", "OK"))
                    .OnClicked_Lambda([]() -> FReply {
                        if (ExistingNotificationWindow.IsValid())
                        {
                            FSlateApplication::Get().RequestDestroyWindow(ExistingNotificationWindow.ToSharedRef());
                            ExistingNotificationWindow.Reset();
                        }
                        return FReply::Handled();
                    })
                ]
            ];

        ExistingNotificationWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>&)
        {
              ExistingNotificationWindow.Reset(); 
        }));
        
        FSlateApplication::Get().AddWindow(ExistingNotificationWindow.ToSharedRef());
        return;
    }
    
    if (!FPaths::FileExists(FilePath))
    {
        // UE_LOG(LogTemp, Error, TEXT("Concept file does not exist at: %s"), *FilePath);
        
        if (ExistingNotificationWindow2.IsValid() && FSlateApplication::Get().FindWidgetWindow(ExistingNotificationWindow2.ToSharedRef()))
        {
            ExistingNotificationWindow2->BringToFront();
            return;
        }
        
        ExistingNotificationWindow2 = SNew(SWindow)
            .Title(LOCTEXT("TipTitle", "Notification"))
            .ClientSize(FVector2D(200, 100))
            .FocusWhenFirstShown(true)
            .SupportsMaximize(false)
            .SupportsMinimize(false)
            .IsTopmostWindow(false)
            .Content()
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(10)
            .HAlign(HAlign_Center)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("DownloadResourceMessage", "Download the resource first!"))
                .Justification(ETextJustify::Center)
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(10, 20, 10, 10)
            .HAlign(HAlign_Center)
            [
                SNew(SButton)
                .VAlign(VAlign_Bottom)
                .Text(LOCTEXT("Confirm", "OK"))
                .OnClicked_Lambda([]() -> FReply
                {
                    if (ExistingNotificationWindow2.IsValid())
                    {
                        FSlateApplication::Get().RequestDestroyWindow(ExistingNotificationWindow2.ToSharedRef());
                        ExistingNotificationWindow2.Reset();
                    }

                    return FReply::Handled();
                })
            ]
        ];
        FSlateApplication::Get().AddWindow(ExistingNotificationWindow2.ToSharedRef());
        ExistingNotificationWindow2->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>&)
        {
            ExistingNotificationWindow2.Reset(); 
        }));
        
        return;
    }

    // Gets the destination file name and replaces illegal characters (Spaces replaced with underscores) 获取目标文件名并替换非法字符（空格替换为下划线）
    FString TargetFileName = FPaths::GetBaseFilename(FilePath).Replace(TEXT(" "), TEXT("_")).Replace(TEXT("."), TEXT("_"));

    // Construct save path 构造保存路径
    FString SavePath = TEXT("/Game/RSpaceImportedAssets/Videos/") + TargetFileName;

    // Check whether the package already exists 检查包是否已经存在
    if (FPackageName::DoesPackageExist(SavePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Asset already exists at: %s"), *SavePath);
        return;
    }

    // Create package 创建包
    UPackage* Package = CreatePackage(*SavePath);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create package for: %s"), *SavePath);
        return;
    }

    // Create a FileMediaSource object 创建 FileMediaSource 对象
    UFileMediaSource* FileMediaSource = NewObject<UFileMediaSource>(Package, UFileMediaSource::StaticClass(), FName(*TargetFileName), RF_Public | RF_Standalone);
    if (!FileMediaSource)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create UFileMediaSource object for: %s"), *FilePath);
        return;
    }

    //Set file path  设置文件路径
    FileMediaSource->FilePath = FilePath;
    FileMediaSource->MarkPackageDirty();

    // Saves a reference to a member variable 保存引用到成员变量
    SavedMediaSource = FileMediaSource;

    // Register assets to the asset registry 注册资产到资产注册表
    FAssetRegistryModule::AssetCreated(SavedMediaSource);
    UE_LOG(LogTemp, Log, TEXT("Successfully registered media source: %s"), *SavePath);

    // Save package 保存包
    FString PackageFilePath = FPackageName::LongPackageNameToFilename(SavePath, FPackageName::GetAssetPackageExtension());
    if (UPackage::SavePackage(Package, FileMediaSource, RF_Public | RF_Standalone, *PackageFilePath, GError, nullptr, true, true, SAVE_None))
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully imported video file as asset: %s"), *SavePath);
        SavedMediaSource = nullptr;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save package for: %s"), *SavePath);
        SavedMediaSource = nullptr;
    }

}




TSharedRef<SWidget> SVideoAssetsWidget::ConstructImageItem(const FString& ProjectImageUrl)
{
    TSharedPtr<SBox> ImageBox = SNew(SBox)
        .WidthOverride(150.f)
        .HeightOverride(100.f)
        [
            SNew(SBorder)
            .BorderBackgroundColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
            [
                SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("Loading", "Loading..."))
                    .Justification(ETextJustify::Center)
                ]
            ]
        ];
    
    if (!ProjectImageUrl.IsEmpty())
    {
        FImageLoader::LoadImageFromUrl(ProjectImageUrl, FOnProjectImageReady::CreateLambda([this, ImageBox](const TArray<uint8>& ImageData)
        {
            if (ImageData.Num() > 0)
            {
                UTexture2D* LoadedTexture = FImageLoader::CreateTextureFromBytes(ImageData);
                if (LoadedTexture && ImageBox.IsValid())
                {
                    const float TextureWidth = LoadedTexture->GetSizeX();
                    const float TextureHeight = LoadedTexture->GetSizeY();
                    const float AspectRatio = TextureWidth / TextureHeight;
                    
                    ImageBox->SetWidthOverride(150.f);
                    ImageBox->SetHeightOverride(150.f / AspectRatio);
                    
                    ImageBox->SetContent(
                        SNew(SBorder)
                        .BorderBackgroundColor(FLinearColor(0, 0, 0, 1.0f)) 
                        [
                            SNew(SScaleBox)
                            .Stretch(EStretch::ScaleToFit) 
                            .StretchDirection(EStretchDirection::Both)
                            [
                                SNew(SImage)
                                .Image(new FSlateImageBrush(LoadedTexture, FVector2D(TextureWidth, TextureHeight)))
                            ]
                        ]
                    );

                    // Set up lifecycle management for textures to avoid garbage collection issues 设置纹理的生命周期管理，以避免垃圾回收问题
                    LoadedTextures.Add(LoadedTexture);
                    LoadedTexture->AddToRoot(); // Make sure it doesn't get recycled 确保不会被垃圾回收
                }
                else
                {
                    ImageBox->SetContent(
                        SNew(SBorder)
                        .BorderBackgroundColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f))
                        [
                            SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("LoadFailed", "Load Failed"))
                                .Justification(ETextJustify::Center)
                            ]
                        ]
                    );
                }
            }
            else
            {
                ImageBox->SetContent(
                    SNew(SBorder)
                    .BorderBackgroundColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f))
                    [
                        SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("Load Failed")))
                            .Justification(ETextJustify::Center)
                        ]
                    ]
                );
            }
        }));
    }
    else
    {
        ImageBox->SetContent(
            SNew(SBorder)
            .BorderBackgroundColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f))
            [
                SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("NoPreview", "No Preview"))
                    .Justification(ETextJustify::Center)
                ]
            ]
        );
    }

    return ImageBox.ToSharedRef();
}


void SVideoAssetsWidget::ClearVideoContent()
{
    if (LoadedTextures.Num() > 0)
    {
        for (UTexture2D* Texture : LoadedTextures)
        {
            if (Texture)
            {
                Texture->ReleaseResource();
                Texture->RemoveFromRoot();
                Texture->MarkAsGarbage();
            }
        }
        LoadedTextures.Empty();
        
    }
    else
    {
        // UE_LOG(LogTemp, Warning, TEXT("No loaded textures to clear."));
    }
    
    if (VideoAssetsContainer.IsValid())
    {
        VideoAssetsContainer->ClearChildren();
    }
    else
    {
        // UE_LOG(LogTemp, Warning, TEXT("ConceptDesignAssetsContainer is invalid or not initialized."));
    }
}


void SVideoAssetsWidget::SearchVideoFileByName(const FText& InputFileName)
{
    // Convert FText to FString and trim the whitespace 将 FText 转换为 FString 并修剪空格
    FString SearchFileName = InputFileName.ToString().TrimStartAndEnd();

    // Check that the search text is empty 检查搜索文本是否为空
    if (SearchFileName.IsEmpty())
    {
        // UE_LOG(LogTemp, Warning, TEXT("Search text is empty."));
        return;
    }

    // URL encoding of the search file name 对搜索文件名进行 URL 编码
    FString EncodedSearchFileName = FGenericPlatformHttp::UrlEncode(SearchFileName);

    UGetVideoAssetLibraryListInfoApi* VideoFileApi = NewObject<UGetVideoAssetLibraryListInfoApi>();
    if (VideoFileApi)
    {
        FOnGetVideoAssetLibraryListInfoResponse OnGetVideoAssetLibraryListInfoResponse;
        OnGetVideoAssetLibraryListInfoResponse.BindLambda([this, EncodedSearchFileName](const FGetVideoAssetLibraryListInfoData* VideoAssetData)
        {
            if (VideoAssetData && VideoAssetData->data.Num() > 0)
            {
                VideoAssetsContainer->ClearChildren();

                UpdateVideoAssets(VideoAssetData->data);
            }
            else
            {
                VideoAssetsContainer->ClearChildren(); 
                OnVideoNothingToShow.ExecuteIfBound();
            }
        });

        SetUserAndProjectParams();
        FString ParentId = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentVideoParentID();

        // 发送 API 请求
        VideoFileApi->SendGetVideoAssetLibraryListInfoRequest(Ticket, ProjectNo, ParentId, EncodedSearchFileName, OnGetVideoAssetLibraryListInfoResponse);
    }
}

#undef LOCTEXT_NAMESPACE