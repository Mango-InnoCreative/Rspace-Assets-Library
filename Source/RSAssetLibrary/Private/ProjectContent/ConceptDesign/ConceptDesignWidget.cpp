// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.


#include "ProjectContent/ConceptDesign/ConceptDesignWidget.h"
#include "RSAssetLibraryStyle.h"
#include "ConceptDesignLibrary/GetConceptDesignLibraryFolderDetailApi.h"
#include "ConceptDesignLibrary/GetConceptDesignPictureCommentApi.h"
#include "ConceptDesignLibrary/GetConceptDesignPictureDetailApi.h"
#include "Misc/FileHelper.h"
#include "ProjectContent/Imageload/FImageLoader.h"
#include "Widgets/Layout/SScrollBox.h"
#include "ProjectContent/ConceptDesign/ConceptDesignDisplay.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "ConceptDesignLibrary/GetConceptDesignLibMenuData.h"
#include "ProjectContent/MediaPlayer/SVideoPlayerWidget.h"
#include "UObject/SavePackage.h"
#include "Widgets/Layout/SScaleBox.h"

#define LOCTEXT_NAMESPACE "SConceptDesignWidget"

void SConceptDesignWidget::Construct(const FArguments& InArgs)
{
    OnConceptDesignAssetClicked = InArgs._OnConceptDesignAssetClicked; 
    OnSelectedConceptDownloadClicked = InArgs._OnConceptDownload;
    OnConceptNothingToShow = InArgs._OnConceptNothingToShow;
    OnConceptRefresh = InArgs._OnConceptRefresh;
    
    AssetDownloadWidget = SNew(SAssetDownloadWidget);
    
    if (AssetDownloadWidget.IsValid())
    {
        AssetDownloadWidget->BindOnDownloadCompleted(FOnDownloadCompleted::CreateSP(this, &SConceptDesignWidget::OnDownloadCompleted));
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
                 SAssignNew(ConceptDesignAssetsContainer, SVerticalBox)
               ]  
            ]
	];
    InitializeDetailClickedButtonStyle();
}

