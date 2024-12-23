// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.



#include "ProjectContent/AudioAssets/SAudioAssetsWidget.h"

#include "FileMediaSource.h"
#include "RSAssetLibraryStyle.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AudioLibrary/GetAudioAssetLibraryFolderListApi.h"
#include "AudioLibrary/GetAudioFileDetailApi.h"
#include "AudioLibrary/GetAudioCommentApi.h"
#include "Widgets/Layout/SScrollBox.h"
#include "ProjectContent/MediaPlayer/SVideoPlayerWidget.h"
#include "Sound/SoundWave.h"
#include "EditorFramework/AssetImportData.h"

#define LOCTEXT_NAMESPACE "SAudioAssetsWidget"

void SAudioAssetsWidget::Construct(const FArguments& InArgs)
{
    OnAudioAssetClicked = InArgs._OnAudioAssetClicked; 
    OnSelectedAudioDownloadClicked = InArgs._OnAudioDownload;
    OnAudioNothingToShow = InArgs._OnAudioNothingToShow;
    OnAudioRefresh = InArgs._OnAudioRefresh;
    
    AssetDownloadWidget = SNew(SAssetDownloadWidget);
    
    if (AssetDownloadWidget.IsValid())
    {
        AssetDownloadWidget->BindOnDownloadCompleted(FOnDownloadCompleted::CreateSP(this, &SAudioAssetsWidget::OnDownloadCompleted));
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
                 SAssignNew(AudioAssetsContainer, SVerticalBox)
               ]  
            ]
	];
    InitializeDetailClickedButtonStyle();
}

void SAudioAssetsWidget::UpdateTagPageAudioAssets(const TArray<FAudioFileData>& AudioFileData)
{

    TSharedPtr<SScrollBox> ScrollBox = SNew(SScrollBox);

    if (AudioFileData.Num() > 0)
    {
        ScrollBox->ClearChildren();
        
        TSharedPtr<SVerticalBox> VerticalContainer = SNew(SVerticalBox);

        TSharedPtr<SHorizontalBox> CurrentRow = SNew(SHorizontalBox);
        int32 ItemCountInRow = 0;
        
        for (const FAudioFileData& AudioFile : AudioFileData)
        {
            const FSlateBrush* IconBrush = FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.AudioImage");

            TSharedPtr<FButtonStyle> FolderButtonStyle = MakeShareable(new FButtonStyle(DetailClickedButtonStyle));
            auto OnClicked = [this, AudioFile, FolderButtonStyle]() -> FReply
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
                
                if (OnAudioAssetClicked.IsBound())
                {
                    TSharedRef<SWidget> DetailsWidget = GenerateDetailsWidget(AudioFile);
                    OnAudioAssetClicked.Execute(AudioFile, DetailsWidget);
                }
                return FReply::Handled();
            };
            
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
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign(HAlign_Center)
                        .Padding(0, 16, 0, 0)
                        [
                            SNew(SBox)
                            .WidthOverride(150.0f).HeightOverride(100.0f)
                            [
                                SNew(SImage)
                                .Image(IconBrush)
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Bottom)
                        .Padding(0, 0, 0, 0)
                        [
                            SNew(SBox)
                            .WidthOverride(150.0f).HeightOverride(30.0f)
                            [
                                SNew(SBorder)
                                .BorderImage(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.FileBorder"))
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock)
                                    .Text(FText::FromString(TruncateText(AudioFile.FileName, 16)))
                                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
                                    .Justification(ETextJustify::Center)
                                ]
                            ]
                        ]
                    ]
                ]
            ];

            ItemCountInRow++;
            
            if (ItemCountInRow == 5)
            {
                VerticalContainer->AddSlot()
                .AutoHeight()
                .Padding(5)
                [
                    CurrentRow.ToSharedRef()
                ];
                
                CurrentRow = SNew(SHorizontalBox);
                ItemCountInRow = 0;
            }
        }
        
        if (ItemCountInRow > 0)
        {
            VerticalContainer->AddSlot()
            .AutoHeight()
            .Padding(5)
            [
                CurrentRow.ToSharedRef()
            ];
        }
        
        ScrollBox->AddSlot()
        [
            VerticalContainer.ToSharedRef()
        ];
        
        AudioAssetsContainer->ClearChildren();
        AudioAssetsContainer->AddSlot()
        .AutoHeight()
        [
            ScrollBox.ToSharedRef()
        ];
        OnAudioRefresh.ExecuteIfBound();
    }
    else
    {
        ClearAudioContent();
        // UE_LOG(LogTemp, Error, TEXT("No audio files found in the response."));
        OnAudioNothingToShow.ExecuteIfBound();
    }

}


void SAudioAssetsWidget::UpdateAudioAssets(const TArray<FAudioAssetLibraryFolderItem>& AudioAsset, const int64& InTagID)
{
    AudioAssetsContainer->ClearChildren();
    TSharedPtr<SScrollBox> ScrollBox = SNew(SScrollBox);

    for (const FAudioAssetLibraryFolderItem& AudioFolder : AudioAsset)
    {
        UGetAudioFileByConditionApi* AudioFileApi = NewObject<UGetAudioFileByConditionApi>();
        if (AudioFileApi)
        {
            FOnGetAudioFileByConditionResponse OnGetAudioFileByConditionResponse;
            OnGetAudioFileByConditionResponse.BindLambda([this, ScrollBox](const FGetAudioFileByConditionResponse& GetAudioFileByConditionResponse)
            {
                if (GetAudioFileByConditionResponse.data.dataList.Num() > 0)
                {
                    ScrollBox->ClearChildren();
                    
                    TSharedPtr<SVerticalBox> VerticalContainer = SNew(SVerticalBox);

                    TSharedPtr<SHorizontalBox> CurrentRow = SNew(SHorizontalBox);
                    int32 ItemCountInRow = 0;
                    
                    for (const FAudioFileData& AudioFile : GetAudioFileByConditionResponse.data.dataList)
                    {
                        const FSlateBrush* IconBrush = FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.AudioImage");

                        TSharedPtr<FButtonStyle> FolderButtonStyle = MakeShareable(new FButtonStyle(DetailClickedButtonStyle));
                        
                        auto OnClicked = [this, AudioFile, FolderButtonStyle]() -> FReply
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
                            
                            if (OnAudioAssetClicked.IsBound())
                            {
                                TSharedRef<SWidget> DetailsWidget = GenerateDetailsWidget(AudioFile);
                                OnAudioAssetClicked.Execute(AudioFile, DetailsWidget);
                            }
                            return FReply::Handled();
                        };
                        
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
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .HAlign(HAlign_Center)
                                    .Padding(0, 16, 0, 0)
                                    [
                                        SNew(SBox)
                                        .WidthOverride(150.0f).HeightOverride(100.0f)
                                        [
                                            SNew(SImage)
                                            .Image(IconBrush)
                                        ]
                                    ]
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .HAlign(HAlign_Center)
                                    .VAlign(VAlign_Bottom)
                                    .Padding(0, 0, 0, 0)
                                    [
                                        SNew(SBox)
                                        .WidthOverride(150.0f).HeightOverride(30.0f)
                                        [
                                            SNew(SBorder)
                                            .BorderImage(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.FileBorder"))
                                            .HAlign(HAlign_Center) 
                                            .VAlign(VAlign_Center) 
                                            [
                                                SNew(STextBlock)
                                                .Text(FText::FromString(TruncateText(AudioFile.FileName, 14)))
                                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
                                                .Justification(ETextJustify::Center)
                                            ]
                                        ]
                                    ]
                                ]
                            ]
                        ];

                        ItemCountInRow++;
                        
                        if (ItemCountInRow == 5)
                        {
                            VerticalContainer->AddSlot()
                            .AutoHeight()
                            .Padding(5)
                            [
                                CurrentRow.ToSharedRef()
                            ];
                            
                            CurrentRow = SNew(SHorizontalBox);
                            ItemCountInRow = 0;
                        }
                    }
                    
                    if (ItemCountInRow > 0)
                    {
                        VerticalContainer->AddSlot()
                        .AutoHeight()
                        .Padding(5)
                        [
                            CurrentRow.ToSharedRef()
                        ];
                    }
                    
                    ScrollBox->AddSlot()
                    [
                        VerticalContainer.ToSharedRef()
                    ];
                    
                    AudioAssetsContainer->ClearChildren();
                    AudioAssetsContainer->AddSlot()
                    .AutoHeight()
                    [
                        ScrollBox.ToSharedRef()
                    ];
                }
                else
                {
                   OnAudioNothingToShow.ExecuteIfBound();
                }
            });
            
            SetUserAndProjectParams();
            FileId = AudioFolder.Id;
            FString StringFileId = FString::FromInt(FileId);
            FString AudioChannel = ""; 
            FString AudioHarvestBits= "";
            FString AudioHarvestRate= "";
            int32 BpmBegin = *""; 
            int32 BpmEnd = *"";
            int32 CurrentPage = 1;
            FString FileFormat = ""; 
            int32 MenuType = 1; 
            int32 PageSize = 1000; 
            FString Search = "";
            FString Sort = "DESC";
            FString SortType = "1";
            int64 TagId = *""; 
            
        AudioFileApi->SendGetAudioFileByConditionRequest(
            Ticket, Uuid, ProjectNo,
            StringFileId, AudioChannel, AudioHarvestBits,
            AudioHarvestRate, BpmBegin, BpmEnd,
            CurrentPage, FileFormat, MenuType,
            PageSize, Search, Sort,
            SortType, InTagID, OnGetAudioFileByConditionResponse);
        }
    }
}

void SAudioAssetsWidget::InitializeDetailClickedButtonStyle()
{
    DetailClickedButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ContentBorder"));
    DetailClickedButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButtonClicked"));
    DetailClickedButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButton"));
}


FString SAudioAssetsWidget::TruncateText(const FString& OriginalText, int32 MaxLength)
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

void SAudioAssetsWidget::SetUserAndProjectParams()
{
    USMSubsystem = GEditor->GetEditorSubsystem<UUSMSubsystem>();
    Ticket = USMSubsystem->GetCurrentUserAndProjectInfo().Ticket;
    Uuid = USMSubsystem->GetCurrentUserAndProjectInfo().Uuid;
    ProjectNo = USMSubsystem->GetSelectedProject().projectNo;
}


TSharedRef<SWidget> SAudioAssetsWidget::GenerateDetailsWidget(const FAudioFileData& AudioFile)
{
    // Initializes the comment container 初始化评论容器
    CommentContainer = SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("LoadingComments", "Loading comments..."))
        ];
    
    UGetAudioCommentApi* GetAudioCommentApi = NewObject<UGetAudioCommentApi>();
    
    if (GetAudioCommentApi)
    {
         FOnGetAudioCommentResponse OnGetAudioCommentResponseDelegate;
        OnGetAudioCommentResponseDelegate.BindLambda([this](UGetAudioCommentResponseData* ResponseData)
        {
            CommentContainer->ClearChildren();

            if (ResponseData->DataList.Num() > 0)
            {
                int32 Index = 1;
                for (const FAudioCommentItem& Comment : ResponseData->DataList)
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
                                .Text(FText::FromString(Comment.MemberName))
                                .Justification(ETextJustify::Left)
                            ]
                            + SHorizontalBox::Slot()
                            [
                                SNew(STextBlock)
                               .Text(FText::FromString(Comment.CreateTime))
                               .Justification(ETextJustify::Right)
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .Padding(FMargin(0.0, 15.0, 0.0, 0.0)) 
                        [
                        SNew(STextBlock)
                        .Text(FText::FromString(Comment.Content))
                        .ColorAndOpacity(FSlateColor(FLinearColor::White))
                        .AutoWrapText(true) 
                        .WrapTextAt(600.0f) 
                        ]
                    ]
                    ];

                    Index++; // 增加计数
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

        SetUserAndProjectParams();
        
        FString FileNo = AudioFile.FileNo;
        int32 CurrentPage = 1;
        int32 PageSize = 100;

        GetAudioCommentApi->SendGetAudioCommentRequest(Ticket, Uuid, FileNo, CurrentPage, PageSize, OnGetAudioCommentResponseDelegate);
    }
    
    TSharedPtr<SVerticalBox> Placeholder = SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
        ];

    UGetAudioFileDetailApi* AudioFileDetailApi = NewObject<UGetAudioFileDetailApi>();
    if (AudioFileDetailApi)
    {
     
        FOnGetAudioFileDetailResponse OnGetAudioFileDetailResponse;
        FString FileNo = AudioFile.FileNo;
        
        AudioFileDetailApi->SendGetAudioFileDetailRequest(
            Ticket, 
            FileNo, 
            FOnGetAudioFileDetailResponse::CreateLambda([this, Placeholder](const FAudioFileDetailData& AudioFileData)
            {
                
                     TSharedPtr<SVerticalBox> TagContainer = SNew(SVerticalBox);
                
                     if (AudioFileData.TagInfoList.Num() == 0)
                     {
                         TagContainer->AddSlot()
                         [
                             SNew(SHorizontalBox) 
                             + SHorizontalBox::Slot()
                             .AutoWidth()
                             .Padding(5, 0, 5, 15)
                             [
                                 SNew(STextBlock)
                                 .Text(LOCTEXT("NoTags", "No Tags"))
                             ]
                         ];
                     }
                     else
                     {
                         
                         TSharedPtr<SHorizontalBox> CurrentRow;
                         int32 TagCount = 0;
                         for (const FTagInfo& TagInfo : AudioFileData.TagInfoList)
                         {
                             if (TagCount % 3 == 0)
                             {
                                 if (CurrentRow.IsValid())
                                 {
                                     TagContainer->AddSlot()
                                     [
                                         CurrentRow.ToSharedRef()  
                                     ];
                                 }
                                 
                                 CurrentRow = SNew(SHorizontalBox);
                             }
                             
                             CurrentRow->AddSlot()
                             .AutoWidth()
                             .Padding(0, 0, 5, 15)
                             [
                                 SNew(SButton)
                                     .ButtonStyle(&TagButtonStyle)
                                     .ContentPadding(FMargin(5.f, 5.f))
                                     [
                                         SNew(STextBlock)
                                         .Text(FText::FromString(TruncateText(TagInfo.TagName, 12)))
                                         .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))  
                                         .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                     ]
                             ];

                             TagCount++;
                         }
                         
                         if (CurrentRow.IsValid())
                         {
                             TagContainer->AddSlot()
                             [
                                 CurrentRow.ToSharedRef()
                             ];
                         }
                     }

                AudioFileName = AudioFileData.FileName;

                bool bIsFileDownloaded = IsFileDownloaded(AudioFileName);
                
                Placeholder->ClearChildren();

                Placeholder->AddSlot()
                .FillHeight(1.0)
                .Padding(5, 0, 0, 0)
                [
                    SNew(SBorder)
                    .Padding(0)
                    .BorderImage(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DetailsBorder"))
                    [
                        SNew(SVerticalBox)
                         + SVerticalBox::Slot()
                        .FillHeight(1.0)
                        .Padding(0)
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
                                           SNew(SImage)
                                           .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.AudioImage")) 
                                        ]
                                        // Button section in the lower right corner        
                                        // 右下角的按钮部分
                                        + SOverlay::Slot()
                                        .HAlign(HAlign_Right)
                                        .VAlign(VAlign_Bottom)
                                        .Padding(10) 
                                        [
                                           SNew(SButton)
                                           .Cursor(EMouseCursor::Hand)
                                            .ButtonStyle(&PlayCircleButtonStyle) 
                                            .ContentPadding(0) 
                                           .OnClicked_Lambda([this, AudioFileData]() -> FReply {
                                               return PlayAudioWithSystemPlayer(AudioFileData.RelativePath);
                                           })
                                        ]
                                    ]
                                ]
                                // Part 2: Blue box - File name
                                // 第二部分：蓝框 - 文件名
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
                                               .Text(FText::FromString(TruncateText(AudioFileName, 26)))
                                               .Font(FCoreStyle::GetDefaultFontStyle("Bold", 15))
                                               .Justification(ETextJustify::Left)
                                               .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                           ]
                                       ]
                                   ]
                                ]
                                // Part 3: Basic information
                                // 第三部分：基本信息
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
                                                .Text(LOCTEXT("AudioInformation", "Audio Info"))
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
                                                    .Text(LOCTEXT("Encoder", "Encoder"))
                                                    .Justification(ETextJustify::Left)
                                                ]
                                                
                                                + SHorizontalBox::Slot()
                                                .FillWidth(1.0f)  
                                                .HAlign(HAlign_Right)
                                                [
                                                    SNew(STextBlock)
                                                    .Text(FText::FromString(AudioFileData.AudioEncoder))
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
                                                    .Text(LOCTEXT("Channels", "Channels"))
                                                    .Justification(ETextJustify::Left)
                                                ]
                                                + SHorizontalBox::Slot()
                                                .FillWidth(1.0f) 
                                                .HAlign(HAlign_Right)
                                                [
                                                    SNew(STextBlock)
                                                    .Text(FText::FromString(AudioFileData.AudioChannel))
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
                                                    .Text(LOCTEXT("SampleRate", "Sample Rate"))
                                                    .Justification(ETextJustify::Left)
                                                ]
                                                + SHorizontalBox::Slot()
                                                .FillWidth(1.0f)  
                                                .HAlign(HAlign_Right)
                                                [
                                                    SNew(STextBlock)
                                                    .Text(FText::FromString(AudioFileData.AudioHarvestRate))
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
                                                    .Text(LOCTEXT("BitRate", "Bit Rate"))
                                                    .Justification(ETextJustify::Left)
                                                ]
                                                + SHorizontalBox::Slot()
                                                .FillWidth(1.0f)  
                                                .HAlign(HAlign_Right)
                                                [
                                                    SNew(STextBlock)
                                                    .Text(FText::FromString(AudioFileData.AudioBiteRate))
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
                                                SNew(STextBlock)
                                                .Text(LOCTEXT("FileInformation", "File Info"))
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
                                                    .Text(LOCTEXT("FileSize", "File Size"))
                                                    .Justification(ETextJustify::Left)
                                                ]
                                                
                                                + SHorizontalBox::Slot()
                                                .FillWidth(1.0f) 
                                                .HAlign(HAlign_Right)
                                                [
                                                    SNew(STextBlock)
                                                    .Text(FText::FromString(AudioFileData.FileSize))
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
                                                    .Text(FText::FromString(AudioFileData.CreateName))
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
                                                    .Text(LOCTEXT("CreationTime", "Creation Time"))
                                                    .Justification(ETextJustify::Left)
                                                ]
                                                + SHorizontalBox::Slot()
                                                .FillWidth(1.0f) 
                                                .HAlign(HAlign_Right)
                                                [
                                                    SNew(STextBlock)
                                                    .Text(FText::FromString(AudioFileData.CreateTime))
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
                                                SNew(STextBlock)
                                                .Text(LOCTEXT("Tags", "Tags"))
                                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                                                .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                            ]
                                            
                                            + SVerticalBox::Slot()
                                            .AutoHeight()
                                            .Padding(20, 0, 25, 15)
                                            [
                                                TagContainer.ToSharedRef()
                                            ]

                                            + SVerticalBox::Slot()
                                            .AutoHeight()
                                            .Padding(FMargin(20.0f, 10.0f, 20.0f, 5.0f)) 
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
                               
                                // Part 4: Bottom button (Download and import)
                                // 第四部分：底部按钮（下载和导入）
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
                                                    .IsEnabled_Lambda([this]() { return bIsButtonEnabled && !IsFileDownloaded(AudioFileName); }) 
                                                    .OnClicked_Lambda([this, AudioFileData]()-> FReply
                                                    {
                                                        // Gets the current timestamp 获取当前时间戳
                                                        float CurrentTime = FPlatformTime::Seconds();

                                                        // Check whether the cooling time conditions are met 判断是否满足冷却时间条件
                                                        if (CurrentTime - LastClickTime < ClickCooldownTime)
                                                        {
                                                            return FReply::Handled();  // If you click too often, ignore the current click 如果点击过于频繁，忽略当前点击
                                                        }
                                                        LastClickTime = CurrentTime;
                                                        bIsDownloadButtonEnabled = false;

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

                                                        SelectedAudioFileInfo.FileName = AudioFileData.FileName;
                                                        SelectedAudioFileInfo.FileUrl = AudioFileData.RelativePath;
                                                        SelectedAudioFileInfo.FileMd5 = AudioFileData.FileMd5;

                                                        if (AudioFileData.FileSize == "0")
                                                        {
                                                            return FReply::Handled();
                                                        }
                                                        OnDownloadAudioButtonClicked(SelectedAudioFileInfo);

                                                        bool bIsFileDownloaded = IsFileDownloaded(AudioFileName);

                                                        if (bIsFileDownloaded)
                                                        {
                                                            bIsDownloadButtonEnabled = false;
                                                        }
                                                        else
                                                        {
                                                            bIsDownloadButtonEnabled = true;
                                                        }
                                                        return FReply::Handled();
                                                    })
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
                                                        ]
                                                    ]
                                                ]
                                                + SHorizontalBox::Slot()
                                                .AutoWidth()
                                                .Padding(5, 2, 5, 2)
                                                [
                                                    SNew(SButton)
                                                    .Cursor(EMouseCursor::Hand)
                                                    .ButtonStyle(&ImportButtonStyle) 
                                                    .OnClicked(this, &SAudioAssetsWidget::OnImportAudioFileButtonClicked)
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
                                                        ]
                                                    ]
                                                ]
                                            ]
                                        ]
                                    ]

                            ]
                        ]
                    ]
                ];
                
                ImportButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadBarBorder"));
                ImportButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ImportHover"));
                ImportButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Importclicked"));

                PlayCircleButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayCircle"));
                PlayCircleButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayCircleHover"));
                PlayCircleButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayCircleHover"));

                TagButtonStyle.SetNormal(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));  
                TagButtonStyle.SetHovered(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));
                TagButtonStyle.SetPressed(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));
            })
        );
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to create UGetAudioFileByConditionApi instance."));
    }
    
    return Placeholder.ToSharedRef();
}

