// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectContent/ModelAssets/ModelAssetsWidget.h"
#include "DesktopPlatformModule.h"
#include "RSpaceAssetLibApi/Public/ModelLibrary/GetModelLibraryData.h"
#include "RSAssetLibraryStyle.h"
#include "Factories/FbxFactory.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxStaticMeshImportData.h"
#include "ModelLibrary/GetModelFileHistoryApi.h"
#include "ModelLibrary/GetModelFileTagApi.h"
#include "ProjectContent/Imageload/FImageLoader.h"
#include "Widgets/Layout/SScrollBox.h"
#include "HAL/PlatformTime.h"
#include "Containers/Ticker.h"
#include "ModelLibrary/SelectModelFileDetailsInfoApi.h"
#include "ModelLibrary/SwithModelFileVersionApi.h"
#include "ProjectContent/ConceptDesign/ConceptDesignDisplay.h"
#include "ProjectContent/MediaPlayer/SVideoPlayerWidget.h"
#include "Misc/Timespan.h"
#include "Tickable.h"  // 包含与 FTicker 和 FTSTicker 相关的内容

#define LOCTEXT_NAMESPACE "SModelAssetsWidget"

void SModelAssetsWidget::Construct(const FArguments& InArgs)
{
    OnModelAssetClicked = InArgs._OnModelAssetClicked; 
    OnSelectedModelDownloadClicked = InArgs._OnSelectedModelDownloadClicked;
    OnModelNothingToShow = InArgs._OnModelNothingToShow;
    OnModelRefresh = InArgs._OnModelRefresh;
    
    AssetDownloadWidget = SNew(SAssetDownloadWidget);
    
    if (AssetDownloadWidget.IsValid())
    {
        AssetDownloadWidget->BindOnDownloadCompleted(FOnDownloadCompleted::CreateSP(this, &SModelAssetsWidget::OnDownloadCompleted));
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
		    SAssignNew(ModelAssetsContainer, SVerticalBox)
	        ]
	    ]
	];

    InitializeDetailClickedButtonStyle();
}


void SModelAssetsWidget::UpdateModelAssets(const TArray<FModelFileItem>& ModelAssets)
{
    ModelAssetsContainer->ClearChildren();

    TSharedPtr<SHorizontalBox> CurrentRow;
    int32 ItemCount = 0;

    // Defines supported model file type suffixes 定义支持的模型文件类型后缀
    TArray<FString> ModelFileExtensions = { TEXT("fbx"), TEXT("obj"), TEXT("glb"), TEXT("stl") };

    int32 FileCount = 0;
    for (const FModelFileItem& ModelAsset : ModelAssets)
    {
        if (ModelAsset.fileType == 0) // 1 Indicates the folder type 1 表示文件夹类型
        {
            FileCount++;
        }
    }
    
    if ( FileCount > 0 )
    {
        for (const FModelFileItem& FileItem : ModelAssets)
        {
            // UE_LOG(LogTemp, Log, TEXT("SelectedgifFirstImg!!!!!!!!!!!!!!!!!!!!: %s"), *FileItem.gifFirstImg);
            TSharedPtr<FButtonStyle> FolderButtonStyle = MakeShareable(new FButtonStyle(DetailClickedButtonStyle));
           
            if (FileItem.fileType != 0)  // Suppose fileType == 0 represents a file 假设 fileType == 0 表示文件
            {
                continue;
            }

            // Gets the file's extension 获取文件的扩展名
            FString FileExtension = FPaths::GetExtension(FileItem.fileName).ToLower();

            auto OnClicked = [this, FileItem, FolderButtonStyle]() -> FReply
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
                
                if (OnModelAssetClicked.IsBound())
                {
                    UGetModelFileTagApi* GetModelFileTag = NewObject<UGetModelFileTagApi>();
                    if (GetModelFileTag)
                    {
                        FOnGetModelFileTagResponse OnGetModelFileTagResponse;
                        
                        OnGetModelFileTagResponse.BindLambda([this, FileItem](const FModelFileTagData& GetModelFileTagData)
                        {
                            // Update the Tag and parse it into an array 更新Tag并解析为数组
                            Tag = GetModelFileTagData.data;
                            TagArray.Empty();
                            Tag.ParseIntoArray(TagArray, TEXT(","), true); // true removes the empty element true表示移除空元素
                        });
                        
                        SetUserAndProjectParams();
                        FString FileNo = FileItem.fileNo;
                        GetModelFileTag->SendGetModelFileTagRequest(Ticket, FileNo, ProjectNo, OnGetModelFileTagResponse);
                    }

                    UGetModelFileHistoryApi* GetModelFileHistoryApi = NewObject<UGetModelFileHistoryApi>();
                    if (GetModelFileHistoryApi)
                    {
                        FOnGetModelFileHistoryResponse OnGetModelFileHistoryResponse;
                        OnGetModelFileHistoryResponse.BindLambda([this, FileItem](UGetModelFileHistoryResponseData* GetModelFileHistoryData)
                        {
                            if (GetModelFileHistoryData)
                            {
                                VersionOptions.Empty();

                                // Iterate through each version in the response data, adding to the drop-down menu options 遍历响应数据中的每个版本，添加到下拉菜单选项中
                                for (const FModelFileHistoryItem& FileHistoryItem : GetModelFileHistoryData->data)
                                {
                                    // Add version as the key and relativePath as the value to the TMap 将 version 作为键，relativePath 作为值，加入到 TMap 中
                                    VersionToFileNoPathMap.Add(FileHistoryItem.version , FileHistoryItem.fileNo);
                                    VersionToRelativePathMap.Add(FileHistoryItem.version , FileHistoryItem.relativePath);
                                    VersionOptions.Add(MakeShared<FString>(FString::Printf(TEXT("%d"), FileHistoryItem.version)));

                                        USelectModelFileDetailsInfoApi* SelectModelFileDetailsInfoApi = NewObject<USelectModelFileDetailsInfoApi>();
                                       if (SelectModelFileDetailsInfoApi)
                                       {
                                           FOnSelectModelFileDetailsInfoResponse OnSelectModelFileDetailsInfoResponse;
                                           OnSelectModelFileDetailsInfoResponse.BindLambda([this, FileItem](USelectModelFileDetailsInfoData* ModelFileDetailsData)
                                           {
                                               if (ModelFileDetailsData)
                                               {
                                                   const FModelFileDetails& FileDetails = ModelFileDetailsData->data;

                                                   InitialModelVersion = FileDetails.version;
                                                   TSharedRef<SWidget> DetailsWidget = GenerateDetailsWidget(FileDetails);
                                                   OnModelAssetClicked.Execute(FileItem, DetailsWidget);
                                               }
                                           });

                                           FString FileNo = FileItem.fileNo;
                                           SelectModelFileDetailsInfoApi->SendSelectModelFileDetailsInfoRequest(Ticket, FileNo, OnSelectModelFileDetailsInfoResponse);
                                       }
                                }

                                // Notify the drop-down box to update options 通知下拉框更新选项
                                if (VersionComboBox.IsValid())
                                {
                                    VersionComboBox->RefreshOptions();
                                }
                            }
                        });

                        SetUserAndProjectParams();
                        FString FileNo = FileItem.fileNo;
                        GetModelFileHistoryApi->SendGetModelFileHistoryRequest(FileNo, Ticket, OnGetModelFileHistoryResponse);
                    }
                        
                   
                }
                
                return FReply::Handled();
            };

            if (ItemCount % 5 == 0)
            {
                CurrentRow = SNew(SHorizontalBox);
                ModelAssetsContainer->AddSlot().AutoHeight().Padding(0)[ CurrentRow.ToSharedRef() ];
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
                        + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
                        .Padding(0, 16, 0, 0)
                        [
                            SNew(SBox)
                            .WidthOverride(150.0f).HeightOverride(100.0f)
                            [
                                LoadImageFromUrl(FileItem.gifFirstImg)
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
                                    SNew(STextBlock).Text(FText::FromString(TruncateText(FileItem.fileName, 14)))
                                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10)).Justification(ETextJustify::Center)
                                ]
                            ]
                        ]
                    ]
                ]
            ];

            ItemCount++;
        }

        OnModelRefresh.ExecuteIfBound();
    }
    else
    {
        OnModelNothingToShow.ExecuteIfBound();
    }
    
}

void SModelAssetsWidget::InitializeDetailClickedButtonStyle()
{
    DetailClickedButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ContentBorder"));
    DetailClickedButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButtonClicked"));
    DetailClickedButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ModerBorderButton"));
}