void SConceptDesignWidget::UpdateConceptDesignTagPageAssets(const TArray<FFileItemDetails> ConceptItems)
{
     if ( ConceptItems.Num() > 0 )
    {
         FConceptDesignFileItem ConceptDesignFileItem;
         
        TSharedPtr<SVerticalBox> VerticalContainer = SNew(SVerticalBox);

        TSharedPtr<SHorizontalBox> CurrentRow = SNew(SHorizontalBox);
        int32 ItemCountInRow = 0;
         
        for (const FFileItemDetails& ConceptFileItem : ConceptItems)
        {
            const FSlateBrush* IconBrush = FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.AudioFile.Icon");
            
            TSharedPtr<FButtonStyle> FolderButtonStyle = MakeShareable(new FButtonStyle(DetailClickedButtonStyle));
            
            auto OnClicked = [this, ConceptFileItem, ConceptDesignFileItem, FolderButtonStyle]() -> FReply
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
                
                if (OnConceptDesignAssetClicked.IsBound())
                {
                    UGetConceptDesignPictureDetailApi* SelectPicFileDetailsInfo = NewObject<UGetConceptDesignPictureDetailApi>();
                    if (SelectPicFileDetailsInfo)
                    {
                        FOnConceptDesignPictureDetailResponse OnConceptDesignPictureDetailResponse;
                        OnConceptDesignPictureDetailResponse.BindLambda([this, ConceptFileItem, ConceptDesignFileItem](FGetConceptDesignPictureDetailData* SelectPicFileDetailsInfoData)
                        {
                            if (SelectPicFileDetailsInfoData)
                            {
                                const FConceptDesignPictureDetail& FileDetails = SelectPicFileDetailsInfoData->data;
                                Filetag.Empty();
                                for (const FTags& Tag : FileDetails.tags)
                                {
                                    Filetag.Add(Tag.tagName); 
                                }

                                FConceptDesignFileItem InConceptDesignFileItem;
                                InConceptDesignFileItem.Id = ConceptFileItem.id;
                                InConceptDesignFileItem.UserNo = ConceptFileItem.userNo;
                                InConceptDesignFileItem.FolderId = "null";
                                InConceptDesignFileItem.Name = ConceptFileItem.name;
                                InConceptDesignFileItem.TagCount = ConceptFileItem.tagCount;
                                InConceptDesignFileItem.RelativePatch = ConceptFileItem.relativePatch;
                                InConceptDesignFileItem.ThumRelativePatch = ConceptFileItem.thumRelativePatch;
                                InConceptDesignFileItem.DeleteStatus = ConceptFileItem.deleteStatus;
                                InConceptDesignFileItem.FileLength = ConceptFileItem.fileLength;
                                InConceptDesignFileItem.FileSize = ConceptFileItem.fileSize;
                                InConceptDesignFileItem.FileSuffix = ConceptFileItem.fileSuffix;
                                InConceptDesignFileItem.FileMd5 = ConceptFileItem.fileMd5;
                                InConceptDesignFileItem.FileMd5GetStatus = ConceptFileItem.fileMd5GetStatus;
                                InConceptDesignFileItem.CreateTime = ConceptFileItem.createTime;
                                InConceptDesignFileItem.UpdateTime = ConceptFileItem.updateTime;
                                InConceptDesignFileItem.ProjectNo = ConceptFileItem.projectNo;

                                
                                TSharedRef<SWidget> DetailsWidget = GenerateDetailsWidget(InConceptDesignFileItem);
                                OnConceptDesignAssetClicked.Execute(InConceptDesignFileItem, DetailsWidget);
                                // UE_LOG(LogTemp, Log, TEXT("~~~SelectPicileDetailsInfoAPI respond Successfully~~File Name: %s, File Path: %s"), *FileDetails.name, *FileDetails.relativePatch);
                            }
                        });
                        SetUserAndProjectParams();
                        int32 PicId = ConceptFileItem.id;
                        SelectPicFileDetailsInfo->SendGetPictureDetailRequest(Ticket, PicId, OnConceptDesignPictureDetailResponse);
                    }
                    
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
                            .WidthOverride(150.0f)
                            .HeightOverride(100.0f)
                            [
                                 ConstructImageItem(ConceptFileItem.thumRelativePatch)
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Bottom)
                        .Padding(0, 0, 0, 0)
                        [
                            SNew(SBox)
                            .WidthOverride(150.0f)
                            .HeightOverride(30.0f)
                            [
                                SNew(SBorder)
                                .BorderImage(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.FileBorder"))
                                .HAlign(HAlign_Center) 
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock)
                                    .Text(FText::FromString(TruncateText(ConceptFileItem.name, 14)))
                                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
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
         
        ConceptDesignAssetsContainer->ClearChildren();
        ConceptDesignAssetsContainer->AddSlot()
        .AutoHeight()
        [
             VerticalContainer.ToSharedRef()
        ];

         OnConceptRefresh.ExecuteIfBound();
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("No audio files found in the response."));
        OnConceptNothingToShow.ExecuteIfBound();
    }
}

void SConceptDesignWidget::UpdateConceptDesignAssets(const TArray<FConceptDesignFolderItem>& ConceptDesignAsset, const FString& InTagID)
{

    for (const FConceptDesignFolderItem& ConceptDesignFolderItem : ConceptDesignAsset)
    {
        UGetConceptDesignLibraryFolderDetailApi* GetConceptDesignLibraryFolderDetailApi = NewObject<UGetConceptDesignLibraryFolderDetailApi>();
        if (GetConceptDesignLibraryFolderDetailApi)
        {
            FOnGetConceptDesignFolderDetailResponse OnGetConceptDesignFolderDetailResponse;
            OnGetConceptDesignFolderDetailResponse.BindLambda([this](UGetConceptDesignLibraryFolderDetailData* GetConceptDesignFolderDetailResponse)
            {
                if (GetConceptDesignFolderDetailResponse)
                {
                    TSharedPtr<SVerticalBox> VerticalContainer = SNew(SVerticalBox);

                    TSharedPtr<SHorizontalBox> CurrentRow = SNew(SHorizontalBox);
                    int32 ItemCountInRow = 0;

                    if ( GetConceptDesignFolderDetailResponse->Items.Num()>0 )
                        {
                        for (const FConceptDesignFileItem& ConceptDesignFileItem : GetConceptDesignFolderDetailResponse->Items)
                        {
                            // UE_LOG(LogTemp, Log, TEXT("GetConceptDesignLibraryFolderDetailApi Reseponse SUCCESSFULLY!!!!!!FoderNAME: %d"), ConceptDesignFileItem.Id);
                            const FSlateBrush* IconBrush = FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.AudioFile.Icon");
                            
                            TSharedPtr<FButtonStyle> FolderButtonStyle = MakeShareable(new FButtonStyle(DetailClickedButtonStyle));
                            
                            auto OnClicked = [this, ConceptDesignFileItem, FolderButtonStyle]() -> FReply
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
                                
                                if (OnConceptDesignAssetClicked.IsBound())
                                {
                                    UGetConceptDesignPictureDetailApi* SelectPicFileDetailsInfo = NewObject<UGetConceptDesignPictureDetailApi>();
                                    if (SelectPicFileDetailsInfo)
                                    {
                                        FOnConceptDesignPictureDetailResponse OnConceptDesignPictureDetailResponse;
                                        OnConceptDesignPictureDetailResponse.BindLambda([this, ConceptDesignFileItem](FGetConceptDesignPictureDetailData* SelectPicFileDetailsInfoData)
                                        {
                                            if (SelectPicFileDetailsInfoData)
                                            {
                                                const FConceptDesignPictureDetail& FileDetails = SelectPicFileDetailsInfoData->data;
                                                Filetag.Empty();
                                                for (const FTags& Tag : FileDetails.tags)
                                                {
                                                    
                                                        Filetag.Add(Tag.tagName);  
                                                    
                                                }
                                                TSharedRef<SWidget> DetailsWidget = GenerateDetailsWidget(ConceptDesignFileItem);
                                                OnConceptDesignAssetClicked.Execute(ConceptDesignFileItem, DetailsWidget);
                                                // UE_LOG(LogTemp, Log, TEXT("~~~SelectPicileDetailsInfoAPI respond Successfully~~File Name: %s, File Path: %s"), *FileDetails.name, *FileDetails.relativePatch);
                                            }
                                        });
                                        SetUserAndProjectParams();
                                        int32 PicId = ConceptDesignFileItem.Id;
                                        SelectPicFileDetailsInfo->SendGetPictureDetailRequest(Ticket, PicId, OnConceptDesignPictureDetailResponse);
                                    }
                                    
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
                                                 ConstructImageItem(ConceptDesignFileItem.ThumRelativePatch)
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
                                                    .Text(FText::FromString(TruncateText(ConceptDesignFileItem.Name, 14)))
                                                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
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
                        
                        ConceptDesignAssetsContainer->ClearChildren();
                        ConceptDesignAssetsContainer->AddSlot()
                        .AutoHeight()
                        [
                             VerticalContainer.ToSharedRef()
                        ];
                    }
                    else
                    {
                        OnConceptNothingToShow.ExecuteIfBound();
                    }

                }
                else
                {
                    // UE_LOG(LogTemp, Error, TEXT("No audio files found in the response."));
                }
            });
            
            SetUserAndProjectParams();
            FString PaintingName = "";
            FString TagId = InTagID;
            CurrentPage = 1;
            PageSize = 1000;
            FolderId = ConceptDesignFolderItem.id;
            //// UE_LOG(LogTemp, Error, TEXT("foldid: %d"), FolderId);
            
            GetConceptDesignLibraryFolderDetailApi->SendGetFolderDetailRequest(Ticket, Uuid, PaintingName, FolderId, CurrentPage, PageSize, TagId, OnGetConceptDesignFolderDetailResponse);
        }
    }
}

void SConceptDesignWidget::InitializeDetailClickedButtonStyle()
{
    DetailClickedButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ContentBorder"));
    DetailClickedButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButtonClicked"));
    DetailClickedButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButton"));
}


FString SConceptDesignWidget::TruncateText(const FString& OriginalText, int32 MaxLength)
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

void SConceptDesignWidget::SetUserAndProjectParams()
{
    USMSubsystem = GEditor->GetEditorSubsystem<UUSMSubsystem>();
    Ticket = USMSubsystem->GetCurrentUserAndProjectInfo().Ticket;
    Uuid = USMSubsystem->GetCurrentUserAndProjectInfo().Uuid;
    ProjectNo = USMSubsystem->GetSelectedProject().projectNo;
}


TSharedRef<SWidget> SConceptDesignWidget::GenerateDetailsWidget(const FConceptDesignFileItem& ConceptDesignFileItem)
{
    ImportButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadBarBorder"));
    ImportButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ImportHover"));
    ImportButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Importclicked"));

    PlayImageCircleButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayImage"));
    PlayImageCircleButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayImageHover"));
    PlayImageCircleButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayImageHover"));

    TagButtonStyle.SetNormal(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));  
    TagButtonStyle.SetHovered(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));
    TagButtonStyle.SetPressed(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));
    
    TSharedPtr<FSlateBrush> ItemBrush = MakeShareable(new FSlateBrush());
    
    CommentContainer = SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("LoadingComments", "Loading comments..."))
        ];

    UGetConceptDesignPictureCommentApi* SelectPicFileComment = NewObject<UGetConceptDesignPictureCommentApi>();
    
    if (SelectPicFileComment)
    {
        FOnGetConceptDesignPictureCommentResponse OnGetConceptDesignPictureCommentResponse;
        OnGetConceptDesignPictureCommentResponse.BindLambda([this](UGetConceptDesignPictureCommentData* SelectPicComment)
        {
            CommentContainer->ClearChildren();

            if (SelectPicComment->items.Num() > 0)
            {
                int32 Index = 1; 
                for (const FPictureCommentItem& CommentItem : SelectPicComment->items)
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
                                .Text(FText::FromString(CommentItem.memberName))
                                .Justification(ETextJustify::Left)
                            ]
                            + SHorizontalBox::Slot()
                            [
                                SNew(STextBlock)
                               .Text(FText::FromString(CommentItem.createTime))
                               .Justification(ETextJustify::Right)
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .Padding(FMargin(0.0, 15.0, 0.0, 0.0)) 
                        [
                        SNew(STextBlock)
                        .Text(FText::FromString(CommentItem.commentContent))
                        .ColorAndOpacity(FSlateColor(FLinearColor::White))
                        .AutoWrapText(true) 
                        .WrapTextAt(600.0f) 
                        ]
                    ]
                    ];

                    // UE_LOG(LogTemp, Error, TEXT("视频评论接口响应成功：评论者: %s; 评论内容：%s"), *CommentItem.memberName, *CommentItem.commentContent);

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
        CurrentPage = 1;
        PageSize = 100;
        int32 PicId = ConceptDesignFileItem.Id;

        SelectPicFileComment->SendGetConceptDesignPictureCommentRequest(Ticket, Uuid, PicId, CurrentPage, PageSize,OnGetConceptDesignPictureCommentResponse);
    }
    
    TSharedPtr<SVerticalBox> TagContainer = SNew(SVerticalBox);
    
    if (Filetag.Num() == 0)
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
        
        for (const FString& CurrentTag : Filetag)
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
                .ContentPadding(FMargin(10.f, 5.f))
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TruncateText(CurrentTag, 12)))
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

    ConceptFileName = ConceptDesignFileItem.Name;
    
    bool bIsFileDownloaded = IsFileDownloaded(ConceptDesignFileItem.Name);

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
                         ConstructImageItem(ConceptDesignFileItem.RelativePatch)
                    ]
                    
                    + SOverlay::Slot()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Bottom)
                    .Padding(10)  
                    [
                        SNew(SButton)
                        .Cursor(EMouseCursor::Hand)
                        .ButtonStyle(&PlayImageCircleButtonStyle)
                        .ContentPadding(0) 
                        .OnClicked_Lambda([this, ImagePath = ConceptDesignFileItem.RelativePatch]() -> FReply
                        {
                        bIsButtonEnabled = false;
                            
                        FMediaPlayerManager::ShowImageInWindow(ImagePath);
                            
                        bIsButtonEnabled = true;

                        return FReply::Handled();
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
                          .Text(FText::FromString(TruncateText(ConceptFileName, 26)))
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
                        .Text(LOCTEXT("BasicInformation", "Base Info"))
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
                            .Text(LOCTEXT("Size", "Size"))
                            .Justification(ETextJustify::Left)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f) 
                        .HAlign(HAlign_Right)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(ConceptDesignFileItem.FileSize))
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
                            .Text(FText::FromString(FormatFileSize(ConceptDesignFileItem.FileLength))) 
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
                            .Text(LOCTEXT("FileType", "File Type"))
                            .Justification(ETextJustify::Left)
                        ]
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f) 
                        .HAlign(HAlign_Right)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(ConceptDesignFileItem.FileSuffix))
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
                            .Text(FText::FromString(ConceptDesignFileItem.CreateTime))
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
                            .Text(LOCTEXT("ModificationTime", "Last Modified Time"))
                            .Justification(ETextJustify::Left)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)  
                        .HAlign(HAlign_Right)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(ConceptDesignFileItem.CreateTime))
                            .Justification(ETextJustify::Right)
                            .ColorAndOpacity(FSlateColor(FLinearColor::White))
                        ]
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
                            .IsEnabled_Lambda([this]() { return bIsButtonEnabled && !IsFileDownloaded(ConceptFileName); })
                             .OnClicked_Lambda([this, ConceptDesignFileItem]()-> FReply
                            {
                                      float CurrentTime = FPlatformTime::Seconds();
                                 
                                      if (CurrentTime - LastClickTime < ClickCooldownTime)
                                      {
                                          return FReply::Handled();  
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
                                 
                               SelectedConceptFileInfo.FileName = ConceptDesignFileItem.Name;
                               SelectedConceptFileInfo.FileUrl = ConceptDesignFileItem.RelativePatch;
                               SelectedConceptFileInfo.FileMd5 = ConceptDesignFileItem.FileMd5;

                               if (ConceptDesignFileItem.FileSize == "0")
                               {
                                   return FReply::Handled();
                               }
                               OnDownloadConceptButtonClicked(SelectedConceptFileInfo);

                                 bool bIsFileDownloaded = IsFileDownloaded(ConceptFileName);

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
                         .AutoWidth()
                         .HAlign(HAlign_Right)
                         .Padding(5, 2, 5, 2)
                         [
                             SNew(SButton)
                             .Cursor(EMouseCursor::Hand)
                             .ButtonStyle(&ImportButtonStyle)
                             .OnClicked_Lambda([this, ConceptDesignFileItem]()-> FReply
                            {
                               SelectedConceptFileInfo.FileName = ConceptDesignFileItem.Name;
                               SelectedConceptFileInfo.FileUrl = ConceptDesignFileItem.RelativePatch;
                               SelectedConceptFileInfo.FileMd5 = ConceptDesignFileItem.FileMd5;
                               OnImportConceptFileButtonClicked(SelectedConceptFileInfo);
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

FString SConceptDesignWidget::FormatFileSize(int64 FileSizeInBytes) const
{
    const float Kb = 1024.0f;
    const float Mb = Kb * 1024.0f;
    const float Gb = Mb * 1024.0f;

    FString FormattedSize;
    if (FileSizeInBytes < Kb)
    {
        FormattedSize = FString::Printf(TEXT("%lld B"), FileSizeInBytes);
    }
    else if (FileSizeInBytes < Mb)
    {
        FormattedSize = FString::Printf(TEXT("%.2f KB"), FileSizeInBytes / Kb);
    }
    else if (FileSizeInBytes < Gb)
    {
        FormattedSize = FString::Printf(TEXT("%.2f MB"), FileSizeInBytes / Mb);
    }
    else
    {
        FormattedSize = FString::Printf(TEXT("%.2f GB"), FileSizeInBytes / Gb);
    }

    return FormattedSize;
}

bool SConceptDesignWidget::IsFileDownloaded(const FString& ForbidFileName) const
{
    FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), ForbidFileName);

    return FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath);
}

void SConceptDesignWidget::OnDownloadCompleted(const FString& AssetFileName)
{
    // If the downloaded file name matches the current file, the download button is disabled 如果下载的文件名与当前文件匹配，则禁用下载按钮
    if (AssetFileName == ConceptFileName)
    {
        if (DownloadButton.IsValid())
        {
            DownloadButton->Invalidate(EInvalidateWidget::LayoutAndVolatility);
        }
    }
}

void SConceptDesignWidget::ClearConceptContent()
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
    
    if (ConceptDesignAssetsContainer.IsValid())
    {
        ConceptDesignAssetsContainer->ClearChildren();
    }
    else
    {
        // UE_LOG(LogTemp, Warning, TEXT("ConceptDesignAssetsContainer is invalid or not initialized."));
    }
}


TSharedRef<SWidget> SConceptDesignWidget::ConstructImageItem(const FString& ProjectImageUrl)
{
    TSharedPtr<SBox> ImageBox = SNew(SBox)
        .WidthOverride(150.f)
        .HeightOverride(100.f)
        [
            // Initial placeholder content 初始占位内容
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

    // Determine if there is a valid image URL 判断是否有有效的图片 URL
    if (!ProjectImageUrl.IsEmpty())
    {
        // Asynchronously loading picture 异步加载图片
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
                    LoadedTexture->AddToRoot(); 
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
                            .Text(LOCTEXT("LoadFailed", "Load Failed"))
                            .Justification(ETextJustify::Center)
                        ]
                    ]
                );
            }
        }));
    }
    else
    {
        // If there is no URL, the default background is displayed 如果没有 URL，显示默认背景
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




FReply SConceptDesignWidget::OnDownloadConceptButtonClicked(FSelectedConceptFileInfo ConceptInfo)
{
    if (OnSelectedConceptDownloadClicked.IsBound())
    {
        OnSelectedConceptDownloadClicked.Execute(ConceptInfo.FileName, ConceptInfo.FileUrl, ConceptInfo.FileMd5);
    }
    
    return FReply::Handled();
}

FReply SConceptDesignWidget::OnImportConceptFileButtonClicked(const FSelectedConceptFileInfo& InSelectedConceptFileInfo)
{
    
    FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), InSelectedConceptFileInfo.FileName);
    
    ImportConceptFile(FilePath);

    return FReply::Handled();
}

void SConceptDesignWidget::ImportConceptFile(const FString& FilePath)
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
                    .Text(LOCTEXT("ResourceDownloading", "Resource is downloading！"))
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

        // If the window already exists and is valid, put it first 如果窗口已存在且有效，将其置于最前
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

    // Handle file names by replacing "." with "_", but keeping the extension 处理文件名，将 "." 替换为 "_"，但保留扩展名
    FString NewFileName = FPaths::GetBaseFilename(FilePath).Replace(TEXT("."), TEXT("_"));
    FString PackageName = TEXT("/Game/RSpaceImportedAssets/Images/") + NewFileName;

    // Check whether assets with the same name already exist 检查是否已经存在同名资产
    if (FPackageName::DoesPackageExist(PackageName))
    {
        UE_LOG(LogTemp, Log, TEXT("Texture already exists, skipping import: %s"), *PackageName);
        return;
    }

    // Load files into memory 加载文件到内存
    TArray<uint8> FileData;
    if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
        return;
    }

    // Detection image format 检测图像格式
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
    EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(FileData.GetData(), FileData.Num());
    if (ImageFormat == EImageFormat::Invalid)
    {
        UE_LOG(LogTemp, Error, TEXT("Unrecognized image format: %s"), *FilePath);
        return;
    }

    // Create an image wrapper 创建图像包装
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
    if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num()))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create image wrapper: %s"), *FilePath);
        return;
    }

    // Unzip image data 解压图像数据
    TArray<uint8> UncompressedRGBA;
    if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedRGBA))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to decompress image data: %s"), *FilePath);
        return;
    }

    // Create package 创建包
    UPackage* Package = CreatePackage(*PackageName);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *PackageName);
        return;
    }

    // Create a UTexture2D object 创建 UTexture2D 对象
    UTexture2D* LoadedTexture = NewObject<UTexture2D>(Package, FName(*NewFileName), RF_Public | RF_Standalone);
    if (!LoadedTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create texture object: %s"), *NewFileName);
        return;
    }

    // Initialize the texture platform data 初始化纹理平台数据
    FTexturePlatformData* PlatformData = new FTexturePlatformData();
    PlatformData->SizeX = ImageWrapper->GetWidth();
    PlatformData->SizeY = ImageWrapper->GetHeight();
    PlatformData->PixelFormat = PF_B8G8R8A8;

    // Create MipMap data 创建 MipMap 数据
    FTexture2DMipMap* Mip = new FTexture2DMipMap();
    PlatformData->Mips.Add(Mip);
    Mip->SizeX = ImageWrapper->GetWidth();
    Mip->SizeY = ImageWrapper->GetHeight();
    Mip->BulkData.Lock(LOCK_READ_WRITE);
    void* TextureData = Mip->BulkData.Realloc(UncompressedRGBA.Num());
    FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
    Mip->BulkData.Unlock();

    // Set up PlatformData 设置 PlatformData
    LoadedTexture->SetPlatformData(PlatformData);

    // Set texture properties 设置纹理属性
    LoadedTexture->SRGB = true;
    LoadedTexture->CompressionSettings = TC_Default;
    LoadedTexture->MipGenSettings = TMGS_FromTextureGroup;
    LoadedTexture->UpdateResource();

    // Mark the package as dirty 标记包为脏
    Package->MarkPackageDirty();

    // Save the package to disk 保存包到磁盘
    FString PackageFilePath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | RF_Standalone;
    SaveArgs.Error = GError;
    SaveArgs.SaveFlags = SAVE_NoError;

    if (UPackage::SavePackage(Package, nullptr, *PackageFilePath, SaveArgs))
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully saved texture to package: %s"), *PackageFilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save package: %s"), *PackageFilePath);
    }
}






#undef LOCTEXT_NAMESPACE