bool SAudioAssetsWidget::IsFileDownloaded(const FString& ForbidFileName) const
{

    FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), ForbidFileName);
    
    return FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath);
}

void SAudioAssetsWidget::OnDownloadCompleted(const FString& AssetFileName)
{
    if (AssetFileName == AudioFileName)
    {
        // Find the appropriate download button and disable it 找到相应的下载按钮并禁用它
        if (DownloadButton.IsValid())
        {
            DownloadButton->Invalidate(EInvalidateWidget::LayoutAndVolatility);
        }
    }
}


FReply SAudioAssetsWidget::PlayAudioWithSystemPlayer(const FString& InPath)
{
    FMediaPlayerManager::PlayAudioWithSystemPlayer(InPath); 
    
    return FReply::Handled();
}

FReply SAudioAssetsWidget::OnDownloadAudioButtonClicked(FSelectedAudioFileInfo AudioInfo)
{

    if (OnSelectedAudioDownloadClicked.IsBound())
    {
        OnSelectedAudioDownloadClicked.Execute(AudioInfo.FileName, AudioInfo.FileUrl, AudioInfo.FileMd5);
    }
    
    return FReply::Handled();
}

FReply SAudioAssetsWidget::OnImportAudioFileButtonClicked()
{
    FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), AudioFileName);
    
    ImportAudioFile(FilePath);

    return FReply::Handled();
}

void SAudioAssetsWidget::ImportAudioFile(const FString& FilePath)
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
              ExistingNotificationWindow.Reset(); // Reset pointer after window destruction 窗口销毁后重置指针
        }));
        
        FSlateApplication::Get().AddWindow(ExistingNotificationWindow.ToSharedRef());
        return;
    }
    
    if (!FPaths::FileExists(FilePath))
    {
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
    
    FString FileExtension = FPaths::GetExtension(FilePath).ToLower();
    
    if (FileExtension == TEXT("wav") || FileExtension == TEXT("aiff") || FileExtension == TEXT("flac") || FileExtension == TEXT("ogg"))
    {
        ImportSupportedAudioFormats(FilePath, FileExtension);
    }
    else
    {
        FString TargetFileName = FPaths::GetBaseFilename(FilePath).Replace(TEXT(" "), TEXT("_")).Replace(TEXT("."), TEXT("_"));
        FString SavePath = TEXT("/Game/RSpaceImportedAssets/Audios/") + TargetFileName;
        
        if (FPackageName::DoesPackageExist(SavePath))
        {
            UE_LOG(LogTemp, Warning, TEXT("Asset already exists at: %s"), *SavePath);
            return;
        }

        UPackage* Package = CreatePackage(*SavePath);
        if (!Package)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create package for: %s"), *SavePath);
            return;
        }

        UFileMediaSource* FileMediaSource = NewObject<UFileMediaSource>(Package, UFileMediaSource::StaticClass(), FName(*TargetFileName), RF_Public | RF_Standalone);
        if (!FileMediaSource)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create UFileMediaSource object for: %s"), *FilePath);
            return;
        }

        FileMediaSource->SetFilePath(FilePath);
        FileMediaSource->MarkPackageDirty();

        // Register assets to the asset registry 注册资产到资产注册表
        FAssetRegistryModule::AssetCreated(FileMediaSource);

        UE_LOG(LogTemp, Log, TEXT("Successfully registered media source: %s"), *SavePath);
        
        FString PackageFilePath = FPackageName::LongPackageNameToFilename(SavePath, FPackageName::GetAssetPackageExtension());
        if (UPackage::SavePackage(Package, FileMediaSource, RF_Public | RF_Standalone, *PackageFilePath, GError, nullptr, true, true, SAVE_None))
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully imported audio file as media source: %s"), *SavePath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to save package for: %s"), *SavePath);
        }
    }
}

void SAudioAssetsWidget::ImportSupportedAudioFormats(const FString& FilePath, const FString& FileExtension)
{
    //  Define the save path and handle Spaces and dots in file names 定义保存路径并处理文件名中的空格和点
    FString TargetFileName = FPaths::GetBaseFilename(FilePath).Replace(TEXT(" "), TEXT("_")).Replace(TEXT("."), TEXT("_"));
    FString SavePath = TEXT("/Game/RSpaceImportedAssets/Audios/") + TargetFileName;
    
    if (FPackageName::DoesPackageExist(SavePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Asset already exists at: %s"), *SavePath);
        return;
    }

    // Read audio file data 读取音频文件数据
    TArray<uint8> AudioData;
    if (!FFileHelper::LoadFileToArray(AudioData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load audio file: %s"), *FilePath);
        return;
    }

    // Using FWaveModInfo to parse WAV files, you can continue to extend the parsing logic for other formats 使用 FWaveModInfo 解析 WAV 文件，针对其他格式可以继续扩展解析逻辑
    FWaveModInfo WaveInfo;
    if (!WaveInfo.ReadWaveInfo(AudioData.GetData(), AudioData.Num()))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse audio file: %s"), *FilePath);
        return;
    }

    // Create an audio asset package 创建音频资产包
    FString PackageName = FPackageName::ObjectPathToPackageName(SavePath);
    UPackage* Package = CreatePackage(*PackageName);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *SavePath);
        return;
    }

    // Create a USoundWave object 创建 USoundWave 对象
    USoundWave* SoundWave = NewObject<USoundWave>(Package, *TargetFileName, RF_Public | RF_Standalone);
    if (!SoundWave)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create USoundWave asset."));
        return;
    }

    // Fill in audio data and properties 填充音频数据和属性
    const int32 BitsPerSample = *WaveInfo.pBitsPerSample;
    const int32 Channels = *WaveInfo.pChannels;
    const int32 SampleRate = *WaveInfo.pSamplesPerSec;
    const int32 DataSize = WaveInfo.SampleDataSize;

    if (BitsPerSample / 8 == 0 || Channels == 0 || SampleRate == 0 || DataSize == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid audio file data."));
        return;
    }

    const int32 SizeOfSample = BitsPerSample / 8;
    const int32 TotalSamples = DataSize / SizeOfSample;
    const int32 Frames = TotalSamples / Channels;

    SoundWave->Duration = static_cast<float>(Frames) / SampleRate;
    SoundWave->NumChannels = Channels;
    SoundWave->SetSampleRate(SampleRate);
    SoundWave->SoundGroup = ESoundGroup::SOUNDGROUP_Default;
    SoundWave->DecompressionType = DTYPE_Setup;
    SoundWave->RawPCMDataSize = DataSize;
    SoundWave->RawPCMData = static_cast<uint8*>(FMemory::Malloc(DataSize));
    FMemory::Memcpy(SoundWave->RawPCMData, WaveInfo.SampleDataStart, DataSize);

    // Mark the object as modified 标记对象已修改
    SoundWave->MarkPackageDirty();

    // Register assets to the asset registry 注册资产到资产注册表
    FAssetRegistryModule::AssetCreated(SoundWave);

    // Save asset package 保存资产包
    if (!UPackage::SavePackage(Package, SoundWave, EObjectFlags::RF_Public | RF_Standalone, *FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension())))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save package: %s"), *SavePath);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully imported audio file: %s"), *FilePath);
    }
}

void SAudioAssetsWidget::ImportWithFileMediaSource(const FString& FilePath)
{
    // For unsupported formats, use file media sources (MP3, M4A, WAV, and other formats) 对于不支持的格式，使用文件媒体源（MP3、M4A、WAV 等其他格式）
    UFileMediaSource* MediaSource = NewObject<UFileMediaSource>();
    if (MediaSource)
    {
        MediaSource->SetFilePath(FilePath);

        UE_LOG(LogTemp, Log, TEXT("Media source created for non-supported format: %s"), *FilePath);

        // Create a media player and start playing 创建媒体播放器并开始播放
        UMediaPlayer* MediaPlayer = NewObject<UMediaPlayer>();
        if (MediaPlayer)
        {
            MediaPlayer->OpenSource(MediaSource);
            MediaPlayer->Play();

            UE_LOG(LogTemp, Log, TEXT("Media player started for file: %s"), *FilePath);
        }
    }
}

#undef LOCTEXT_NAMESPACE