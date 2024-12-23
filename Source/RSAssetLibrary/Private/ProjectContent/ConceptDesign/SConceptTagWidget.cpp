// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectContent/ConceptDesign/SConceptTagWidget.h"

#include "RSAssetLibraryStyle.h"
#include "Async/Async.h"
#include "ConceptDesignLibrary/GetConceptDesignLibMenuApi.h"
#include "ConceptDesignLibrary/GetConceptDesignLibraryTagListApi.h"
#include "Widgets/Layout/SSeparator.h"
#include "Subsystem/USMSubsystem.h"
#include "ProjectContent/ConceptDesign/ConceptDesignWidget.h"
#include "ProjectContent/Imageload/FImageLoader.h"

#define LOCTEXT_NAMESPACE "SConceptTagWidget"

void SConceptTagWidget::Construct(const FArguments& InArgs)
{
    ConceptDesignWidget = InArgs._ConceptDesignWidget;
    OnClearConceptTagFilter = InArgs._OnClearConceptTagFilter;
    OnConceptTagClickClearWidget = InArgs._OnConceptTagClickClearWidget;
    OnConceptTagDataReceived = InArgs._OnConceptTagDataReceived;
    TagButtonContainer = SNew(SVerticalBox);
    SelectedTagsContainer = SNew(SHorizontalBox);

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
                .Text(LOCTEXT("SelectedText", "Selected: "))
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
                .Text(LOCTEXT("ConditionTypeLabel", "Filter: "))
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
           .OnClicked(this, &SConceptTagWidget::ClearSelectedTags)  
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
                        .Text(LOCTEXT("ClearFilter", "Clear"))
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


void SConceptTagWidget::FetchTagList()
{
    if (!ConceptDesignWidget.IsValid())
    {
        return;
    }

    FString Ticket = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Ticket;
    FString ProjectNo = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetSelectedProject().projectNo;
    FString Uuid = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Uuid;

    // The HTTP request is processed in the background thread 在后台线程处理 HTTP 请求
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Ticket, ProjectNo, Uuid]()
    {
        UGetConceptDesignLibraryTagListApi* GetConceptDesignLibraryTagListApi = NewObject<UGetConceptDesignLibraryTagListApi>();
        if (GetConceptDesignLibraryTagListApi)
        {
            int32 TestType = 0;
            FOnGetConceptDesignLibraryTagListResponse OnGetConceptDesignLibraryTagListResponse;
            OnGetConceptDesignLibraryTagListResponse.BindLambda([this](UGetConceptDesignLibraryTagListResponseData* ResponseData)
            {
                // Return to the main thread to update the UI 返回主线程更新 UI
                AsyncTask(ENamedThreads::GameThread, [this, ResponseData]()
                {
                    if (ResponseData)
                    {
                        if (ResponseData->status == "success" && ResponseData->code == "200")
                        {
                            AddTagButtons(ResponseData->data);
                        }
                        else
                        {
                            // UE_LOG(LogTemp, Error, TEXT("Failed to fetch tag list: %s"), *ResponseData->message);
                        }
                    }
                });
            });

            GetConceptDesignLibraryTagListApi->SendConceptDesignLibraryTagListRequest(Ticket, ProjectNo, TestType, Uuid, OnGetConceptDesignLibraryTagListResponse);
        }
    });
}



void SConceptTagWidget::AddTagButtons(const TArray<FConceptDesignFileItemTagList>& TagInfoList)
{
    TagButtonContainer->ClearChildren();

    TSharedPtr<SHorizontalBox> CurrentRow = SNew(SHorizontalBox);
    int32 ButtonCount = 0;

    for (const FConceptDesignFileItemTagList& TagInfo : TagInfoList)
    {
        if (ButtonCount >= 20)
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
                .Text(FText::FromString(TagInfo.tagName))
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
            ]
            .OnClicked_Lambda([this, TagInfo]()
            {
                return OnTagButtonClicked(TagInfo.tagName, TagInfo.id);
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
    
    OnConceptTagDataReceived.ExecuteIfBound();
}

// void SConceptTagWidget::ConceptSetShouldCallAudioApi(bool bValue)
// {
//     bShouldCallAudioApi = bValue;
//
//     // UE_LOG(LogTemp, Warning, TEXT("bShouldCallAudioApi is now: %s"), bShouldCallAudioApi ? TEXT("true") : TEXT("false"));
// }


FReply SConceptTagWidget::OnTagButtonClicked(const FString& InTagName, const int64& InTagID)
{
    AddTagToSelected(InTagName);

    // UE_LOG(LogTemp, Warning, TEXT("已选择的标签为：%s"), *InTagName);

    FString TagIdString = LexToString(InTagID);
    
    UGetConceptDesignLibMenuApi* GetConceptDesignLibMenuApi = NewObject<UGetConceptDesignLibMenuApi>();
    if (GetConceptDesignLibMenuApi)
    {
        FOnGetConceptDesignLibMenuResponse OnGetConceptDesignLibMenuResponse;
        OnGetConceptDesignLibMenuResponse.BindLambda([this](FGetConceptDesignLibMenuData* ConceptDesignMenuData)
        {
            if (ConceptDesignMenuData && ConceptDesignMenuData->status == "Success" && ConceptDesignMenuData->code == "200")
            {
                FImageLoader::CancelAllImageRequests();
                ConceptDesignWidget->UpdateConceptDesignTagPageAssets(ConceptDesignMenuData->data.items);
                
            }
            else
            {
                // UE_LOG(LogTemp, Error, TEXT("API Response Error: %s"), ConceptDesignMenuData ? *ConceptDesignMenuData->message : TEXT("Invalid Response"));
            }
        });

        FString Uuid = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Uuid;
        FString Ticket = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Ticket;
        FString ProjectNo = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetSelectedProject().projectNo;
        int32 CurrentPage = 1;
        int32 MenuType = 0;
        int32 PageSize = 100;

        FString FolderId = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentConceptFolderID();
        
        FString paintingName = "";
        //int32 TagId = 0;
        //FString TagName = "";

        GetConceptDesignLibMenuApi->SendGetConceptDesignLibMenuRequest(Ticket, CurrentPage, FolderId, MenuType, PageSize, paintingName, ProjectNo, TagIdString, InTagName, Uuid, OnGetConceptDesignLibMenuResponse);
    }
    
    return FReply::Handled();
}



void SConceptTagWidget::AddTagToSelected(const FString& TagName)
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



FReply SConceptTagWidget::ClearSelectedTags()
{
    SelectedTags.Empty();
    SelectedTagsContainer->ClearChildren();
    FImageLoader::CancelAllImageRequests();
    // ConceptDesignWidget->ClearConceptContent();

    // Reset current level 重置当前层级
    OnClearConceptTagFilter.ExecuteIfBound();
    OnConceptTagClickClearWidget.ExecuteIfBound();

    // 打印日志以确认清空操作
    // UE_LOG(LogTemp, Warning, TEXT("已选择的标签已清空"));

    return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE