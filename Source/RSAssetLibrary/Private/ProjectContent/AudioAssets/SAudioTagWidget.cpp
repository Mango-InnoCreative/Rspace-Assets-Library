// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.


#include "ProjectContent/AudioAssets/SAudioTagWidget.h"

#include "RSAssetLibraryStyle.h"
#include "Async/Async.h"
#include "AudioLibrary/GetAudioAssetLibraryTagListApi.h"
#include "ProjectContent/Imageload/FImageLoader.h"
#include "Widgets/Layout/SSeparator.h"
#include "Subsystem/USMSubsystem.h"

#define LOCTEXT_NAMESPACE "SAudioTagWidget"

void SAudioTagWidget::Construct(const FArguments& InArgs)
{
    AudioAssetsWidget = InArgs._AudioAssetWidget;
    OnClearAudioTagFilterDelegate = InArgs._OnClearAudioTagFilter;
    OnAudioTagClickClearWidget = InArgs._OnAudioTagClickClearWidget;
    OnAudioTagDataReceived = InArgs._OnAudioTagDataReceived;
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

        // "Clear Filter" button "清空筛选" 按钮
        + SVerticalBox::Slot()
       .VAlign(VAlign_Bottom)  
       .HAlign(HAlign_Right)  
       .Padding(10)
       [
           SNew(SButton)
           .ButtonStyle(&ImportButtonStyle)
           .Cursor(EMouseCursor::Hand)
           .OnClicked(this, &SAudioTagWidget::ClearSelectedTags) 
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


void SAudioTagWidget::FetchTagList()
{
    if (!AudioAssetsWidget.IsValid())
    {
        return;
    }

    FString Ticket = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Ticket;
    FString ProjectNo = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetSelectedProject().projectNo;
    FString Uuid = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Uuid;
    int32 TestGroupStatus = 0;

    // The HTTP request is processed in the background thread 在后台线程处理 HTTP 请求
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Ticket, ProjectNo, Uuid, TestGroupStatus]()
    {
        UGetAudioAssetLibraryTagListApi* GetAudioAssetLibraryTagListApi = NewObject<UGetAudioAssetLibraryTagListApi>();
        if (GetAudioAssetLibraryTagListApi)
        {
            FOnGetAudioAssetLibraryTagListResponse OnGetAudioAssetLibraryTagListResponse;
            OnGetAudioAssetLibraryTagListResponse.BindLambda([this](FGetAudioAssetLibraryTagListResponseData ResponseData)
            {
                // Return to the main thread to update the UI 返回主线程更新 UI
                AsyncTask(ENamedThreads::GameThread, [this, ResponseData]()
                {
                    if (ResponseData.Status == "success" && ResponseData.Code == "200")
                    {
                        AddTagButtons(ResponseData.Data);
                    }
                    else
                    {
                        // UE_LOG(LogTemp, Error, TEXT("Failed to fetch tag list: %s"), *ResponseData.Message);
                    }
                });
            });

            GetAudioAssetLibraryTagListApi->SendGetAudioAssetLibraryTagListRequest(Ticket, Uuid, ProjectNo, TestGroupStatus, OnGetAudioAssetLibraryTagListResponse);
        }
    });
}



void SAudioTagWidget::AddTagButtons(const TArray<FAudioTagInfo>& TagInfoArray)
{
    TagButtonContainer->ClearChildren();

    TSharedPtr<SHorizontalBox> CurrentRow = SNew(SHorizontalBox);
    int32 ButtonCount = 0;

    for (const FAudioTagInfo& TagInfo : TagInfoArray)
    {
        // UE_LOG(LogTemp, Log, TEXT("Selectedtagname!!!!!!!!!!!!!!!!!!!!: %s"), *TagInfo.TagName);
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
                .Text(FText::FromString(TagInfo.TagName))
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
            ]
            .OnClicked_Lambda([this, TagInfo]()
            {
                OnAudioTagClickClearWidget.ExecuteIfBound();
                return OnTagButtonClicked(TagInfo.TagName, TagInfo.Id);
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

    OnAudioTagDataReceived.ExecuteIfBound();
}

FReply SAudioTagWidget::OnTagButtonClicked(const FString& InTagName, const int64& InTagID)
{
    AddTagToSelected(InTagName);

    // UE_LOG(LogTemp, Warning, TEXT("已选择的标签为：%s"), *InTagName);

    UGetAudioFileByConditionApi* AudioApi = NewObject<UGetAudioFileByConditionApi>();
    if (AudioApi)
    {
        FOnGetAudioFileByConditionResponse OnResponseDelegate;
        OnResponseDelegate.BindLambda([this](const FGetAudioFileByConditionResponse& ApiResponse)
        {
            if (ApiResponse.Status == "success" && ApiResponse.Code == "200")
            {
                FAudioFileResponseData AudioFileResponseData;
                AudioFileResponseData = ApiResponse.data;
                FImageLoader::CancelAllImageRequests();
                AudioAssetsWidget->UpdateTagPageAudioAssets(AudioFileResponseData.dataList);
            }
            else
            {
                // UE_LOG(LogTemp, Error, TEXT("API Response Error: %s"), *ApiResponse.Message);
            }
        });
        
        FString Uuid = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Uuid;
        FString Ticket = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().Ticket;
        FString ProjectNo = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetSelectedProject().projectNo;
        FString AudioChannel = TEXT("");
        FString AudioHarvestBits = TEXT("");
        FString AudioHarvestRate = TEXT("");
        int32 BpmBegin = *"";
        int32 BpmEnd = *"";
        int32 CurrentPage = 1;
        FString FileFormat = TEXT("");
        // FString GroupId = "";
        FString GroupId = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentAudioGroupID();
        int32 MenuType = 1;
        int32 PageSize = 1000;
        FString Search = TEXT("");
        FString Sort =  TEXT("DESC");
        FString SortType =  TEXT("1");
        //int64 TagId =  *"";
        
        AudioApi->SendGetAudioFileByConditionRequest(
            Ticket,
            Uuid,
            ProjectNo,
            GroupId,
            AudioChannel,
            AudioHarvestBits,
            AudioHarvestRate,
            BpmBegin,
            BpmEnd,
            CurrentPage,
            FileFormat,
            MenuType,
            PageSize,
            Search,
            Sort,
            SortType,
            InTagID,
            OnResponseDelegate
        );
    }

    return FReply::Handled();
}



void SAudioTagWidget::AddTagToSelected(const FString& TagName)
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



FReply SAudioTagWidget::ClearSelectedTags()
{
    SelectedTags.Empty();
    SelectedTagsContainer->ClearChildren();
    
    if (OnClearAudioTagFilterDelegate.IsBound())
    {
        OnClearAudioTagFilterDelegate.Execute(); 
    }

    OnAudioTagClickClearWidget.ExecuteIfBound();
    

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE