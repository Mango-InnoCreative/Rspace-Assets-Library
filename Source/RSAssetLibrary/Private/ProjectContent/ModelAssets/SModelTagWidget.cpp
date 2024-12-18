// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectContent/ModelAssets/SModelTagWidget.h"
#include "RSAssetLibraryStyle.h"
#include "Async/Async.h"
#include "ModelLibrary/GetModelAssetLibraryTagListApi.h"
#include "ModelLibrary/GetModelLibrary.h"
#include "ProjectContent/Imageload/FImageLoader.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Subsystem/USMSubsystem.h"
#include "Widgets/Layout/SSeparator.h"

#define LOCTEXT_NAMESPACE "SModelTagWidget"

void SModelTagWidget::Construct(const FArguments& InArgs)
{
    ModelAssetsWidget = InArgs._ModelAssetsWidget;
    OnClearModelTagFilterDelegate = InArgs._OnClearModelTagFilter;
    OnModelTagClickClearWidget = InArgs._OnModelTagClickClearWidget;
    OnModelTagDataReceived = InArgs._OnModelTagDataReceived;
    TagButtonContainer = SNew(SVerticalBox);
    SelectedTagsContainer = SNew(SHorizontalBox);  

    GetModelLibraryApi = NewObject<UGetModelLibrary>();

    ChildSlot
    [
        SNew(SVerticalBox)
        
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()  
            .VAlign(VAlign_Center)  
            .Padding(5)
            [
                SNew(STextBlock)
                .Justification(ETextJustify::Left)
                .Text(LOCTEXT("SelectedText", "已选择："))
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1.0)  
            .HAlign(HAlign_Left)  
            .VAlign(VAlign_Center)  
            .Padding(5)
            [
                
                SNew(SBox)
                .HeightOverride(35)
                [
                    SelectedTagsContainer.ToSharedRef()
                ]
            ]
        ]
        
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SImage)
            .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ProjectLine"))
        ]

 
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("ConditionTypeLabel", "条件类型："))
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                TagButtonContainer.ToSharedRef() 
            ]
        ]
        
        + SVerticalBox::Slot()
        .VAlign(VAlign_Bottom) 
        .HAlign(HAlign_Right) 
        .Padding(10)
        [
            SNew(SButton)
            .ButtonStyle(&ImportButtonStyle) 
            .Cursor(EMouseCursor::Hand)
            .OnClicked(this, &SModelTagWidget::ClearSelectedTags) 
            .ContentPadding(0) 
             [
                 SNew(SBox)
                 .WidthOverride(106)
                 .HeightOverride(40)
                 [
                     SNew(SHorizontalBox)
                     + SHorizontalBox::Slot()
                     .VAlign(VAlign_Center)
                     .HAlign(HAlign_Center)
                     .Padding(0, 5, 0, 5)
                     [
                         SNew(SImage)
                         .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ClearFilter")) 
                     ]
                     + SHorizontalBox::Slot()
                     .VAlign(VAlign_Center)
                     .HAlign(HAlign_Center)
                     .Padding(0, 5, 5, 5)
                     [
                         SNew(STextBlock)
                         .Text(LOCTEXT("ClearFilter", "清空筛选"))
                         .Justification(ETextJustify::Center)
                         .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                         .ColorAndOpacity(FSlateColor(FLinearColor::White))
                     ]
                 ]
             ]
        ]
    ];

    FetchTagList();

    TagButtonStyle.SetNormal(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));  
    TagButtonStyle.SetHovered(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));
    TagButtonStyle.SetPressed(FSlateColorBrush(FLinearColor(0.12f, 0.6f, 0.3f, 0.2f)));
    
    TransparentBrush.TintColor = FSlateColor(FLinearColor::Transparent); 
    ImportButtonStyle.SetNormal(TransparentBrush);
    ImportButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ImportHover"));
    ImportButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Importclicked"));
}



void SModelTagWidget::FetchTagList()
{
    if (!ModelAssetsWidget.IsValid())
    {
        return;
    }
    
    FString Ticket = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Ticket;
    FString ProjectNo = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetSelectedProject().projectNo;

    // Process HTTP requests in a thread pool to avoid blocking the main thread 在线程池中处理 HTTP 请求，避免阻塞主线程
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Ticket, ProjectNo]()
    {
        UGetModelAssetLibraryTagListApi* GetModelAssetLibraryTagListApi = NewObject<UGetModelAssetLibraryTagListApi>();
        if (GetModelAssetLibraryTagListApi)
        {
            FOnGetModelAssetLibraryTagListResponse OnGetModelAssetLibraryTagListResponse;
            OnGetModelAssetLibraryTagListResponse.BindLambda([this](const FGetModelAssetLibraryTagListResponseData& ModelAssetLibraryTagList)
            {
                // Put the UI update logic back into the main thread 将更新 UI 的逻辑放回主线程
                AsyncTask(ENamedThreads::GameThread, [this, ModelAssetLibraryTagList]()
                {
                    if (ModelAssetLibraryTagList.Status == "success" && ModelAssetLibraryTagList.Code == "200")
                    {
                        AddTagButtons(ModelAssetLibraryTagList.Data);
                    }
                });
            });

            GetModelAssetLibraryTagListApi->SendGetModelAssetLibraryTagListRequest(Ticket, ProjectNo, OnGetModelAssetLibraryTagListResponse);
        }
    });
}