FString SModelAssetsWidget::TruncateText(const FString& OriginalText, int32 MaxLength)
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

void SModelAssetsWidget::SetUserAndProjectParams()
{
    USMSubsystem = GEditor->GetEditorSubsystem<UUSMSubsystem>();
    Ticket = USMSubsystem->GetCurrentUserAndProjectInfo().Ticket;
    Uuid = USMSubsystem->GetCurrentUserAndProjectInfo().Uuid;
    ProjectNo = USMSubsystem->GetSelectedProject().projectNo;
}



TSharedRef<SWidget> SModelAssetsWidget::GenerateDetailsWidget(const FModelFileDetails& FileDetailsItem)
{
    ImportButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadBarBorder"));
    ImportButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ImportHover"));
    ImportButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Importclicked"));
    
    TagButtonStyle.SetNormal(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));  
    TagButtonStyle.SetHovered(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));
    TagButtonStyle.SetPressed(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));
    
    FString FileExtension = FPaths::GetExtension(FileDetailsItem.fileName);
    FString VersionString = FString::FromInt(FileDetailsItem.version);
    if (!FileExtension.IsEmpty())
    {
        // The file has an extension 文件有扩展名
        FString FileNameWithoutExtension = FPaths::GetBaseFilename(FileDetailsItem.fileName);
        ModelFileName = FileNameWithoutExtension + "_V" + VersionString + "." + FileExtension;
    }
    else
    {
        // The file does not have an extension, the version number is added directly after the file name 文件没有扩展名，直接在文件名后添加版本号
        ModelFileName = FileDetailsItem.fileName + "_V" + VersionString;
    }
    
    TSharedPtr<SVerticalBox> TagContainer = SNew(SVerticalBox);

    // If the TagArray array is empty, "No labels yet" is displayed 如果 TagArray 数组为空，则显示"暂无标签"
    if (TagArray.Num() == 0)
    {
        TagContainer->AddSlot()
        [
            SNew(SHorizontalBox) 
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5, 0, 5, 15)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("NoTags", "暂无标签"))
            ]
        ];
    }
    else
    {
        TSharedPtr<SHorizontalBox> CurrentRow;
        int32 TagCount = 0;
        
        for (const FString& CurrentTag : TagArray)
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
    
    TSharedPtr<FSlateBrush> ImageBrush = MakeShareable(new FSlateImageBrush(FName(*FileDetailsItem.gifUrl), FVector2D(256, 256))); 
    
    if (VersionOptions.Num() > 0)
    {
        SelectedVersion = MakeShared<FString>(FString::Printf(TEXT("%d"), FileDetailsItem.version));
        int32 DefaultVersionNumber = FCString::Atoi(**SelectedVersion);
    
        if (VersionToRelativePathMap.Contains(DefaultVersionNumber))
        {
            SelectedRelativePath = MakeShared<FString>(VersionToRelativePathMap[DefaultVersionNumber]);
        }
    }

    //SelectedVersion = MakeShared<FString>(FString::Printf(TEXT("%d"), InitialModelVersion));
    PlayImageCircleButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayImage"));
    PlayImageCircleButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayImageHover"));
    PlayImageCircleButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PlayImageHover"));

   
    bool bIsFileDownloaded = IsFileDownloaded(ModelFileName);

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
                        LoadImageFromUrl(FileDetailsItem.gifFirstImg)
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
                        .OnClicked_Lambda([this, ImagePath = FileDetailsItem.gifFirstImg]() -> FReply
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
                            .Text(FText::FromString(TruncateText(ModelFileName, 26)))
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
                        .Text(LOCTEXT("BasicInfoTitle", "基本信息"))
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
                            .Text(LOCTEXT("CreateTimeLabel", "创建时间"))
                            .Justification(ETextJustify::Left)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)  
                        .HAlign(HAlign_Right)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(FileDetailsItem.createTime))  // Here you can keep the dynamic text.
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
                            .Text(LOCTEXT("LastModifyTimeLabel", "最后修改时间"))
                            .Justification(ETextJustify::Left)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f) 
                        .HAlign(HAlign_Right)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(FileDetailsItem.updateTime))  // Here you can keep the dynamic text.
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
                            .Text(LOCTEXT("CreatorLabel", "创建者"))
                            .Justification(ETextJustify::Left)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f) 
                        .HAlign(HAlign_Right)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(FileDetailsItem.createrBy))  // Here you can keep the dynamic text.
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
                            .Text(LOCTEXT("LastModifyByLabel", "最后修改者"))
                            .Justification(ETextJustify::Left)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f) 
                        .HAlign(HAlign_Right)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(FileDetailsItem.updateBy))  // Here you can keep the dynamic text.
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
                        .Text(LOCTEXT("TagsTitle", "标签"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                        .ColorAndOpacity(FSlateColor(FLinearColor::White))
                    ]
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(20, 0, 25, 15)
                    [
                        TagContainer.ToSharedRef()
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
                        .HAlign(HAlign_Fill) 
                        .FillWidth(1.0f) 
                        [
                             SNew(SHorizontalBox)
                               + SHorizontalBox::Slot()
                               .FillWidth(1.0f)
                               .HAlign(HAlign_Left) 
                               .Padding(10, 2, 10, 2)
                               [
                                   SNew(SBox)
                                   .HeightOverride(40)
                                   .WidthOverride(210)
                                   [
                                        // Initialize the drop-down box 初始化下拉框
                                        SAssignNew(VersionComboBox, SComboBox<TSharedPtr<FString>>)  
                                        .OptionsSource(&VersionOptions)  // Set the option source for the drop-down box 设置下拉框的选项源
                                        .OnGenerateWidget_Lambda([](TSharedPtr<FString> InOption)
                                        {
                                           // Precede the options with a "V" 在选项内容前面加上 "V"
                                           FString DisplayText = FString::Printf(TEXT("V%s"), **InOption);
                                           return SNew(STextBlock)
                                               .Text(FText::FromString(DisplayText));  // Display content for each option 每个选项的显示内容
                                        })
                                        .OnSelectionChanged(this, &SModelAssetsWidget::OnVersionSelected)  // Version selection event 版本选择事件
                                        .InitiallySelectedItem(MakeShared<FString>(FString::Printf(TEXT("%d"), FileDetailsItem.version)))  // FileItem.version is displayed by default 默认显示 FileItem.version
                                        .Content()
                                        [
                                           SNew(STextBlock)
                                           .Text_Lambda([this, FileDetailsItem]()
                                           {
                                               // Returns the currently selected version number, or the default version number if not selected 返回当前选中的版本号，如果没有选中则显示默认的版本号
                                               return FText::FromString(SelectedVersion.IsValid() ? FString::Printf(TEXT("V%s"), **SelectedVersion) : FString::Printf(TEXT("V%d"), FileDetailsItem.version));
                                           })
                                        ]
                                    ]
                               ]
                               
                             + SHorizontalBox::Slot()
                             .AutoWidth()
                             .HAlign(HAlign_Right)
                             .Padding(5, 2, 5, 2)
                             [
                                SAssignNew(DownloadButton, SButton)  
                                 .Cursor(EMouseCursor::Hand)
                                 .ButtonStyle(&ImportButtonStyle) 
                                 .ContentPadding(0) 
                                .IsEnabled_Lambda([this]() { return bIsButtonEnabled && !IsFileDownloaded(ModelFileName); })
                                 .OnClicked_Lambda([this, FileDetailsItem]()-> FReply
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
                                     
                                     
                                        OnModelFileDownloadButtonClicked(FileDetailsItem, *SelectedRelativePath);

                                         bool bIsFileDownloaded = IsFileDownloaded(ModelFileName);

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
                                 .ContentPadding(0) 
                                 .OnClicked(this, &SModelAssetsWidget::OnImportFBXButtonClicked)
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
                                         //     //.Text(LOCTEXT("Import", "导入"))
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

// Version selection callback function 版本选择回调函数
void SModelAssetsWidget::OnVersionSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (NewSelection.IsValid())
    {
        // UE_LOG(LogTemp, Log, TEXT("Selected Version: %s"), **NewSelection);
        SelectedVersion = NewSelection;
        
        int32 SelectedVersionNumber = FCString::Atoi(**NewSelection);
        // UE_LOG(LogTemp, Log, TEXT("SelectedVersionNumber: %d"), SelectedVersionNumber);

        // Obtain the corresponding relativePath from the VersionToRelativePathMap 从 VersionToRelativePathMap 中获取对应的 relativePath
        if (VersionToRelativePathMap.Contains(SelectedVersionNumber))
        {
            SelectedRelativePath = MakeShared<FString>(VersionToRelativePathMap[SelectedVersionNumber]);
        }
        
        if (VersionToFileNoPathMap.Contains(SelectedVersionNumber))
        {
            SelectedFileNo = MakeShared<FString>(VersionToFileNoPathMap[SelectedVersionNumber]);

           USwithModelFileVersionApi* SwithModelFileVersion = NewObject<USwithModelFileVersionApi>();
            if (SwithModelFileVersion)
            {
                FSwithModelFileVersionApiResponse SwithModelFileVersionApiResponse;
                SwithModelFileVersionApiResponse.BindLambda([this](const FSwithModelFileVersionData& SwithModelFileVersionData)
                {
                        FString ResponeseCode = SwithModelFileVersionData.code;
                        FString ResponeseMessage = SwithModelFileVersionData.message;
                          USelectModelFileDetailsInfoApi* SelectModelFileDetailsInfo = NewObject<USelectModelFileDetailsInfoApi>();
                          if (SelectModelFileDetailsInfo)
                          {
                              FOnSelectModelFileDetailsInfoResponse OnSelectModelFileDetailsInfoResponse;
                              OnSelectModelFileDetailsInfoResponse.BindLambda([this](USelectModelFileDetailsInfoData* NewSelectModelFileDetailsInfoData)
                              {
                                  if (NewSelectModelFileDetailsInfoData)
                                  {
                                      const FModelFileDetails& SelectFileDetails = NewSelectModelFileDetailsInfoData->data;
                                      // UE_LOG(LogTemp, Error, TEXT("切换版本后的********///*//*/* Version: %d, fileMd5: %s"), SelectFileDetails.version, *SelectFileDetails.fileMd5);

                                      InitialModelVersion = SelectFileDetails.version;

                                      TSharedRef<SWidget> SelectedDetailsWidget = GenerateDetailsWidget(SelectFileDetails);

                                      // Passed to the parent class by delegation 通过委托传递给父类
                                      if (OnModelUpdateDetailsBar.IsBound())
                                      {
                                          // Update the version content selected in the drop-down box 更新下拉框选中的版本内容
                                          SelectedVersion = MakeShared<FString>(FString::Printf(TEXT("%d"), InitialModelVersion));
                                          OnModelUpdateDetailsBar.Execute(SelectedDetailsWidget); // Call the UpdateDetailsBar of the parent class 调用父类的 UpdateDetailsBar
                                      }
                                    
                                  }
                              });
                              SetUserAndProjectParams();
                              FString FileNo = SelectedFileNo.IsValid() ? *SelectedFileNo : TEXT("");
                              SelectModelFileDetailsInfo->SendSelectModelFileDetailsInfoRequest(Ticket, FileNo, OnSelectModelFileDetailsInfoResponse);
                          }
                    
                });
                SetUserAndProjectParams();
                int32 Version = SelectedVersionNumber;
                // UE_LOG(LogTemp, Error, TEXT("1111111111111111111111111: %d"), Version);
                FString FileNo = SelectedFileNo.IsValid() ? *SelectedFileNo : TEXT("");
                SwithModelFileVersion->SendSwithModelFileVersionRequest(Ticket, FileNo, Uuid, Version, SwithModelFileVersionApiResponse);
            }

        }
    }

}

void SModelAssetsWidget::ClearModelContent()
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

    if (ModelAssetsContainer.IsValid())
    {
        ModelAssetsContainer->ClearChildren();
    }
}



bool SModelAssetsWidget::IsFileDownloaded(const FString& ForbidFileName) const
{
    FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), ForbidFileName);
    
    // Check whether the file exists 检查文件是否存在
    return FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath);
}

void SModelAssetsWidget::OnDownloadCompleted(const FString& AssetFileName)
{
    // If the downloaded file name matches the current file, the download button is disabled 如果下载的文件名与当前文件匹配，则禁用下载按钮
    if (AssetFileName == ModelFileName)
    {
        if (DownloadButton.IsValid())
        {
            DownloadButton->Invalidate(EInvalidateWidget::LayoutAndVolatility);
        }
    }
}


void SModelAssetsWidget::OnModelFileDownloadButtonClicked(FModelFileDetails ModelFileDetailsItem, const FString& RelativePath)
{
    if (ModelFileDetailsItem.fileSize > 0)
    {
        SelectedModelFileInfo.FileName = ModelFileName;
        SelectedModelFileInfo.FileUrl = RelativePath;
        SelectedModelFileInfo.FileMd5 = ModelFileDetailsItem.fileMd5;

        // UE_LOG(LogTemp, Warning, TEXT("FileName Is : %s"), *SelectedModelFileInfo.FileName);
        // UE_LOG(LogTemp, Warning, TEXT("FileUrl Is : %s"), *SelectedModelFileInfo.FileUrl);
        // UE_LOG(LogTemp, Warning, TEXT("FileMd5 Is : %s"), *SelectedModelFileInfo.FileMd5);

        if (OnSelectedModelDownloadClicked.IsBound())
        {
            OnSelectedModelDownloadClicked.Execute(SelectedModelFileInfo.FileName, SelectedModelFileInfo.FileUrl, SelectedModelFileInfo.FileMd5);
        }
    }
}