void SModelTagWidget::AddTagButtons(const TArray<FModelTagInfo>& TagInfoList)
{
    TagButtonContainer->ClearChildren();

    TSharedPtr<SHorizontalBox> CurrentRow = SNew(SHorizontalBox);
    int32 ButtonCount = 0;

    for (const FModelTagInfo& TagInfo : TagInfoList)
    {
        if (ButtonCount >= 10)
        {
            TagButtonContainer->AddSlot()
            .AutoHeight()
            [
                CurrentRow.ToSharedRef()
            ];

            CurrentRow = SNew(SHorizontalBox);
            ButtonCount = 0;
        }

        CurrentRow->AddSlot()
        .AutoWidth()
        .Padding(5)
        [
            SNew(SButton)
            .ButtonStyle(&TagButtonStyle)
            .Cursor(EMouseCursor::Hand)
            .ContentPadding(FMargin(10.f, 5.f))
            [
                SNew(STextBlock)
                .Text(FText::FromString(TagInfo.TagName))
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
            ]
            .OnClicked_Lambda([this, CurrentFileName = TagInfo.TagName]()
            {
                OnModelTagClickClearWidget.ExecuteIfBound();
                return OnTagButtonClicked(CurrentFileName);
            })
        ];

        ButtonCount++;
    }

    if (ButtonCount > 0)
    {
        TagButtonContainer->AddSlot()
        .AutoHeight()
        [
            CurrentRow.ToSharedRef()
        ];
    }

    OnModelTagDataReceived.ExecuteIfBound();
}

FReply SModelTagWidget::OnTagButtonClicked(const FString& InTagName)
{
    AddTagToSelected(InTagName);
    
    if (GetModelLibraryApi)
    {
        FOnGetModelLibraryResponse OnGetModelLibraryResponseDelegate;
        OnGetModelLibraryResponseDelegate.BindLambda([this](UGetModelLibraryResponseData* ModelLibraryData)
        {
            if (ModelLibraryData)
            {
                for (const FModelFileItem& FileItem : ModelLibraryData->data)
                {
                    // UE_LOG(LogTemp, Error, TEXT("File Name: %s, File Path: %s"), *FileItem.fileName, *FileItem.relativePath);
                }

                FImageLoader::CancelAllImageRequests();
                ModelAssetsWidget->UpdateModelAssets(ModelLibraryData->data);
            }
        });

        FString Uuid = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Uuid;
        FString Ticket = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Ticket;
        FString ProjectNo = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetSelectedProject().projectNo;
        int32 FileId = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentModelRootID();
        FString FileName = "";
        int64 TagId = *"";
        FString TagName = InTagName;
        GetModelLibraryApi->SendGetModelLibraryRequest(Ticket, Uuid, FileId, ProjectNo, FileName, TagId, TagName, OnGetModelLibraryResponseDelegate);
    }

    return FReply::Handled();
}

void SModelTagWidget::AddTagToSelected(const FString& TagName)
{
    SelectedTags.Empty();
    
    SelectedTags.Add(TagName);
    
    SelectedTagsContainer->ClearChildren();

    SelectedTagsContainer->AddSlot()
    .HAlign(HAlign_Left)
    .AutoWidth()
    .Padding(5)
    [
        SNew(SButton)
        .ButtonStyle(&TagButtonStyle)
        .Cursor(EMouseCursor::Hand)
        .ContentPadding(FMargin(10.f, 5.f))
        [
            SNew(STextBlock)
            .Text(FText::FromString(TagName))
            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
        ]
    ];
}



FReply SModelTagWidget::ClearSelectedTags()
{
    SelectedTags.Empty();
    SelectedTagsContainer->ClearChildren();
    FImageLoader::CancelAllImageRequests();
    
    if (OnClearModelTagFilterDelegate.IsBound())
    {
        OnClearModelTagFilterDelegate.Execute(); 
    }

    OnModelTagClickClearWidget.ExecuteIfBound();
    
    // UE_LOG(LogTemp, Warning, TEXT("已选择的标签已清空"));

    return FReply::Handled();
}