FReply SModelAssetsWidget::OnImportFBXButtonClicked()
{
    FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RspaceAssetsLibrary"), ModelFileName);
    
    ImportFBXFile(FilePath);

    return FReply::Handled();
}


void SModelAssetsWidget::ImportFBXFile(const FString& FilePath)
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
        
        ExistingNotificationWindow = SNew(SWindow)
            .Title(LOCTEXT("TipTitle", "提示"))
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
                    .Text(LOCTEXT("ResourceDownloading", "资源正在下载中，无法导入！"))
                    .Justification(ETextJustify::Center)
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10, 20, 10, 10)
                .HAlign(HAlign_Center)
                [
                    SNew(SButton)
                    .Text(LOCTEXT("Confirm", "确定"))
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

    // Check whether the file exists 检查文件是否存在
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
            .Title(LOCTEXT("TipTitle", "提示"))
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
                .Text(LOCTEXT("DownloadResourceMessage", "请先下载该资源！"))
                .Justification(ETextJustify::Center)
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(10, 20, 10, 10)
            .HAlign(HAlign_Center)
            [
                SNew(SButton)
                .VAlign(VAlign_Bottom)
                .Text(LOCTEXT("Confirm", "确定"))
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
        // Display window 显示窗口
        FSlateApplication::Get().AddWindow(ExistingNotificationWindow2.ToSharedRef());
        ExistingNotificationWindow2->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>&)
        {
            ExistingNotificationWindow2.Reset(); 
        }));
        
        return;
    }

    // Creating an import path 创建导入路径
    FString ImportPath = TEXT("/Game/RSpaceImportedAssets/Models");

    // Gets the file name (including the extension) and finds the location of the last '. ' 获取文件名（包含扩展名）并找到最后一个 `.` 的位置
    FString FileNameWithExtension = FPaths::GetCleanFilename(FilePath);
    int32 LastDotIndex;
    if (FileNameWithExtension.FindLastChar('.', LastDotIndex))
    {
        // Separate file name and extension 分离文件名和扩展名
        FString NamePart = FileNameWithExtension.Left(LastDotIndex).Replace(TEXT("."), TEXT("_")); // Replace all '. 'in the file name 替换文件名中的所有 `.`
        FString ExtensionPart = FileNameWithExtension.RightChop(LastDotIndex); // Keep the extension (including the last '. ') 保留扩展名（包括最后的 `.`）

        // Combine new file names 组合新的文件名
        FileNameWithExtension = NamePart + ExtensionPart;
    }

    // Gets the file name used for the display (remove the extension and replace '. 'with' _ ') 获取显示用的文件名（去掉扩展名，替换 `.` 为 `_`）
    FString DisplayFileName = FPaths::GetBaseFilename(FileNameWithExtension).Replace(TEXT("."), TEXT("_"));

    // Construct package name 构造包名称
    FString PackageName = ImportPath + TEXT("/") + DisplayFileName;

    // Check whether the package already exists 检查包是否已经存在
    UPackage* ExistingPackage = FindPackage(nullptr, *PackageName);
    if (ExistingPackage)
    {
        UE_LOG(LogTemp, Warning, TEXT("Package already exists: %s"), *PackageName);
        return;
    }

    // Create package 创建包
    UPackage* Package = CreatePackage(*PackageName);

    // Configure import options 配置导入选项
    UFbxFactory* FbxFactory = NewObject<UFbxFactory>(UFbxFactory::StaticClass());
    FbxFactory->ImportUI->bAutomatedImportShouldDetectType = true; // Automatic detection type 自动检测类型
    FbxFactory->ImportUI->bImportAsSkeletal = true;               // Nonskeletal model 非骨骼模型
    FbxFactory->ImportUI->bImportMaterials = true;                // Import material 导入材质
    FbxFactory->ImportUI->bImportTextures = true;                 // mport texture 导入纹理
    FbxFactory->ImportUI->bImportAnimations = false;              // Do not import animation 不导入动画
    FbxFactory->ImportUI->MeshTypeToImport = FBXIT_SkeletalMesh;  // Imported bone model 导入骨骼模型
    FbxFactory->ImportUI->bImportMesh = true;                     // Lead-in grid 导入网格
    FbxFactory->ImportUI->bAutoComputeLodDistances = true;
    FbxFactory->ImportUI->StaticMeshImportData->bCombineMeshes = true;
    FbxFactory->ImportUI->StaticMeshImportData->ImportUniformScale = 1.0f;
    FbxFactory->ImportUI->StaticMeshImportData->bAutoGenerateCollision = true;

    // Do not display dialog box, import directly 不显示对话框，直接导入
    EObjectFlags Flags = RF_Transactional | RF_Public | RF_Standalone;

    // Call the ImportObject method to import the asset 调用 ImportObject 方法导入资产
    bool bOutCanceled = false;
    const TCHAR* Parms = nullptr;
    UObject* ImportedAsset = FbxFactory->ImportObject(UStaticMesh::StaticClass(), Package, FName(*DisplayFileName), Flags, FilePath, Parms, bOutCanceled);

    // Check whether the import is successful 检查导入是否成功
    if (ImportedAsset && !bOutCanceled)
    {
        UE_LOG(LogTemp, Log, TEXT("FBX Import Successful!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FBX Import Failed or Canceled!"));
    }
}

TSharedRef<SWidget> SModelAssetsWidget::LoadImageFromUrl(const FString& GifUrl)
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
                    .Text(LOCTEXT("Loading", "加载中..."))
                    .Justification(ETextJustify::Center)
                ]
            ]
        ];
    

    if (!GifUrl.IsEmpty())
    {
  
        FImageLoader::LoadImageFromUrl(GifUrl, FOnProjectImageReady::CreateLambda([this, GifUrl, ImageBox](const TArray<uint8>& ImageData)
        {
            if (ImageData.Num() > 0)
            {
                UTexture2D* LoadedTexture = FImageLoader::CreateTextureFromBytes(ImageData);
                if (LoadedTexture && ImageBox.IsValid())
                {
                    // UE_LOG(LogTemp, Log, TEXT("Image loaded successfully"));
       
                    ImageBox->SetContent(
                        SNew(SImage)
                        .Image(new FSlateImageBrush(LoadedTexture, FVector2D(150, 150)))
                    );

                    LoadedTextures.Add(LoadedTexture);
                    LoadedTexture->AddToRoot();
                }
                else
                {
                    // UE_LOG(LogTemp, Warning, TEXT("Failed to load texture from bytes or ImageBox is invalid: %s"), *GifUrl);
                    
           
                    ImageBox->SetContent(
                        SNew(SBorder)
                        .BorderBackgroundColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f))
                        [
                            SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("ImageLoadFailed", "图片加载失败"))
                                .Justification(ETextJustify::Center)
                            ]
                        ]
                    );
                }
            }
            else
            {
                // UE_LOG(LogTemp, Error, TEXT("Image data is empty, failed to download or decode image from URL: %s"), *GifUrl);
                
    
                ImageBox->SetContent(
                    SNew(SBorder)
                    .BorderBackgroundColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f))
                    [
                        SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("ImageLoadFailed", "图片加载失败"))
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
                    .Text(LOCTEXT("NoPreviewText", "无预览图"))
                    .Justification(ETextJustify::Center)
                ]
            ]
        );
    }

    return ImageBox.ToSharedRef();
}




#undef LOCTEXT_NAMESPACE