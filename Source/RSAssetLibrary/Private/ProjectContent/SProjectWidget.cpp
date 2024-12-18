// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectContent/SProjectWidget.h"
#include "RSAssetLibraryStyle.h"
#include "ModelLibrary/GetModelLibrary.h"
#include "AudioLibrary/GetAudioAssetLibraryFolderListApi.h"
#include "AudioLibrary/GetAudioAssetLibraryFolderListData.h"
#include "ConceptDesignLibrary/GetConceptDesignLibraryApi.h"
#include "VideoLibrary/GetVideoAssetLibraryListInfoApi.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SWindow.h"
#include "Widgets/Input/SComboButton.h"
#include "ProjectContent/ConceptDesign/ConceptDesignWidget.h"
#include "ProjectContent/AudioAssets/SAudioAssetsWidget.h"
#include "ProjectContent/VideoAssets/VideoAssetsWidget.h"
#include "ProjectContent/ModelAssets/ModelAssetsWidget.h"
#include "ProjectContent/ProjectList/SProjectlistWidget.h"
#include "Widgets/Layout/SScrollBox.h"
#include "ProjectContent/AssetDownloader/SAssetDownloadWidget.h"
#include "TimerManager.h"
#include "ConceptDesignLibrary/GetConceptDesignLibMenuApi.h"
#include "ProjectContent/AudioAssets/SAudioTagWidget.h"
#include "ProjectContent/ConceptDesign/SConceptTagWidget.h"
#include "ProjectContent/Imageload/FImageLoader.h"
#include "ProjectContent/ModelAssets/SModelTagWidget.h"
#include "ProjectList/FindProjectListApi.h"


#define LOCTEXT_NAMESPACE "ProjectWidget"


void SProjectWidget::Construct(const FArguments& InArgs)
{
    ButtonClick = EButtonClick::None;

    UserAndProjectInfo = InArgs._UserAndProjectInfo;
    OnLogoutDelegate = InArgs._OnLogout;
	
	// DownloadCompleteWidget = SNew(SDownloadCompleteWidget);
	LoadAndDisplayUserAvatar();

	// Initializes the animation sequence 初始化动画序列
	TagAnimationSequence = FCurveSequence();
	TagAnimationCurve = TagAnimationSequence.AddCurve(0.f, 0.3f, ECurveEaseFunction::QuadOut);

	// Initializes the animation sequence 初始化动画序列
	ProjectListAnimationSequence = FCurveSequence();
	ProjectListAnimationCurve = ProjectListAnimationSequence.AddCurve(0.f, 0.3f, ECurveEaseFunction::QuadOut);

	ChildSlot
	[
		SNew(SBox)
		.MinDesiredWidth(1620.0f)  
		.MinDesiredHeight(1080.0f) 
		[
			SNew(SHorizontalBox)

			// Options box on the left 左侧的选项框
			+ SHorizontalBox::Slot()
			.AutoWidth() 
			.Padding(5, 8, 5, 8)
			[
				SNew(SBox)
				.WidthOverride(310.0f)  
				[
					MakeOptionBar()  
				]
			]

			// Content and details box combination on the right 右侧的内容和详情框组合
			+ SHorizontalBox::Slot()
			.FillWidth(1.0) 
			[
				SNew(SVerticalBox)

				// Upper search box 上部的搜索框
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5, 8, 5, 5)
				[
					MakeSearchBar()  // Invoke the encapsulated search box method 调用封装的搜索框方法
				]

				// Lower content box and detail box 下部的内容框和详情框
				+ SVerticalBox::Slot()
				.FillHeight(1.0)
				.Padding(5)
				[
					SNew(SHorizontalBox)

					// Content box on the left 左边的内容框
					+ SHorizontalBox::Slot()
					.FillWidth(1.0)  
					[
						SAssignNew(ContentBar, SBox)
						.MinDesiredWidth(800.0f) 
						.MinDesiredHeight(300.0f)
						.Padding(0, 0, 0, 5)
						[
							SNew(SBorder)
							.BorderImage(new FSlateRoundedBoxBrush(FLinearColor(0.011612, 0.011612, 0.011612, 1.0f), 6.0f)) 
							.Padding(FMargin(1)) // Sets the width of the border lines 设置边框线条的宽度
							.Padding(0,0,5,0) // Sets the spacing within the border 设置边框内的间距
							[
								SNew(SBox)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
								SNew(SImage)
								.Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.NoAssets"))
								]
							]
						]
					]

					// Details box on the right 右边的详情框
					+ SHorizontalBox::Slot()
					.AutoWidth()  
					[
						SAssignNew(DetailsBar, SBox)
						.WidthOverride(400.0f)  
						.Padding(5, 0, 0, 5)
						[
							SNew(SBorder)
						   .Padding(5,0,0,0)
						   .BorderImage(new FSlateRoundedBoxBrush(FLinearColor(0.011612, 0.011612, 0.011612, 1.0f), 6.0f)) 
						   .Padding(FMargin(1))
							[
								SNew(SBox)
								.VAlign(VAlign_Center)
								.HAlign(HAlign_Center)
								[
									SNew(STextBlock)
									.Text(LOCTEXT("NoAssetSelected1", "未选择资产"))
									.Justification(ETextJustify::Center)
								]
							]
						]
					]
				]
			]
		]
	];



    InitializeConceptButtonStyle();
    InitializeAudioButtonStyle();
    InitializeVideoButtonStyle();
    InitializeModelButtonStyle();
    InitializeProjectListButtonStyle();
    InitializeAudioFolderButtonStyle();
    InitializeConceptFolderButtonStyle();
    InitializeVideoFolderButtonStyle();
    InitializeModelFolderButtonStyle();
	InitializeLogoutButtonStyle();
}


FTextBlockStyle SProjectWidget::InitializeFont()
{
	FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 12);

	FTextBlockStyle TitleFontStyle = FTextBlockStyle()
	.SetFont(TitleFont)
	.SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1)));
	
	return TitleFontStyle;
}


TSharedRef<SWidget> SProjectWidget::MakeOptionBar()
{
	FTextBlockStyle MakeFontStyle = InitializeFont();
	FSlateFontInfo MidFont = FCoreStyle::GetDefaultFontStyle("Regular", 12);

	ModelTreeContainer = SNew(SVerticalBox);
	AudioTreeContainer = SNew(SVerticalBox);
	ConceptDesignTreeContainer = SNew(SVerticalBox);
	VideoTreeContainer = SNew(SVerticalBox);


	// Top box for displaying the list of items (originally "Collapsed") 用于显示项目列表的顶部框（初始为Collapsed）
	SAssignNew(ProjectListContainer, SVerticalBox)
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(FMargin(0.0, 0.0, 0.0, 0.0))
	[
		SNew(SVerticalBox)
		.Visibility(EVisibility::Collapsed)
	];


	// The bottom box for displaying the item list button 用于显示项目列表按钮的底部框
	TSharedPtr<SVerticalBox> BottomButtonContainer = SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.AutoHeight()
	.VAlign(VAlign_Bottom)
	[
		SNew(SBox)
		.HeightOverride(80)
		.Padding(FMargin(24, 22, 24, 22))
		[
			SAssignNew(ProjectlistButton, SButton)
			.ButtonStyle(&ProjectListButtonStyle)
			.Cursor(EMouseCursor::Hand)
			.HAlign(HAlign_Left) 
			.VAlign(VAlign_Center)
			.OnClicked(this, &SProjectWidget::OnProjectButtonClicked)
			[
				SNew(SBox)
				.HeightOverride(40)
				.WidthOverride(200)
				.Padding(10,0,0,0)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(FMargin(0.0, 0.0, 0.0, 0.0))
					[
						SNew(SBox)
						.HeightOverride(22)
						.WidthOverride(22)
						[
							SNew(SImage)
							.Image(FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.xiangmu.Icon"))
						]
					]

					// Dynamic text binding 动态文本绑定
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					.Padding(FMargin(8.0, 0.0, 0.0, 0.0))
					[
						SNew(STextBlock)
						.Text_Lambda([this]() -> FText
						{
							// 如果 CurrentProjectName 为空，则显示本地化文本 "No project selected"
							if (CurrentProjectName == "")
							{
								return LOCTEXT("CurrentProjectName", "未选择项目");  // 本地化文本
							}
							else
							{
								return FText::FromString(CurrentProjectName);  // 显示项目名称
							}
						})
						.Font(MidFont)
					]
				]
			]
		]
	];


	return SNew(SBox).WidthOverride(320.0) .HeightOverride(900.0).MinDesiredHeight(900.0).MaxDesiredHeight(900.0)
	[
		SNew(SBox).WidthOverride(320.0).HeightOverride(900.0).MinDesiredHeight(900.0).MaxDesiredHeight(900.0)
		[
			SNew(SBorder)
			.BorderImage(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.OptionBorder"))
			.Padding(5)
			[
				
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					[
						 SNew(SVerticalBox)

						 // Conceptual design 概念设计
						 +SVerticalBox::Slot()
						 .AutoHeight()
						 .Padding(FMargin(0.0, 10.0, 0.0, 0.0))
						 [
							 SNew(SBox)
							 .HeightOverride(60)
							 .WidthOverride(200)
							 [
								 SNew(SButton)
								 .ButtonStyle(&ConceptButtonStyle)
								 .Cursor(EMouseCursor::Hand)
								 .OnClicked(this, &SProjectWidget::OnConceptDesignClicked)
								 .HAlign(HAlign_Left)  
								 .VAlign(VAlign_Center)
								 [
									SNew(SBox)
									.HeightOverride(60)
									.WidthOverride(250)
									.Padding(FMargin(43, 0, 0, 0))  
									[
										SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.AutoWidth()
										.VAlign(EVerticalAlignment::VAlign_Center)
										.Padding(FMargin(0, 0.0, 0.0, 0.0)) 
										[
											SNew(SBox)
											.HeightOverride(28)
											.WidthOverride(28)
										[
											SNew(SImage)
											.Image(FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.ConceptIcon.Icon"))
										]
									]
									
									+ SHorizontalBox::Slot()
									.VAlign(EVerticalAlignment::VAlign_Center)
									.HAlign(HAlign_Center)
									.Padding(FMargin(0.0, 1.0, 0.0, 0.0))
									[
									SNew(STextBlock)
									.Text(LOCTEXT("ConceptDesignButton", "概念设计"))
									.Justification(ETextJustify::Center)
									.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
									.ColorAndOpacity(FSlateColor(FLinearColor::White))
									]
									]
								 ]
							 ]
						 ]
						 
						 + SVerticalBox::Slot().AutoHeight().Padding(FMargin(18.0, 5.0, 0.0, 0.0))
						 [
							ConceptDesignTreeContainer.ToSharedRef()  
						 ]

			 			
						// Audio assets 音频资产
						+SVerticalBox::Slot()
						.AutoHeight()
						.Padding(FMargin(0.0, 10.0, 0.0, 0.0))
						[
							SNew(SBox)
							.HeightOverride(60)
							.WidthOverride(200)
							[
								SNew(SButton)
								.ButtonStyle(&AudioButtonStyle)
								.Cursor(EMouseCursor::Hand)
								.OnClicked(this, &SProjectWidget::OnAudioAssetsClicked)
								.HAlign(HAlign_Left) 
								.VAlign(VAlign_Center)
								[
									SNew(SBox)
									.HeightOverride(60)
									.WidthOverride(250)
								   .Padding(FMargin(43, 0, 0, 0)) 
								   [
										SNew(SHorizontalBox)
										
										+ SHorizontalBox::Slot()
										.AutoWidth()
										.VAlign(EVerticalAlignment::VAlign_Center)
										.Padding(FMargin(0, 0.0, 0.0, 0.0))  
										[
											SNew(SBox)
											.HeightOverride(28)
											.WidthOverride(28)
											[
												SNew(SImage)
												.Image(FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.AudioIcon.Icon")) 
											]
										]

										+ SHorizontalBox::Slot()
										.VAlign(EVerticalAlignment::VAlign_Center)
										.HAlign(HAlign_Center)
										.Padding(FMargin(0.0, 1.0, 0.0, 0.0))
										[
											SNew(STextBlock)
											.Text(LOCTEXT("AudioAssetsButton", "音频资产"))
											.Justification(ETextJustify::Center)
											.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
											.ColorAndOpacity(FSlateColor(FLinearColor::White))
										]
									]
								]
							]
						]
						
						+ SVerticalBox::Slot().AutoHeight().Padding(FMargin(18.0, 5.0, 0.0, 0.0))
						[
							AudioTreeContainer.ToSharedRef()  
						]

						// Video assets 视频资产
						+SVerticalBox::Slot()
						.AutoHeight()
						.Padding(FMargin(0.0, 10.0, 0.0, 0.0))
						[
							SNew(SBox)
							.HeightOverride(60)
							.WidthOverride(200)
							[
								SNew(SButton)
								.ButtonStyle(&VideoButtonStyle)
								.Cursor(EMouseCursor::Hand)
								.OnClicked(this, &SProjectWidget::OnVideoAssetsClicked)
								.HAlign(HAlign_Left)  
								.VAlign(VAlign_Center)
								[
									SNew(SBox)
									.HeightOverride(60)
									.WidthOverride(250)
								   .Padding(FMargin(43, 0, 0, 0)) 
								   [
																	
										SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.AutoWidth()
										.VAlign(VAlign_Center)
										.Padding(FMargin(0, 0.0, 0.0, 0.0))  
										[
											SNew(SBox)
											.HeightOverride(28)
											.WidthOverride(28)
											[
												SNew(SImage)
												.Image(FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.VideoIcon.Icon"))
											]
										]

										+ SHorizontalBox::Slot()
										.VAlign(EVerticalAlignment::VAlign_Center)
										.HAlign(HAlign_Center)
										.Padding(FMargin(0.0, 1.0, 0.0, 0.0))
										[
											SNew(STextBlock)
											.Text(LOCTEXT("VideoAssetsButton", "视频资产"))
											.Justification(ETextJustify::Center)
											.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
											.ColorAndOpacity(FSlateColor(FLinearColor::White))
										]
									]
								]
							]
						]
						
						+ SVerticalBox::Slot().AutoHeight().Padding(FMargin(18.0, 5.0, 0.0, 0.0))
						[
							VideoTreeContainer.ToSharedRef() 
						]
						
						// Model asset button 模型资产按钮
						+SVerticalBox::Slot()
						.AutoHeight()
						.Padding(FMargin(0.0, 10.0, 0.0, 0.0))
						[
							SNew(SBox)
							.HeightOverride(60)
							.WidthOverride(200)
							[
								SNew(SButton)
								.ButtonStyle(&ModelButtonStyle)
								.Cursor(EMouseCursor::Hand)
								.OnClicked(this, &SProjectWidget::OnModelAssetsClicked)
								.HAlign(HAlign_Left)  
								.VAlign(VAlign_Center)
								[
									SNew(SBox)
									.HeightOverride(60)
									.WidthOverride(250)
								   .Padding(FMargin(43, 0, 0, 0)) 
								   [
										SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.AutoWidth()
										.VAlign(VAlign_Center)
										.Padding(FMargin(0, 0.0, 0.0, 0.0))  
										[
											SNew(SBox)
											.HeightOverride(28)
											.WidthOverride(28)
												[
													SNew(SImage)
													.Image(FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.ModelIcon.Icon")) 
												]
										]
										
										+ SHorizontalBox::Slot()
										.VAlign(EVerticalAlignment::VAlign_Center)
										.HAlign(HAlign_Center)
										.Padding(FMargin(0.0, 1.0, 0.0, 0.0))
										[
											SNew(STextBlock)
											.Text(LOCTEXT("ModelAssetsButton", "模型资产"))
											.Justification(ETextJustify::Center)
											.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
											.ColorAndOpacity(FSlateColor(FLinearColor::White))
										]
									]
								]
							]
						]
						
						+ SVerticalBox::Slot().AutoHeight().Padding(FMargin(18.0, 5.0, 0.0, 0.0))
						[
							ModelTreeContainer.ToSharedRef()  
						]
			 		]	
				]
				
				+ SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Bottom) // Place the item list button at the bottom 将项目列表按钮放在底部
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							ProjectListContainer.ToSharedRef()  
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(FMargin(24.0f, 5.0f, 24.0f, 0.0f)) 
						[
						   SNew(SBox)
						   [
							   SNew(SImage)
							   .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ProjectLine")) 
						   ]
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							BottomButtonContainer.ToSharedRef() 
						]
					]
				]
			]
		]
    ];	
}

// Method to load and display the user's avatar 加载并显示用户头像的方法
void SProjectWidget::LoadAndDisplayUserAvatar()
{
	// Gets the URL of the user's avatar 获取用户头像的URL
	FString UserAvatarUrl = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo().UserAvatarUrl + TEXT("?imageMogr2/iradius/200");

	if (UserAvatarUrl.IsEmpty())
	{
		return;
	}

	// Defines the callback delegate after the image is loaded 定义加载图片后的回调委托
	FOnProjectImageReady OnImageReadyDelegate = FOnProjectImageReady::CreateLambda([this](const TArray<uint8>& ImageData)
	{
		UTexture2D* UserAvatarTexture = FImageLoader::CreateTextureFromBytes(ImageData);
		UserAvatarTexture->AddToRoot();

		if (UserAvatarTexture)
		{
			UserAvatarBrush = MakeShared<FSlateBrush>();
			UserAvatarBrush->SetResourceObject(UserAvatarTexture);
			UserAvatarBrush->ImageSize = FVector2D(UserAvatarTexture->GetSizeX(), UserAvatarTexture->GetSizeY());
			
			if (UserAvatarImageWidget.IsValid())
			{
				UserAvatarImageWidget->SetImage(UserAvatarBrush.Get());
			}
		}
	});
	
	FImageLoader::LoadImageFromUrl(UserAvatarUrl, OnImageReadyDelegate);
}

bool SProjectWidget::CheckProjectIsSelected()
{
    if ( GEditor->GetEditorSubsystem<UUSMSubsystem>() && GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetSelectedProject().projectNo == "" )
    {
        if (!ProjectSelectedNotificationWindow.IsValid())
        {
            // 创建新的窗口
            ProjectSelectedNotificationWindow = SNew(SWindow)
        		.Title(LOCTEXT("TipTitle", "提示"))
                .ClientSize(FVector2D(200.f, 100.f))		
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
                    .Text(LOCTEXT("SelectProjectWarning", "请先选择一个项目！"))
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
                    .OnClicked_Lambda([this]() -> FReply
                    {
                        // Close the prompt window 关闭提示窗口
                        if (ProjectSelectedNotificationWindow.IsValid())
                        {
                            FSlateApplication::Get().RequestDestroyWindow(ProjectSelectedNotificationWindow.ToSharedRef());
                            ProjectSelectedNotificationWindow.Reset();
                        }
                        return FReply::Handled();
                    })
                ]
            ];
        	
            ProjectSelectedNotificationWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>&)
            {
                ProjectSelectedNotificationWindow.Reset();
            }));

            // Display window 显示窗口
            FSlateApplication::Get().AddWindow(ProjectSelectedNotificationWindow.ToSharedRef());
        }
        else
        {
            ProjectSelectedNotificationWindow->BringToFront();
        }

        return false; 
    }

    return true; 
}


TSharedRef<SWidget> SProjectWidget::MakeSearchBar()
{
	TransparentBrush.TintColor = FSlateColor(FLinearColor::Transparent);

	TagButtonStyle.SetNormal(TransparentBrush);
	TagButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.TagSearchButton"));
	TagButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.TagSearchButton"));
	
    return SNew(SVerticalBox)
    + SVerticalBox::Slot()
    [
        SNew(SHorizontalBox)
        // Search box 搜索框
        + SHorizontalBox::Slot()
        .Padding(0, 0, 0, 0)
        [
	        SNew(SBorder)
			.BorderImage(new FSlateRoundedBoxBrush(FLinearColor(0.011612, 0.011612, 0.011612, 1.0f), 6.0f)) 
			.Padding(FMargin(1))
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0)
                    .Padding(5,5,10,5)
                    [
                        SNew(SEditableTextBox)
                        .HintText(LOCTEXT("SearchHint", "按回车键搜索，支持模糊查询"))
                        .OnTextCommitted(this, &SProjectWidget::OnSearchTextCommitted)
                    ]

                    // Label button 标签按钮
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(5, 5, 10, 5)
                    [
						SNew(SBox)
						.HeightOverride(30)
						.WidthOverride(30)
						[
							SAssignNew(MainTagButton, SButton)
	                        .Cursor(EMouseCursor::Hand)
	                        .ButtonStyle(&TagButtonStyle)
	                        .ContentPadding(0)
	                        .OnClicked(this, &SProjectWidget::OnTagButtonClicked)
	                        [
	                            SNew(SBox)
	                            .HeightOverride(27)
	                            .WidthOverride(27)
	                            .Padding(3, 3, 3, 3)
	                            [
	                                SNew(SImage)
	                                .Image(FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.biaoqian.Icon"))
	                            ]
	                        ]
	                    ]
                    ]

                    // Download button 下载按钮
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(5, 5, 10, 5)
                    [
	                    SNew(SBox)
						.HeightOverride(30)
						.WidthOverride(30)
						[
	                        SNew(SButton)
	                        .Cursor(EMouseCursor::Hand)
	                        .ButtonStyle(&TagButtonStyle)
	                        .ContentPadding(0)
	                        .OnClicked(this, &SProjectWidget::OnDownloadButtonClicked)
	                        [
	                            SNew(SBox)
	                            .HeightOverride(27)
	                            .WidthOverride(27)
	                            .Padding(3, 3, 3, 3)
	                            [
	                                SNew(SImage)
	                                .Image(FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.xiazai.Icon"))
	                            ]
	                        ]
	                    ]
                    ]

                    // The user information button displays the user profile picture 用户信息按钮，显示用户头像
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(10,5,20,5)
                    [
                        SAssignNew(UserInfoButton,SButton)
                        .Cursor(EMouseCursor::Hand)
                        .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder"))
                        .ContentPadding(0)
                        .OnClicked(this, &SProjectWidget::OnUserInfoButtonClicked)
                        [
                            SNew(SBox)
                            .HeightOverride(27)
                            .WidthOverride(27)
                            .Padding(2, 4, 2, 4)
                            [
                                SAssignNew(UserAvatarImageWidget, SImage)
                                .Image(UserAvatarBrush.IsValid() ? UserAvatarBrush.Get() : nullptr)
                            ]
                        ]
                    ]
                ]

                // New pull-down drawing window 新增的下拉动画窗口
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0, 5, 0, 0)
                [
                    SAssignNew(TagContainer, SBox)
                    .WidthOverride(400)
                    .HeightOverride_Lambda([this]() -> float
                    {
                        return 270.f * TagAnimationCurve.GetLerp();
                    })
                    .Visibility(EVisibility::Collapsed)
                    [
                        // The initial content is empty, and the TagWidget can be dynamically inserted based on different objects 初始内容为空，可以根据不同对象动态插入 TagWidget
                        SNullWidget::NullWidget
                    ]
                ]
            ]
        ]
    ];
}


void SProjectWidget::UpdateDetailsBar(TSharedRef<SWidget> InWidget)
{
	// Update the contents of the DetailsBox with the InWidget passed in 使用传入的 InWidget 更新 DetailsBox 的内容
	DetailsBar->SetContent(InWidget);
}

FReply SProjectWidget::OnConceptDesignClicked()
{
	// Call CheckProjectIsSelected and return if no item is selected 调用 CheckProjectIsSelected，若未选择项目则直接返回
	if (!CheckProjectIsSelected())
	{
		return FReply::Handled();
	}

	FImageLoader::CancelAllImageRequests();
	ClearAllCachedTextures();
	ResetAllButtonStyles(); // Start by resetting all button styles 首先重置所有按钮样式
	ResetSelectedTag();
	ResetToggleTagContainer();
	ButtonClick = EButtonClick::ConceptDesign;
	GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentConceptFolderID("");
	
	bIsConceptFirstPage = true;

	SetUserAndProjectParams();

	// Fold processing logic 折叠处理逻辑
	if (ExpandedStateMap.Contains(EButtonClick::ConceptDesign) && ExpandedStateMap[EButtonClick::ConceptDesign])
	{
		ExpandedStateMap[EButtonClick::ConceptDesign] = false;
		ConceptDesignTreeContainer->ClearChildren();
		ResetExpandedState(EButtonClick::ConceptDesign);

		ResetSlateWidgets();
		InitializeConceptButtonStyle();

		if (ConceptDesignWidget.IsValid())
		{
			ConceptDesignWidget->ClearConceptContent();
		}
	}
	// Expansion processing logic 展开处理逻辑
	else
	{
		ExpandedStateMap.Add(EButtonClick::ConceptDesign, true);

		// If the ConceptDesignWidget is not initialized, it is created 如果 ConceptDesignWidget 未初始化，则创建它
		if (!ConceptDesignWidget.IsValid())
		{
			ConceptDesignWidget = SNew(SConceptDesignWidget)
				.OnConceptDesignAssetClicked(FOnConceptDesignAssetClicked::CreateSP(this, &SProjectWidget::HandleConceptDesignAssetClicked))
				.OnConceptDownload(FOnSelectedConceptDownloadClicked::CreateSP(this, &SProjectWidget::HandleConceptAssetDownloadClicked))
				.OnConceptNothingToShow(FOnConceptNothingToShow::CreateLambda([this]()
				{
					ResetSlateWidgets();
				}))
				.OnConceptRefresh(FOnConceptRefresh::CreateLambda([this]()
				{
					UpdateRightContentBox(ConceptDesignWidget.ToSharedRef());
				}));

			ShowAllConceptFiles();
		}

		ShowAllConceptFiles();

		// equest concept design resources 请求概念设计资源
		RequestConceptDesignLibrary(0, ConceptDesignTreeContainer); 
		UpdateRightContentBox(ConceptDesignWidget.ToSharedRef());
		
		ConceptButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Pressed"));
		ConceptButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.ButtonPressedHovered"));
	}

	return FReply::Handled();
}


FReply SProjectWidget::OnAudioAssetsClicked()
{
	// Call CheckProjectIsSelected and return if no item is selected 调用 CheckProjectIsSelected，若未选择项目则直接返回
	if (!CheckProjectIsSelected())
	{
		return FReply::Handled();
	}
	
	FImageLoader::CancelAllImageRequests();
	ResetAllButtonStyles();
	ResetSelectedTag();
	ResetToggleTagContainer();
	ButtonClick = EButtonClick::AudioAssets;
	GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentAudioGroupID("");

	bIsAudioFirstPage = true;
	SetUserAndProjectParams();

	// Expand and collapse processing logic 展开和折叠处理逻辑
	if (ExpandedStateMap.Contains(EButtonClick::AudioAssets) && ExpandedStateMap[EButtonClick::AudioAssets])
	{
		ExpandedStateMap[EButtonClick::AudioAssets] = false;
		AudioTreeContainer->ClearChildren();
		ResetExpandedState(EButtonClick::AudioAssets);

		ResetSlateWidgets();
		InitializeAudioButtonStyle();
	}
	else
	{
		ExpandedStateMap.Add(EButtonClick::AudioAssets, true);

		if (!AudioAssetsWidget.IsValid())
		{
			AudioAssetsWidget = SNew(SAudioAssetsWidget)
				.OnAudioAssetClicked(FOnAudioAssetClicked::CreateSP(this, &SProjectWidget::HandleAudioAssetClicked))  // 绑定委托
				.OnAudioDownload(FOnSelectedAudioDownloadClicked::CreateSP(this, &SProjectWidget::HandleModelAssetDownloadClicked))
				.OnAudioNothingToShow(FOnAudioNothingToShow::CreateLambda([this]()
				{
					ResetSlateWidgets();
				}))
				.OnAudioRefresh(FOnAudioRefresh::CreateLambda([this]()
				{
					UpdateRightContentBox(AudioAssetsWidget.ToSharedRef());
				}));

			ShowAllAudioFiles();
		}

		AudioAssetsWidget->ClearAudioContent();
		ShowAllAudioFiles();
		RequestAudioAssetLibrary(0, AudioTreeContainer); 
		UpdateRightContentBox(AudioAssetsWidget.ToSharedRef());
		
		AudioButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Pressed"));
		AudioButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.ButtonPressedHovered"));
	}
	return FReply::Handled();
}

// Video asset button click event processing 视频资产按钮点击事件处理
FReply SProjectWidget::OnVideoAssetsClicked()
{
	// Call CheckProjectIsSelected and return if no item is selected 调用 CheckProjectIsSelected，若未选择项目则直接返回
	if (!CheckProjectIsSelected())
	{
		return FReply::Handled();
	}
	
	FImageLoader::CancelAllImageRequests();
	ClearAllCachedTextures();
	ResetAllButtonStyles(); 
	ResetSelectedTag();
	ResetToggleTagContainer();
	ButtonClick = EButtonClick::VideoAssets;

	SetUserAndProjectParams();

	// The wrapper method is called to handle the expansion and collapse of the total directory 调用封装方法处理总目录的展开和折叠
	HandleVideoRootDirectory(EButtonClick::VideoAssets);

	return FReply::Handled();
}

void SProjectWidget::HandleVideoRootDirectory(EButtonClick ButtonType)
{
	if (ExpandedStateMap.Contains(ButtonType) && ExpandedStateMap[ButtonType])
	{
		// If expanded, empty the subitems and collapse 如果已经展开，则清空子项并折叠
		ExpandedStateMap[ButtonType] = false;
		VideoTreeContainer->ClearChildren();
		ResetExpandedState(ButtonType);
		ResetSlateWidgets();
		InitializeVideoButtonStyle();
		
		if (VideoAssetsWidget)
		{
			VideoAssetsWidget->ClearVideoContent();
		}
	}
	else
	{
		ExpandedStateMap.Add(ButtonType, true);

		if (!VideoAssetsWidget.IsValid())
		{
			VideoAssetsWidget = SNew(SVideoAssetsWidget)
				.OnVideoAssetClicked(FOnVideoAssetClicked::CreateSP(this, &SProjectWidget::HandleVideoAssetClicked))
				.OnVideoDownload(FOnSelectedVideoDownloadClicked::CreateSP(this, &SProjectWidget::HandleVideoAssetDownloadClicked))
				.OnVideoNothingToShow(FOnVideoNothingToShow::CreateLambda([this]
				{
					ResetSlateWidgets();
				}));
			
			VideoAssetsWidget->OnVedioUpdateDetailsBar.BindSP(this, &SProjectWidget::UpdateDetailsBar);
		}
		
		GenerateVideoAssetTree(0, VideoTreeContainer, TEXT("-1"), 24);

		VideoButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Pressed"));
		VideoButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.ButtonPressedHovered"));
	}
}


void SProjectWidget::GenerateVideoAssetTree(int32 CurrentFileId, TSharedPtr<SVerticalBox> ParentBox, FString CurrentParentId, int32 MaxLength)
{
    ResetDetailBar();
	CurrentActiveWidget = EActiveWidget::VideoAssets;

    // If it is the root directory, clear the empty subdirectory status data 如果是根目录，清空子目录状态数据
    if (CurrentFileId == 0)
    {
        VideoChildExpandedStateSet.Empty();
    }
	
    UGetVideoAssetLibraryListInfoApi* GetVideoAssetLibraryListInfoApi = NewObject<UGetVideoAssetLibraryListInfoApi>();
    if (GetVideoAssetLibraryListInfoApi)
    {
        FOnGetVideoAssetLibraryListInfoResponse OnGetVideoAssetLibraryListInfoResponse;
        OnGetVideoAssetLibraryListInfoResponse.BindLambda([this, ParentBox, CurrentFileId, CurrentParentId, MaxLength](FGetVideoAssetLibraryListInfoData* VideoLibraryData)
        {
            if (VideoLibraryData)
            {
                // Update the content of the current level, either collapsed or expanded 更新当前层级的内容，无论折叠还是展开
                TArray<FVideoAssetInfo> VideoAssetsData = VideoLibraryData->data;
                UpdateVideoAssetsWidget(VideoAssetsData);

                // Check the expansion state of the current level 检查当前层级的展开状态
                if (VideoChildExpandedStateSet.Contains(CurrentFileId))
                {
                    // If expanded, clear the subitems and collapse, while cleaning up all the sublevel states of the current level 如果已展开，清空子项并折叠，同时清理当前层级的所有子层级状态
                    ParentBox->ClearChildren();

                    // Gets all the sublevels that need to be cleaned 获取所有需要清理的子层级
                    TArray<int32> KeysToReset;
                    for (const int32& Key : VideoChildExpandedStateSet)
                    {
                        // Assuming that the ID of the sublevel is greater than the current level ID, you can adjust this logic as required 假设子层级的 ID 都大于当前层级 ID，可以根据需求调整此逻辑
                        if (Key != CurrentFileId && Key > CurrentFileId)
                        {
                            KeysToReset.Add(Key);
                        }
                    }
                    // Delete all keys that need to be reset 删除所有需要重置的键
                    for (int32 Key : KeysToReset)
                    {
                        VideoChildExpandedStateSet.Remove(Key);
                    }

                    VideoChildExpandedStateSet.Remove(CurrentFileId); // Mark as folded 标记为折叠
                    return;
                }

                // Mark the current level as expanded 标记当前层级为展开
                if (CurrentFileId != 0)
                {
                    VideoChildExpandedStateSet.Add(CurrentFileId);
                }

                // Clear the current contents and generate a new subdirectory 清空当前内容并生成新的子目录
                ParentBox->ClearChildren();

                // Update the left asset tree 更新左侧资产树
                for (const FVideoAssetInfo& VideoFileItem : VideoLibraryData->data)
                {
                    if (VideoFileItem.fileType != 0) continue; 

                    TSharedPtr<FButtonStyle> FolderButtonStyle = MakeShareable(new FButtonStyle(ModelFolderButtonStyle));
                    TSharedPtr<SVerticalBox> ChildBox = SNew(SVerticalBox);

                    ParentBox->AddSlot()
                    .AutoHeight()
                    .Padding(20, 0, 0, 0)
                    [
                        SNew(SButton)
                        .Cursor(EMouseCursor::Hand)
                        .ButtonStyle(FolderButtonStyle.Get())
                        .HAlign(HAlign_Left) 
                        .OnClicked_Lambda([this, VideoFileItem, ChildBox, FolderButtonStyle, MaxLength]() -> FReply
                        {
                        	FImageLoader::CancelAllImageRequests();
                        	ClearAllCachedTextures();
                        	// ResetSlateWidgets();
                        	ResetToggleTagContainer();
                            if (SelectedButtonStyle.IsValid())
                            {
                                ResetSelectedTag();
                            }

                            FolderButtonStyle->SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
                            FolderButtonStyle->SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
                        	
                            SelectedButtonStyle = FolderButtonStyle;
                            TagClick = ETagClick::VideoTag;

                        	int32 NewMaxLength = FMath::Max(MaxLength - 4, 6);

                            // Recursive calls generate sublevels 递归调用生成子层级
                            GenerateVideoAssetTree(VideoFileItem.id, ChildBox, VideoFileItem.fileNo, NewMaxLength);

                            return FReply::Handled();
                        })
                        [
                            SNew(SBox)
                            .HeightOverride(40)
                            .WidthOverride(200)
                            .Padding(FMargin(36, 0, 0, 0)) 
                            [
                                SNew(SHorizontalBox)
                                
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                .Padding(FMargin(5, 0, 10, 0))
                                [
                                    SNew(SImage)
                                    .Image(FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.SmallFolder.Icon"))
                                ]
                                
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(EVerticalAlignment::VAlign_Center)
                                .HAlign(HAlign_Center)
                                [
                                    SNew(STextBlock)
                                    .Text(FText::FromString(TruncateText(VideoFileItem.fileName, MaxLength)))
                                ]
                            ]
                        ]
                    ];

                    ParentBox->AddSlot()
                    .AutoHeight()
                    .Padding(20, 0, 0, 0)
                    [
                        ChildBox.ToSharedRef()
                    ];
                }

            	GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentVideoFolderItems(VideoAssetsData);
				GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentVideoParentID(CurrentParentId);
            }
        });

        GetVideoAssetLibraryListInfoApi->SendGetVideoAssetLibraryListInfoRequest(Ticket, ProjectNo, CurrentParentId, FileName, OnGetVideoAssetLibraryListInfoResponse);
    }
}


// Model asset button click event handling 模型资产按钮点击事件处理
FReply SProjectWidget::OnModelAssetsClicked()
{
	// Call CheckProjectIsSelected and return if no item is selected 调用 CheckProjectIsSelected，若未选择项目则直接返回
	if (!CheckProjectIsSelected())
	{
		return FReply::Handled();
	}
	
	FImageLoader::CancelAllImageRequests();
	ClearAllCachedTextures();
	ResetAllButtonStyles();
	ResetSelectedTag();
	ResetToggleTagContainer();
	GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentModelRootID(-1);
	CurrentActiveWidget = EActiveWidget::ModelAssets;
	ButtonClick = EButtonClick::ModelAssets;

	SetUserAndProjectParams();

	// The wrapper method is called to handle the expansion and collapse of the total directory 调用封装方法处理总目录的展开和折叠
	HandleModelRootDirectory(EButtonClick::ModelAssets);

	return FReply::Handled();
}

void SProjectWidget::HandleModelRootDirectory(EButtonClick ButtonType)
{
	if (ExpandedStateMap.Contains(ButtonType) && ExpandedStateMap[ButtonType])
	{
		// If expanded, empty the subitems and collapse 如果已经展开，则清空子项并折叠
		ExpandedStateMap[ButtonType] = false;
		ModelTreeContainer->ClearChildren();
		ResetExpandedState(ButtonType);
		ResetSlateWidgets();
		InitializeModelButtonStyle();
		
		if (ModelAssetsWidget.IsValid())
		{
			ModelAssetsWidget->ClearModelContent();
		}
	}
	else
	{
		ExpandedStateMap.Add(ButtonType, true);

		if (!ModelAssetsWidget.IsValid())
		{
			ModelAssetsWidget = SNew(SModelAssetsWidget)
				.OnModelAssetClicked(FOnModelAssetClicked::CreateSP(this, &SProjectWidget::HandleModelAssetClicked))
				.OnSelectedModelDownloadClicked(FOnSelectedModelDownloadClicked::CreateSP(this, &SProjectWidget::HandleModelAssetDownloadClicked))
				.OnModelNothingToShow(FOnModelNothingToShow::CreateLambda([this]()
				{
					ResetSlateWidgets();
				}))
				.OnModelRefresh(FOnModelRefresh::CreateLambda([this]()
				{
					UpdateRightContentBox(ModelAssetsWidget.ToSharedRef());
				}));

			
			ModelAssetsWidget->OnModelUpdateDetailsBar.BindSP(this, &SProjectWidget::UpdateDetailsBar);
		}
		
		// Invoke the method that generates the subasset tree 调用生成子资产树的方法
		GenerateModelAssetTree(0, ModelTreeContainer, 24);

		ModelButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Pressed"));
		ModelButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.ButtonPressedHovered"));
	}
}


void SProjectWidget::GenerateModelAssetTree(int32 CurrentFileId, TSharedPtr<SVerticalBox> ParentBox, int32 MaxLength)
{
    ResetDetailBar();

    // If it is the root directory, clear the empty subdirectory status data 如果是根目录，清空子目录状态数据
    if (CurrentFileId == 0)
    {
        ModelChildExpandedStateMap.Empty(); 
    }
	
    GetModelLibraryApi = NewObject<UGetModelLibrary>();
    if (GetModelLibraryApi)
    {
        FOnGetModelLibraryResponse OnGetModelLibraryResponseDelegate;
        OnGetModelLibraryResponseDelegate.BindLambda([this, ParentBox, CurrentFileId, MaxLength](UGetModelLibraryResponseData* ModelLibraryData)
        {
            if (ModelLibraryData)
            {
                // Update the content of the current level, either collapsed or expanded 更新当前层级的内容，无论折叠还是展开
                TArray<FModelFileItem> ModelAssetsData = ModelLibraryData->data;
                UpdateModelAssetsWidget(ModelAssetsData);

                // Check the expansion state of the current level 检查当前层级的展开状态
                if (ModelChildExpandedStateMap.Contains(CurrentFileId) && ModelChildExpandedStateMap[CurrentFileId])
                {
                    // If expanded, clear the subitems and collapse, while cleaning up all the sublevel states of the current level 如果已展开，清空子项并折叠，同时清理当前层级的所有子层级状态
                    ParentBox->ClearChildren();
                	
                	// Gets all the sublevels that need to be cleaned 获取所有需要清理的子层级
					TArray<int32> KeysToReset;
					for (const auto& Entry : ModelChildExpandedStateMap)
					{
						// Assuming that the ID of the sublevel is greater than the current level ID, you can adjust this logic as required 假设子层级的 ID 都大于当前层级 ID，可以根据需求调整此逻辑
						if (Entry.Key != CurrentFileId && Entry.Key > CurrentFileId)
						{
							KeysToReset.Add(Entry.Key);
						}
					}
					// Delete all keys that need to be reset 删除所有需要重置的键
					for (int32 Key : KeysToReset)
					{
						ModelChildExpandedStateMap.Remove(Key);
					}
                	
                    ModelChildExpandedStateMap[CurrentFileId] = false; // Mark as folded 标记为折叠
                    return;
                }
            	
                if (CurrentFileId != 0)
                {
                    ModelChildExpandedStateMap.Add(CurrentFileId, true);
                }
            	
                ParentBox->ClearChildren();

                // Update the left asset tree 更新左侧资产树
                for (const FModelFileItem& FileItem : ModelLibraryData->data)
                {
                    if (FileItem.fileType != 1) continue; 

                    TSharedPtr<FButtonStyle> FolderButtonStyle = MakeShareable(new FButtonStyle(ModelFolderButtonStyle));
                    TSharedPtr<SVerticalBox> ChildBox = SNew(SVerticalBox);

                    ParentBox->AddSlot()
                    .AutoHeight()
                    .Padding(20, 0, 0, 0)
                    [
                        SNew(SButton)
                        .Cursor(EMouseCursor::Hand)
                        .ButtonStyle(FolderButtonStyle.Get())
                        .HAlign(HAlign_Left) 
                        .OnClicked_Lambda([this, FileItem, ChildBox, FolderButtonStyle, MaxLength]() -> FReply
                        {
                        	FImageLoader::CancelAllImageRequests();
                        	ClearAllCachedTextures();
                        	ResetToggleTagContainer();
                        	CurrentActiveWidget = EActiveWidget::ModelAssets;
                            if (SelectedButtonStyle.IsValid())
                            {
                                ResetSelectedTag();
                            }

                            FolderButtonStyle->SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
                            FolderButtonStyle->SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
                        	
                            SelectedButtonStyle = FolderButtonStyle;
                            TagClick = ETagClick::ModelTag;

                        	int32 NewMaxLength = FMath::Max(MaxLength - 4, 6);
                        	
                            GenerateModelAssetTree(FileItem.id, ChildBox, NewMaxLength);

                            GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentModelRootID(FileItem.id);
                            return FReply::Handled();
                        })
                        [
                            SNew(SBox)
                            .HeightOverride(40)
                            .WidthOverride(200)
                            .Padding(FMargin(36, 0, 0, 0)) 
                            [
                                SNew(SHorizontalBox)
                                
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                .Padding(FMargin(5, 0, 10, 0)) 
                                [
                                    SNew(SImage)
                                    .Image(FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.SmallFolder.Icon")) 
                                ]
                                
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(EVerticalAlignment::VAlign_Center)
                                .HAlign(HAlign_Center)
                                .Padding(0, 1, 0, 0) 
                                [
                                    SNew(STextBlock)
                                    .Text(FText::FromString(TruncateText(FileItem.fileName, MaxLength)))
                                ]
                            ]
                        ]
                    ];

                    ParentBox->AddSlot()
                    .AutoHeight()
                    .Padding(20, 0, 0, 0)
                    [
                        ChildBox.ToSharedRef()
                    ];
                }
            }
        });

        int64 TagId = *""; 
        GetModelLibraryApi->SendGetModelLibraryRequest(Ticket, Uuid, CurrentFileId, ProjectNo, FileName, TagId, TagName, OnGetModelLibraryResponseDelegate);
    }
}



void SProjectWidget::ShowAllAudioFiles()
{
	UGetAudioFileByConditionApi* AudioApi = NewObject<UGetAudioFileByConditionApi>();
	if (!AudioApi)
	{
		// UE_LOG(LogTemp, Error, TEXT("Failed to create UGetAudioFileByConditionApi instance."));
		return;
	}
	
	FOnGetAudioFileByConditionResponse OnResponseDelegate;
	OnResponseDelegate.BindLambda([this](const FGetAudioFileByConditionResponse& ApiResponse)
	{
		if (ApiResponse.Status == "Success" && ApiResponse.Code == "200")
		{
			FAudioFileResponseData AudioFileResponseData;
			// Append the audio data returned by the API to the AudioFileList 将 API 返回的音频数据追加到 AudioFileList
			AudioFileResponseData = ApiResponse.data;
			AudioAssetsWidget->UpdateTagPageAudioAssets(AudioFileResponseData.dataList);
			GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentFirstPageAudioFolderItems(AudioFileResponseData.dataList);
		}
		else
		{
			// UE_LOG(LogTemp, Error, TEXT("API Response Error: %s"), *ApiResponse.Message);
			ResetSlateWidgets();
		}
	});
	
	FString AudioChannel = TEXT("");
	FString AudioHarvestBits = TEXT("");
	FString AudioHarvestRate = TEXT("");
	int32 BpmBegin = *"";
	int32 BpmEnd = *"";
	int32 CurrentPage = 1;
	FString FileFormat = TEXT("");
	FString GroupId = "";
	int32 MenuType = 1;
	int32 PageSize = 1000;
	FString Search = TEXT("");
	FString Sort =  TEXT("DESC");
	FString SortType =  TEXT("1");
	int64 TagId =  *"";

	SetUserAndProjectParams();
	
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
		TagId,
		OnResponseDelegate
	);
}

void SProjectWidget::ShowAllConceptFiles()
{
	UGetConceptDesignLibMenuApi* GetConceptDesignLibMenuApi = NewObject<UGetConceptDesignLibMenuApi>();
	if (!GetConceptDesignLibMenuApi)
	{
		// UE_LOG(LogTemp, Error, TEXT("Failed to create UGetAudioFileByConditionApi instance."));
		return;
	}
	
	FOnGetConceptDesignLibMenuResponse OnGetConceptDesignLibMenuResponse;
	OnGetConceptDesignLibMenuResponse.BindLambda([this](FGetConceptDesignLibMenuData* ConceptDesignMenuData)
	{
		if (ConceptDesignMenuData && ConceptDesignMenuData->status == "Success" && ConceptDesignMenuData->code == "200")
		{
			ConceptDesignWidget->UpdateConceptDesignTagPageAssets(ConceptDesignMenuData->data.items);
			GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentFirstPageConceptItems(ConceptDesignMenuData->data.items);
		}
		else
		{
			// UE_LOG(LogTemp, Error, TEXT("API Response Error: %s"), ConceptDesignMenuData ? *ConceptDesignMenuData->message : TEXT("Invalid Response"));
			ResetSlateWidgets();
		}
	});

	
	SetUserAndProjectParams();

	int32 CurrentPage = 1;
	int32 MenuType = 0;
	int32 PageSize = 100;
	FString FolderId = "";
	FString paintingName = "";
	FString TagId = "";
	FString ThisTagName = "";
	
	// UE_LOG(LogTemp, Warning, TEXT("CurrentPage: %d"), CurrentPage);
	// UE_LOG(LogTemp, Warning, TEXT("MenuType: %d"), MenuType);
	// UE_LOG(LogTemp, Warning, TEXT("PageSize: %d"), PageSize);
	// UE_LOG(LogTemp, Warning, TEXT("FolderId: %s"), *FolderId);  // 如果 FolderId 为无效值，可以进一步调试
	// UE_LOG(LogTemp, Warning, TEXT("paintingName: %s"), *paintingName); // 由于 paintingName 是 FString，需使用 * 进行解引用
	// UE_LOG(LogTemp, Warning, TEXT("ProjectNo: %s"), *ProjectNo); // 由于 paintingName 是 FString，需使用 * 进行解引用
	// UE_LOG(LogTemp, Warning, TEXT("TagId: %s"), *TagId);  // TagId 也是 int32
	// UE_LOG(LogTemp, Warning, TEXT("ThisTagName: %s"), *ThisTagName); // 同样，解引用 FString 类型
	
	GetConceptDesignLibMenuApi->SendGetConceptDesignLibMenuRequest(Ticket, CurrentPage, FolderId, MenuType, PageSize, paintingName, ProjectNo, TagId, ThisTagName, Uuid, OnGetConceptDesignLibMenuResponse);
}


void SProjectWidget::CollapseAllExcept(EButtonClick ActiveButton)
{
	// Whether to fold all labels 是否折叠所有标签
	bool bCollapseAll = (ActiveButton == EButtonClick::None);

	// Traverse the state of all labels 遍历所有标签的状态
	for (auto& Entry : ExpandedStateMap)
	{
		// If you need to fold all labels, or if the current label is not the passed ActiveButton, collapse it 如果需要折叠所有标签，或者当前标签不是传入的 ActiveButton，则将其折叠
		if (bCollapseAll || Entry.Key != ActiveButton)
		{
			Entry.Value = false;

			// Empty the corresponding content container based on the label type and reset the style 根据标签类型清空对应的内容容器，并重置样式
			switch (Entry.Key)
			{
			case EButtonClick::ConceptDesign:
				ConceptDesignTreeContainer->ClearChildren();
				InitializeConceptButtonStyle();
				break;

			case EButtonClick::AudioAssets:
				AudioTreeContainer->ClearChildren();
				InitializeAudioButtonStyle();
				break;

			case EButtonClick::VideoAssets:
				VideoTreeContainer->ClearChildren();
				InitializeVideoButtonStyle();
				break;

			case EButtonClick::ModelAssets:
				ModelTreeContainer->ClearChildren();
				InitializeModelButtonStyle();
				break;

			default:
				// UE_LOG(LogTemp, Warning, TEXT("Unknown ButtonType: %d"), static_cast<int32>(Entry.Key));
				break;
			}
		}
	}

	if (bCollapseAll)
	{
		// UE_LOG(LogTemp, Log, TEXT("Collapsed all labels."));
	}
	else
	{
		// UE_LOG(LogTemp, Log, TEXT("Collapsed all labels except for: %d"), static_cast<int32>(ActiveButton));
	}
}

void SProjectWidget::ResetSelectedTag()
{
	switch (TagClick)
	{
	case ETagClick::ConceptTag:
		SelectedButtonStyle->SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal.NoArrow"));
		SelectedButtonStyle->SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered.NoArrow"));
		SelectedButtonStyle->SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
		break;
	case ETagClick::AudioTag:
		SelectedButtonStyle->SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal.NoArrow"));
		SelectedButtonStyle->SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered.NoArrow"));
		SelectedButtonStyle->SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
		break;
	case ETagClick::VideoTag:
		SelectedButtonStyle->SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal"));
		SelectedButtonStyle->SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered"));
		SelectedButtonStyle->SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
		break;
	case ETagClick::ModelTag:
		SelectedButtonStyle->SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal"));
		SelectedButtonStyle->SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered"));
		SelectedButtonStyle->SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
		break;
	case ETagClick::None:
		break;
	}
}


// Recursive function: Resets the expansion state of all sublabels 递归函数：重置所有子标签的展开状态
void SProjectWidget::ResetExpandedState(EButtonClick ParentButtonType)
{
	// Fold only its own state 仅折叠自身状态
	if (ExpandedStateMap.Contains(ParentButtonType))
	{
		ExpandedStateMap[ParentButtonType] = false;
	}

	// Empty the content container associated with the current button 清空与当前按钮关联的内容容器
	switch (ParentButtonType)
	{
	case EButtonClick::ConceptDesign:
		ConceptDesignTreeContainer->ClearChildren();
		break;

	case EButtonClick::AudioAssets:
		AudioTreeContainer->ClearChildren();
		break;

	case EButtonClick::VideoAssets:
		VideoTreeContainer->ClearChildren();
		break;

	case EButtonClick::ModelAssets:
		ModelTreeContainer->ClearChildren();
		break;

	default:
		// UE_LOG(LogTemp, Warning, TEXT("Unknown ParentButtonType passed to ResetExpandedState"));
		break;
	}

	// UE_LOG(LogTemp, Log, TEXT("Reset expanded state for: %d"), static_cast<int32>(ParentButtonType));
}



void SProjectWidget::RequestConceptDesignLibrary(int32 CurrentFileId, TSharedPtr<SVerticalBox> ParentBox)
{
	ResetDetailBar();
	UGetConceptDesignLibraryApi* GetConceptDesignLibraryApi = NewObject<UGetConceptDesignLibraryApi>();
	if (GetConceptDesignLibraryApi)
	{
		FOnGetConceptDesignLibraryResponse OnGetConceptDesignLibraryResponse;
		OnGetConceptDesignLibraryResponse.BindLambda([this, ParentBox, CurrentFileId](UGetConceptDesignLibraryResponseData* ConceptDesignLibraryData)
		{
			if (ConceptDesignLibraryData)
			{
				ParentBox->ClearChildren();
				// Create an array to hold the complete asset data (including files and folders) needed for the right side interface 创建一个数组保存右侧界面所需的完整资产数据（包括文件和文件夹）
				TArray<FConceptDesignFolderItem> ConceptDesignAssetsData = ConceptDesignLibraryData->data;  
				for (const FConceptDesignFolderItem& ConceptDesignFolderItem : ConceptDesignLibraryData->data)
				{
                    TSharedPtr<SVerticalBox> ChildBox = SNew(SVerticalBox);

					TSharedPtr<FButtonStyle> FolderButtonStyle = MakeShareable(new FButtonStyle(ConceptFolderButtonStyle));

                    ParentBox->AddSlot()
                    .AutoHeight()
					.Padding(0, 0, 0, 0)
                    [
                        SNew(SButton)
                        .Cursor(EMouseCursor::Hand)
                        .ButtonStyle(FolderButtonStyle.Get())
                        .HAlign(HAlign_Left) 
                        .OnClicked_Lambda([this, ConceptDesignFolderItem, ChildBox, ConceptDesignAssetsData, FolderButtonStyle]() -> FReply
                        {
                        	FImageLoader::CancelAllImageRequests();
                        	ClearAllCachedTextures();
                        	bIsConceptFirstPage = false;
                        	ResetSlateWidgets();
                        	ResetToggleTagContainer();
							if (SelectedButtonStyle.IsValid())
							{
							ResetSelectedTag();
							}

							FolderButtonStyle->SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
							FolderButtonStyle->SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
                        	
							SelectedButtonStyle = FolderButtonStyle;
							TagClick = ETagClick::ConceptTag;

							// Dynamically update ModelAssetsWidget on the right to display folder and file ICONS 动态更新右侧的 ModelAssetsWidget，显示文件夹和文件图标
							TArray<FConceptDesignFolderItem> SpecificConceptDesignData = { ConceptDesignFolderItem }; 

							UpdateConceptDesignAssetsWidget(SpecificConceptDesignData);

							GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentConceptFolderItems(SpecificConceptDesignData);
                        	GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentConceptFolderID(FString::FromInt(ConceptDesignFolderItem.id));

							return FReply::Handled();
                        })
                        [
	                        SNew(SBox)
							 .HeightOverride(40)
							 .WidthOverride(200)
							.Padding(FMargin(56, 0, 0, 0))  
							[
								SNew(SHorizontalBox)
								
								+ SHorizontalBox::Slot()
								.AutoWidth() 
								.VAlign(VAlign_Center)
								.Padding(FMargin(5, 0, 10, 0))  
								[
									SNew(SImage)
									.Image(FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.SmallFolder.Icon"))
								]
								
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(EVerticalAlignment::VAlign_Center)
								.HAlign(HAlign_Center)
								.Padding(0, 1, 0, 0)
								[
									SNew(STextBlock)
									.Text(FText::FromString(TruncateText(ConceptDesignFolderItem.folderName, 24)))
								]
							]
						]
                    ];

                    ParentBox->AddSlot()
                    .AutoHeight()
					.Padding(0, 0, 0, 0)
                    [
                        ChildBox.ToSharedRef()  
                    ];
				}
			}
		});
		FileId = 0;
		// UE_LOG(LogTemp, Log, TEXT("ConceptDesignLibrary Button Clicked! ProjectNo: %s, Uuid: %s, Ticket: %S, FileId: %d"), *ProjectNo, *Uuid, *Ticket, FileId);

		GetConceptDesignLibraryApi->SendGetConceptDesignLibraryRequest(Ticket, Uuid, ProjectNo, OnGetConceptDesignLibraryResponse);
	}
}


void SProjectWidget::RequestAudioAssetLibrary(int32 CurrentFileId, TSharedPtr<SVerticalBox> ParentBox)
{
	ResetDetailBar();
	UGetAudioAssetLibraryFolderListApi* GetAudioAssetLibraryFolderListApi = NewObject<UGetAudioAssetLibraryFolderListApi>();
	if (GetAudioAssetLibraryFolderListApi)
	{
		FOnGetAudioAssetLibraryFolderListResponse OnGetAudioAssetLibraryFolderListResponse;
		OnGetAudioAssetLibraryFolderListResponse.BindLambda([this, ParentBox, CurrentFileId](FGetAudioAssetLibraryFolderListData* AudioLibData)
		{
			if (AudioLibData)
			{
				ParentBox->ClearChildren();
				TArray<FAudioAssetLibraryFolderItem> AudioAssetsData = AudioLibData->Data;
				for (const FAudioAssetLibraryFolderItem& AudioFolder : AudioLibData->Data)
				{
                   
                   TSharedPtr<SVerticalBox> ChildBox = SNew(SVerticalBox);

					TSharedPtr<FButtonStyle> FolderButtonStyle = MakeShareable(new FButtonStyle(AudioFolderButtonStyle));


                   ParentBox->AddSlot()
                   .AutoHeight()
					.Padding(0, 0, 0, 0)
                   [
                       SNew(SButton)
                       .Cursor(EMouseCursor::Hand)
                       .ButtonStyle(FolderButtonStyle.Get())
                       .HAlign(HAlign_Left)  
                       .OnClicked_Lambda([this, AudioFolder, ChildBox, AudioAssetsData, FolderButtonStyle]() -> FReply
                       	{
                       		FImageLoader::CancelAllImageRequests();
                       		bIsAudioFirstPage = false;
							ResetSlateWidgets();
                       		ResetToggleTagContainer();
							// If there is a previously selected button, restore its style to its initial state 如果有之前选中的按钮，将其样式恢复为初始状态
							if (SelectedButtonStyle.IsValid())
							{
							ResetSelectedTag();
							}

							FolderButtonStyle->SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
							FolderButtonStyle->SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));

                       	
							SelectedButtonStyle = FolderButtonStyle;
							TagClick = ETagClick::AudioTag;
                       	
							TArray<FAudioAssetLibraryFolderItem> SpecificAudioData = { AudioFolder }; 

							UpdateAudioAssetsWidget(SpecificAudioData);

							GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentAudioFolderItems(SpecificAudioData);
                       		GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentAudioGroupID(FString::FromInt(AudioFolder.Id));

							return FReply::Handled();
                       })
                       [
	                       SNew(SBox)
							 .HeightOverride(40)
							 .WidthOverride(200)
							.Padding(FMargin(56, 0, 0, 0))  
							[
		                        SNew(SHorizontalBox)
		                        
		                        + SHorizontalBox::Slot()
		                        .AutoWidth()  
		                        .VAlign(VAlign_Center)
								.Padding(FMargin(5, 0, 10, 0))
		                        [
		                            SNew(SImage)
		                            .Image(FRSAssetLibraryStyle::Get().GetBrush("PluginIcon.SmallFolder.Icon")) 
		                        ]
		                        
		                        + SHorizontalBox::Slot()
		                        .AutoWidth()
		                        .VAlign(EVerticalAlignment::VAlign_Center)
								.HAlign(HAlign_Center)
								.Padding(0, 1, 0, 0)
		                        [
		                            SNew(STextBlock)
		                            .Text(FText::FromString(TruncateText(AudioFolder.GroupName, 24)))
		                        ]
		                    ]
						]
                   ];

                   ParentBox->AddSlot()
                   .AutoHeight()
					.Padding(0, 0, 0, 0)
                   [
                       ChildBox.ToSharedRef() 
                    ];
					
				}
				
			}
			
		});
		FString GroupName = "";  
		
		// UE_LOG(LogTemp, Log, TEXT("Sending GetAudioAssetLibraryFolderList request with parameters: Ticket: %s, Uuid: %s, ProjectNo: %s, GroupName: %s"), *Ticket, *Uuid, *ProjectNo, *GroupName);
		GetAudioAssetLibraryFolderListApi->SendGetAudioAssetLibraryFolderListRequest(Ticket, Uuid, ProjectNo, GroupName, OnGetAudioAssetLibraryFolderListResponse);
	}

}


void SProjectWidget::HandleModelAssetClicked(const FModelFileItem& ClickedFileItem, TSharedRef<SWidget> DetailsWidget)
{
	UpdateDetailsBar(DetailsWidget);
}

void SProjectWidget::HandleModelAssetDownloadClicked(const FString& InName, const FString& InURL, const FString& InMD5)
{
	AddToDownloadQueue(InName, InURL, InMD5);
}

void SProjectWidget::HandleAudioAssetClicked(const FAudioFileData& ClickedAudioFolder, TSharedRef<SWidget> DetailsWidget)
{
	
	UpdateDetailsBar(DetailsWidget);  
	
}

void SProjectWidget::HandleAudioAssetDownloadClicked(const FString& InName, const FString& InURL, const FString& InMD5)
{

	AddToDownloadQueue(InName, InURL, InMD5);
}

void SProjectWidget::HandleConceptDesignAssetClicked(const FConceptDesignFileItem& ClickedAudioFolder, TSharedRef<SWidget> DetailsWidget)
{
	
	UpdateDetailsBar(DetailsWidget); 
	
}

void SProjectWidget::HandleConceptAssetDownloadClicked(const FString& InName, const FString& InURL,
	const FString& InMD5)
{
	AddToDownloadQueue(InName, InURL, InMD5);
}

void SProjectWidget::HandleVideoAssetClicked(const FVideoAssetInfo& ClickedVideoFileItem, TSharedRef<SWidget> DetailsWidget)
{
	UpdateDetailsBar(DetailsWidget);
}

void SProjectWidget::HandleVideoAssetDownloadClicked(const FString& InName, const FString& InURL, const FString& InMD5)
{
	AddToDownloadQueue(InName, InURL, InMD5);
}


void SProjectWidget::UpdateModelAssetsWidget(const TArray<FModelFileItem>& ModelAssetsData)
{
	if (!ModelAssetsWidget.IsValid())
	{
		ModelAssetsWidget = SNew(SModelAssetsWidget)
							.OnModelAssetClicked(FOnModelAssetClicked::CreateSP(this, &SProjectWidget::HandleModelAssetClicked))
							.OnSelectedModelDownloadClicked(FOnSelectedModelDownloadClicked::CreateSP(this, &SProjectWidget::HandleModelAssetDownloadClicked))
							.OnModelNothingToShow(FOnModelNothingToShow::CreateLambda([this]()
							{
								ResetSlateWidgets();
							}))
							.OnModelRefresh(FOnModelRefresh::CreateLambda([this]()
							{
								UpdateRightContentBox(ModelAssetsWidget.ToSharedRef());
							}));
	}

	
	ModelAssetsWidget->ClearModelContent();

	// Check VideoAssetsData for folder type elements 检查 VideoAssetsData 中是否有文件夹类型的元素
	int32 FileCount = 0;
	for (const FModelFileItem& ModelAsset : ModelAssetsData)
	{
		if (ModelAsset.fileType == 0) // 1 Indicates the folder type 1 表示文件夹类型
		{
			FileCount++;
		}
	}

	if ( FileCount > 0 )
	{
		ModelAssetsWidget->UpdateModelAssets(ModelAssetsData);
		UpdateRightContentBox(ModelAssetsWidget.ToSharedRef());
		GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetCurrentModelItems(ModelAssetsData);
		return;
	}
	ResetSlateWidgets();
	GEditor->GetEditorSubsystem<UUSMSubsystem>()->ResetCurrentModelItems();
}

void SProjectWidget::UpdateAudioAssetsWidget(const TArray<FAudioAssetLibraryFolderItem>& SpecificAudioData) {
	if (!AudioAssetsWidget.IsValid()) {
		AudioAssetsWidget = SNew(SAudioAssetsWidget)
							.OnAudioAssetClicked(FOnAudioAssetClicked::CreateSP(this, &SProjectWidget::HandleAudioAssetClicked)) 
							.OnAudioDownload(FOnSelectedAudioDownloadClicked::CreateSP(this, &SProjectWidget::HandleModelAssetDownloadClicked))
							.OnAudioNothingToShow(FOnAudioNothingToShow::CreateLambda([this]()
							{
								ResetSlateWidgets();
							}))
							.OnAudioRefresh(FOnAudioRefresh::CreateLambda([this]()
							{
								UpdateRightContentBox(AudioAssetsWidget.ToSharedRef());
							}));
	}
	
	int64 TagIDNone = *""; 
	AudioAssetsWidget->ClearAudioContent();
	AudioAssetsWidget->UpdateAudioAssets(SpecificAudioData, TagIDNone);
	
	UpdateRightContentBox(AudioAssetsWidget.ToSharedRef());
}

void SProjectWidget::UpdateConceptDesignAssetsWidget(const TArray<FConceptDesignFolderItem> &SpecificConceptDesignData) {
	if (!ConceptDesignWidget.IsValid()) {
		ConceptDesignWidget = SNew(SConceptDesignWidget)
									.OnConceptDesignAssetClicked(FOnConceptDesignAssetClicked::CreateSP(this, &SProjectWidget::HandleConceptDesignAssetClicked)) // 绑定委托
									.OnConceptDownload(FOnSelectedConceptDownloadClicked::CreateSP(this, &SProjectWidget::HandleConceptAssetDownloadClicked))
									.OnConceptNothingToShow(FOnConceptNothingToShow::CreateLambda([this]()
										{
											ResetSlateWidgets();
										}))
									.OnConceptRefresh(FOnConceptRefresh::CreateLambda([this]()
									{
										UpdateRightContentBox(ConceptDesignWidget.ToSharedRef());
									}));
	}

	FString TagIDNone = "";
	
	ConceptDesignWidget->ClearConceptContent();
	ConceptDesignWidget->UpdateConceptDesignAssets(SpecificConceptDesignData, TagIDNone);
	
	UpdateRightContentBox(ConceptDesignWidget.ToSharedRef());
}

void SProjectWidget::UpdateVideoAssetsWidget(const TArray<FVideoAssetInfo>& VideoAssetsData)
{
	ButtonClick = EButtonClick::VideoAssets;
	if (!VideoAssetsWidget.IsValid())
	{
		VideoAssetsWidget = SNew(SVideoAssetsWidget)
							.OnVideoAssetClicked(FOnVideoAssetClicked::CreateSP(this, &SProjectWidget::HandleVideoAssetClicked))
							.OnVideoDownload(FOnSelectedVideoDownloadClicked::CreateSP(this, &SProjectWidget::HandleVideoAssetDownloadClicked))
							.OnVideoNothingToShow(FOnVideoNothingToShow::CreateLambda([this]
							{
								ResetSlateWidgets();
							}));
	}
	
	VideoAssetsWidget->ClearVideoContent();
	
	int32 FileCount = 0;
	for (const FVideoAssetInfo& VideoFileItem : VideoAssetsData)
	{
		if (VideoFileItem.fileType == 1)
		{
			FileCount++;
		}
	}

	if ( FileCount > 0)
	{
		VideoAssetsWidget->UpdateVideoAssets(VideoAssetsData);
		UpdateRightContentBox(VideoAssetsWidget.ToSharedRef());

		return;
	}
	ResetSlateWidgets();
}


void SProjectWidget::ClearSessionVariables()
{
	// Clear the strings, integers, and UserAndProjectInfo 清空字符串、整数以及 UserAndProjectInfo
	Ticket.Empty();
	Uuid.Empty();
	FileId = 0;
	ProjectNo.Empty();
	FileName.Empty();
	TagName.Empty();
	UserAndProjectInfo = FUserAndProjectInfo();  

	// UE_LOG(LogTemp, Log, TEXT("会话变量已清空"));
}


void SProjectWidget::ResetSlateWidgets()
{
	//Reset the ContentBar  重置 ContentBar
	if (ContentBar.IsValid())
	{
		ContentBar->SetContent
		(
			SNew(SBorder)
			.BorderImage(new FSlateRoundedBoxBrush(FLinearColor(0.011612, 0.011612, 0.011612, 1.0f), 6.0f)) 
			.Padding(FMargin(1)) 
			.Padding(0,0,5,0) 
			[
				SNew(SBox)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(SImage)
					.Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.NoAssets"))
				]
			]
		);
	}

	// Reset the DetailsBar 重置 DetailsBar
	if (DetailsBar.IsValid())
	{
		DetailsBar->SetContent(
			SNew(SBorder)
		   .Padding(5,0,0,0)
		   .BorderImage(new FSlateRoundedBoxBrush(FLinearColor(0.011612, 0.011612, 0.011612, 1.0f), 6.0f))
			.Padding(FMargin(1)) 
			[
				SNew(SBox)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("NoAssetSelected2", "未选择资产"))
					.Justification(ETextJustify::Center)
				]
			]
		);
	}
	
	if (ProjectlistWindow.IsValid())
	{
		ProjectlistWindow.Reset();
	}

	// UE_LOG(LogTemp, Log, TEXT("Slate 控件已重置"));
	
}


void SProjectWidget::ResetDetailBar()
{
	if (DetailsBar.IsValid())
	{
		DetailsBar->SetContent(
			SNew(SBorder)
		   .Padding(5,0,0,0)
		   .BorderImage(new FSlateRoundedBoxBrush(FLinearColor(0.011612, 0.011612, 0.011612, 1.0f), 6.0f))
		   .Padding(FMargin(1)) 
			[
				SNew(SBox)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("NoAssetSelected3", "未选择资产"))
					.Justification(ETextJustify::Center)
				]
			]
		);
	}
}

void SProjectWidget::DoubleClearCheck(TSharedPtr<SVerticalBox> TreeContainer)
{
	if (TreeContainer.IsValid())
	{
		// First clear 第一次清除
		TreeContainer->ClearChildren();
		TreeContainer->Invalidate(EInvalidateWidget::Layout);
		// TreeContainer->InvalidatePrepass();
		TreeContainer->Invalidate(EInvalidateWidgetReason::Prepass);

		// If residue remains after removal, remove it again 如果清除后仍有残留，再次清除
		if (TreeContainer->GetChildren()->Num() > 0)
		{
			TreeContainer->ClearChildren();
			TreeContainer->Invalidate(EInvalidateWidget::Layout);
			// TreeContainer->InvalidatePrepass();
			TreeContainer->Invalidate(EInvalidateWidgetReason::Prepass);
		}
	}
}

void SProjectWidget::InitializeConceptButtonStyle()
{
	ConceptButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal"));
	ConceptButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered"));
	ConceptButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Pressed"));
}

void SProjectWidget::InitializeAudioButtonStyle()
{
	AudioButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal"));
	AudioButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered"));
	AudioButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Pressed"));
}

void SProjectWidget::InitializeVideoButtonStyle()
{
	VideoButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal"));
	VideoButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered"));
	VideoButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Pressed"));
}

void SProjectWidget::InitializeModelButtonStyle()
{
	ModelButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal"));
	ModelButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered"));
	ModelButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Pressed"));
}

void SProjectWidget::InitializeProjectListButtonStyle()
{
	ProjectListButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal.NoArrow"));
	ProjectListButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered.NoArrow"));
	ProjectListButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered.NoArrow"));
}

void SProjectWidget::InitializeConceptFolderButtonStyle()
{
	ConceptFolderButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal.NoArrow"));
	ConceptFolderButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered.NoArrow"));
	ConceptFolderButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
}

void SProjectWidget::InitializeAudioFolderButtonStyle()
{
	AudioFolderButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal.NoArrow"));
	AudioFolderButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered.NoArrow"));
	AudioFolderButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
}

void SProjectWidget::InitializeVideoFolderButtonStyle()
{
	VideoFolderButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal"));
	VideoFolderButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered"));
	VideoFolderButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
}

void SProjectWidget::InitializeModelFolderButtonStyle()
{
	ModelFolderButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal"));
	ModelFolderButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered"));
	ModelFolderButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
}

void SProjectWidget::InitializeLogoutButtonStyle()
{
	LogoutButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.LogoutButton"));
	LogoutButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.LogoutButton"));
	LogoutButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.LogoutButton"));
}

void SProjectWidget::CLearWidgetTexture()
{
	// Check that the ConceptDesignWidget is valid and invoke the cleanup method 检查 ConceptDesignWidget 是否有效并调用清理方法
	if (ConceptDesignWidget.IsValid())
	{
		ConceptDesignWidget->ClearConceptContent();
	}

	if (VideoAssetsWidget.IsValid())
	{
		VideoAssetsWidget->ClearVideoContent();
	}
	
	if (ModelAssetsWidget.IsValid())
	{
		ModelAssetsWidget->ClearModelContent();
	}
}


void SProjectWidget::ResetAllButtonStyles()
{
	ModelFolderButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal"));
	ModelFolderButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered"));
	ModelFolderButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));

	VideoFolderButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal.NoArrow"));
	VideoFolderButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered.NoArrow"));
	VideoFolderButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));

	AudioFolderButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal.NoArrow"));
	AudioFolderButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered.NoArrow"));
	AudioFolderButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));

	ConceptFolderButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Normal.NoArrow"));
	ConceptFolderButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.Hovered.NoArrow"));
	ConceptFolderButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.IconTab.AssetSelected"));
}


void SProjectWidget::UpdateRightContentBox(TSharedRef<SWidget> NewContentWidget)
{
	
	// Determine and set the enumeration value based on the NewContentWidget 根据 NewContentWidget 判断并设置枚举值
	if (NewContentWidget == ConceptDesignWidget)
	{
		CurrentActiveWidget = EActiveWidget::ConceptDesign;
	}
	else if (NewContentWidget == AudioAssetsWidget)
	{
		CurrentActiveWidget = EActiveWidget::AudioAssets;
	}
	else if (NewContentWidget == VideoAssetsWidget)
	{
		CurrentActiveWidget = EActiveWidget::VideoAssets;
	}
	else if (NewContentWidget == ModelAssetsWidget)
	{
		CurrentActiveWidget = EActiveWidget::ModelAssets;
	}
	else
	{
		CurrentActiveWidget = EActiveWidget::None;
	}
	
	// Update the contents of the ContentBar 更新 ContentBar 的内容
	ContentBar->SetContent(NewContentWidget);
}

void SProjectWidget::SetUserAndProjectParams()
{
	USMSubsystem = GEditor->GetEditorSubsystem<UUSMSubsystem>();
	Ticket = USMSubsystem->GetCurrentUserAndProjectInfo().Ticket;
	Uuid = USMSubsystem->GetCurrentUserAndProjectInfo().Uuid;
	ProjectNo = USMSubsystem->GetSelectedProject().projectNo;
}

void SProjectWidget::SetProjectButtonEnabled(bool bEnabled)
{
    if (ProjectlistButton.IsValid())
    {
        ProjectlistButton->SetEnabled(bEnabled);
    }
}

FReply SProjectWidget::OnProjectButtonClicked()
{
    if (bIsProjectListWindowOpen)
    {
        CloseProjectListWindow();
        return FReply::Handled();
    }
	
    bIsProjectListWindowOpen = true;

    // Disable button to prevent repeated calls 禁用按钮以防重复调用
    SetProjectButtonEnabled(false);

    // Dynamically fills the top container contents 动态填充顶部容器内容
    ProjectListContainer->ClearChildren();
    ProjectListContainer->SetVisibility(EVisibility::Visible);
	
    UFindProjectListApi* FindProjectListApi = NewObject<UFindProjectListApi>();
    if (FindProjectListApi)
    {
        FOnFindProjectListResponse OnFindProjectListResponseDelegate;
        OnFindProjectListResponseDelegate.BindLambda([this](UFindProjectListResponseData* ProjectListData)
        {
            SetProjectButtonEnabled(true);

            if (ProjectListData)
            {
                
                GEditor->GetEditorSubsystem<UUSMSubsystem>()->SetUpdatePrjectItems(ProjectListData->data);
                FUserAndProjectInfo UpdatedUserAndProjectInfo = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentUserAndProjectInfo();
            	
                ProjectListContainer->AddSlot()
                .AutoHeight()
                [
                    SNew(SBox)
                    .HeightOverride_Lambda([this]() -> float
                    {
                        return 310.f * ProjectListAnimationCurve.GetLerp();
                    })
                    [
                        SNew(SProjectlistWidget)
                        .UserAndProjectInfo(UpdatedUserAndProjectInfo)
                        .OnProjectSwitched(FOnProjectSwitchedDelegate::CreateSP(this, &SProjectWidget::HandleProjectSwitched))
                    ]
                ];

                // Startup animation 启动动画
                ProjectListAnimationSequence.Play(this->AsShared());

                // Enable focus change listening 启用焦点变化监听
                if (!FocusChangeHandle.IsValid())
                {
                    FocusChangeHandle = FSlateApplication::Get().OnFocusChanging().AddSP(this, &SProjectWidget::HandleFocusChanging);
                }
            }
            else
            {
           
                SetProjectButtonEnabled(true);
                // UE_LOG(LogTemp, Warning, TEXT("Project list data is null."));
            }
        });

        SetUserAndProjectParams();
        FindProjectListApi->SendFindProjectListRequest(Ticket, Uuid, OnFindProjectListResponseDelegate);
    }
    else
    {
        
        SetProjectButtonEnabled(true);
        // UE_LOG(LogTemp, Error, TEXT("Failed to create FindProjectListApi."));
    }

    return FReply::Handled();
}


void SProjectWidget::CloseProjectListWindow()
{
	bIsProjectListWindowOpen = false;

	// Stop the animation and empty the content 停止动画并清空内容
	ProjectListAnimationSequence.JumpToEnd();
	ProjectListContainer->ClearChildren();
	ProjectListContainer->SetVisibility(EVisibility::Collapsed);

	// Remove focus change listening 移除焦点变化监听
	if (FocusChangeHandle.IsValid())
	{
		FSlateApplication::Get().OnFocusChanging().Remove(FocusChangeHandle);
		FocusChangeHandle.Reset();
	}
}

void SProjectWidget::HandleFocusChanging(const FFocusEvent& InFocusEvent,
										 const FWeakWidgetPath& InWeakWidgetPath,
										 const TSharedPtr<SWidget>& InNewFocusedWidget,
										 const FWidgetPath& InWidgetPath,
										 const TSharedPtr<SWidget>& InCurrentFocus)
{
	if (bIsProjectListWindowOpen)
	{
		bool bIsInProjectListWindow = InWidgetPath.ContainsWidget(ProjectListContainer.Get());

		// If the new focus is not in the project list window, close the window 如果新焦点不在项目列表窗口中，关闭窗口
		if (!InNewFocusedWidget.IsValid() || !bIsInProjectListWindow)
		{
			CloseProjectListWindow();
			// SetProjectButtonEnabled(true);  // 恢复按钮可用
		}
	}
}


// For switching items 用于切换项目
void SProjectWidget::HandleProjectSwitched()
{
	CurrentProjectName = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetSelectedProject().projectName;
	
	if (ProjectlistButton.IsValid())
	{
		ProjectlistButton->Invalidate(EInvalidateWidgetReason::Layout);
	}
	
	OnProjectSelected();
	ResetSlateWidgets();
	ClearAllCachedTextures();
	CloseAllOpenedWindows();

	FImageLoader::CancelAllImageRequests();
	
	CollapseAllExcept(EButtonClick::None); // Fold all labels 折叠所有标签

	CurrentActiveWidget = EActiveWidget::None;

	if (TagContainer.IsValid())
	{
		// Toggle window visibility and start animation 切换窗口的可见性并启动动画
		if (TagContainer->GetVisibility() == EVisibility::Visible)
		{
			TagContainer->SetVisibility(EVisibility::Collapsed);
		}
	}
}

// The project selects the callback function, automatically closes the window and enables the button after selecting the project 项目选择回调函数，选择项目后自动关闭窗口并启用按钮
void SProjectWidget::OnProjectSelected()
{
    CloseProjectListWindow();

	ResetDetailBar();

    SetProjectButtonEnabled(true);
}





void SProjectWidget::OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		// Get the search term and remove the Spaces before and after 获取搜索关键词并去掉前后空格
		FString SearchKeyword = Text.ToString().TrimStartAndEnd();
		ResetDetailBar();

		switch (CurrentActiveWidget)
		{
		case EActiveWidget::VideoAssets:
			// UE_LOG(LogTemp, Log, TEXT("搜索关键词: %s"), *SearchKeyword);

			if (VideoAssetsWidget)
			{
				if (SearchKeyword.IsEmpty())
				{
					// UE_LOG(LogTemp, Warning, TEXT("搜索关键词为空，恢复默认显示。"));
					VideoAssetsWidget->UpdateVideoAssets(GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentVideoFolderItems());
					
					UpdateRightContentBox(VideoAssetsWidget.ToSharedRef());
					
					break;
				}
				VideoAssetsWidget->SearchVideoFileByName(Text);
				UpdateRightContentBox(VideoAssetsWidget.ToSharedRef()); // Update the display to ensure that search results are displayed correctly 更新显示以确保搜索结果显示正确
			}
			break;

		case EActiveWidget::AudioAssets:
			// UE_LOG(LogTemp, Log, TEXT("当前音频页不支持搜索！"));
			break;

		case EActiveWidget::ConceptDesign:
			// UE_LOG(LogTemp, Log, TEXT("当前概设页不支持搜索！"));
			break;

		case EActiveWidget::ModelAssets:
			// UE_LOG(LogTemp, Log, TEXT("当前模型页不支持搜索！"));
			break;

		default:
			// UE_LOG(LogTemp, Log, TEXT("当前默认页不支持搜索！"));
			break;
		}
	}
}



FReply SProjectWidget::OnTagButtonClicked()
{
    // UE_LOG(LogTemp, Warning, TEXT("--- OnTagButtonClicked ---"));

    switch (CurrentActiveWidget)
    {
    case EActiveWidget::ConceptDesign:
    	MainTagButton->SetEnabled(false);
        // UE_LOG(LogTemp, Warning, TEXT("Active Widget: ConceptDesignWidget"));
        
        if (TagContainer.IsValid())
        {
            TagContainer->SetContent(
                SAssignNew(ConceptTagWidget, SConceptTagWidget)
                .ConceptDesignWidget(ConceptDesignWidget)
                .OnClearConceptTagFilter(FOnClearConceptTagFilter::CreateSP(this, &SProjectWidget::ClearConceptTagFilter))
                .OnConceptTagClickClearWidget(FOnConceptTagClickClearWidget::CreateLambda([this]()
				{
					ResetDetailBar();
				}))
				.OnConceptTagDataReceived(FOnConceptTagDataReceived::CreateLambda([this]()
				{
					MainTagButton->SetEnabled(true);
				})));

            // Toggle window visibility and start animation 切换窗口的可见性并启动动画
            ToggleTagContainerVisibility();
        }
        break;
        
    case EActiveWidget::AudioAssets:
    	MainTagButton->SetEnabled(false);
        // UE_LOG(LogTemp, Warning, TEXT("Active Widget: AudioAssetsWidget"));
        
        if (TagContainer.IsValid())
        {
            TagContainer->SetContent(
				SAssignNew(AudioTagWidget, SAudioTagWidget)
                .AudioAssetWidget(AudioAssetsWidget)
                .OnClearAudioTagFilter(FOnClearAudioTagFilter::CreateSP(this, &SProjectWidget::ClearAudioTagFilter))
                .OnAudioTagClickClearWidget(FOnAudioTagClickClearWidget::CreateLambda([this]()
				{
					ResetDetailBar();
				}))
        		.OnAudioTagDataReceived(FOnAudioTagDataReceived::CreateLambda([this]()
				{
					MainTagButton->SetEnabled(true);
				})));
        	
            ToggleTagContainerVisibility();
        }
        break;
        
    case EActiveWidget::VideoAssets:
        // UE_LOG(LogTemp, Warning, TEXT("Active Widget: VideoAssetsWidget"));
    	
        if (!ExistingNotificationWindow.IsValid())
        {
            
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
                    .Text(LOCTEXT("NoTags", "暂无标签"))
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
                    .OnClicked_Lambda([this]() -> FReply
                    {
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
        }
        else
        {
            
            ExistingNotificationWindow->BringToFront();
        }

        if (TagContainer.IsValid())
        {
            TagContainer->SetContent(SNullWidget::NullWidget);
        };

        TagContainer->SetVisibility(EVisibility::Collapsed);
        
        break;
        
    case EActiveWidget::ModelAssets:
        // UE_LOG(LogTemp, Warning, TEXT("Active Widget: ModelAssetsWidget"));
    	MainTagButton->SetEnabled(false);
        if (TagContainer.IsValid())
        {
           
            TagContainer->SetContent(
                SNew(SModelTagWidget)
                .ModelAssetsWidget(ModelAssetsWidget)
                .OnClearModelTagFilter(FOnClearModelTagFilter::CreateSP(this, &SProjectWidget::ClearModelTagFilter))
                .OnModelTagDataReceived(FOnModelTagDataReceived::CreateLambda([this]()
				{
					MainTagButton->SetEnabled(true);
				}))
                .OnModelTagClickClearWidget(FOnModelTagClickClearWidget::CreateLambda([this]()
				{
					ResetDetailBar();
				})));


            ToggleTagContainerVisibility();
        }
        break;
        
    case EActiveWidget::None:
    default:
        // UE_LOG(LogTemp, Warning, TEXT("No Active Widget"));
        break;
    }

    return FReply::Handled();
}


void SProjectWidget::ToggleTagContainerVisibility()
{
	if (TagContainer.IsValid())
	{
		if (TagContainer->GetVisibility() == EVisibility::Collapsed)
		{
			TagContainer->SetVisibility(EVisibility::Visible);
			TagAnimationSequence.Play(this->AsShared());
		}
		else
		{
			TagContainer->SetVisibility(EVisibility::Collapsed);
		}
	}
}

void SProjectWidget::ResetToggleTagContainer()
{
	if (TagContainer.IsValid())
	{
		if (TagContainer->GetVisibility() == EVisibility::Visible)
		{
			TagContainer->SetVisibility(EVisibility::Collapsed);
		}
	}
}

void SProjectWidget::ClearConceptTagFilter()
{
	FString TagIDNone = "";

	if (bIsConceptFirstPage)
	{
		if (!GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentFirstPageConceptItems().IsEmpty())
		{
			ConceptDesignWidget->UpdateConceptDesignTagPageAssets(GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentFirstPageConceptItems());
		}
	}
	else
	{
		if (!GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentConceptFolderItems().IsEmpty())
		{
			ConceptDesignWidget->UpdateConceptDesignAssets(GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentConceptFolderItems(), TagIDNone);
		}
		UpdateRightContentBox(ConceptDesignWidget.ToSharedRef());
	}
	
}

void SProjectWidget::ClearAudioTagFilter()
{
	int64 TagIDNone = *"";

	if (bIsAudioFirstPage)
	{
		if (!GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentFirstPageAudioFolderItems().IsEmpty())
		{
			AudioAssetsWidget->UpdateTagPageAudioAssets(GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentFirstPageAudioFolderItems());
		}
	}
	else
	{
		if (!GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentAudioFolderItems().IsEmpty())
		{
			AudioAssetsWidget->UpdateAudioAssets(GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentAudioFolderItems(), TagIDNone);
		}
		UpdateRightContentBox(AudioAssetsWidget.ToSharedRef());
	}
}



void SProjectWidget::ClearModelTagFilter()
{
	TArray<FModelFileItem> CurrentModeleItems = GEditor->GetEditorSubsystem<UUSMSubsystem>()->GetCurrentModelItems();

	if ( CurrentModeleItems.Num() > 0 )
	{
		
		ModelAssetsWidget->UpdateModelAssets(CurrentModeleItems);
	}
}

void SProjectWidget::ClearAllCachedTextures()
{
	if (ConceptDesignWidget.IsValid())
	{
		ConceptDesignWidget->ClearConceptContent();
	}
	
	if (ModelAssetsWidget.IsValid())
	{
		ModelAssetsWidget->ClearModelContent();
	}

	if (VideoAssetsWidget)
	{
		VideoAssetsWidget->ClearVideoContent();
	}
}


FReply SProjectWidget::OnDownloadButtonClicked()
{
	// UE_LOG(LogTemp, Warning, TEXT("--- OnDownloadButtonClicked ---"));
	
	ShowDownloadQueue();
	return FReply::Handled();
}


FReply SProjectWidget::OnUserInfoButtonClicked()
{
	// UE_LOG(LogTemp, Warning, TEXT("--- OnUserInfoButtonClicked ---"));

	TSharedRef<SBox> LogoutButtonContainer = SNew(SBox)
		.WidthOverride(70)
		.HeightOverride(35)
		[
			SNew(SButton)
			.ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder"))
			.ButtonStyle(&LogoutButtonStyle)
			.Cursor(EMouseCursor::Hand)
			.OnClicked(FOnClicked::CreateSP(this, &SProjectWidget::OnLogoutButtonClicked)) 
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Logout", "退出登录"))
					.Justification(ETextJustify::Center)
					.ColorAndOpacity(FSlateColor(FLinearColor::Black)) 
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				]
			]
		];

	// Gets the location of the avatar button 获取头像按钮的位置
	FVector2D UserInfoButtonPosition = UserInfoButton->GetCachedGeometry().GetAbsolutePosition();
	
	// Gets the bit size of the avatar button 获取头像按钮的位尺寸
	FVector2D UserInfoButtonSize = UserInfoButton->GetCachedGeometry().GetAbsoluteSize();

	// Gets the button width for dynamic calculations 获取动态计算的按钮宽度
	float LogoutButtonWidth = LogoutButtonContainer->GetCachedGeometry().GetLocalSize().X;

	// Calculate the position of the eject button so that its center lines up with the center of the avatar button 计算弹出按钮的位置，使其中心与头像按钮的中心对齐
	FVector2D ButtonPosition = UserInfoButtonPosition + FVector2D(-(UserInfoButtonSize.X - LogoutButtonWidth) * 0.8f, UserInfoButtonSize.Y);


	FSlateApplication::Get().PushMenu(
		AsShared(),
		FWidgetPath(),
		LogoutButtonContainer,
		ButtonPosition, // The menu is displayed in the calculated location 菜单显示在计算的位置
		FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
	);

	return FReply::Handled();
}

FReply  SProjectWidget::OnLogoutButtonClicked()
{
	FSlateApplication::Get().DismissAllMenus();
	FImageLoader::CancelAllImageRequests();
	
	// Gets the USMSubsystem instance 获取 USMSubsystem 实例
	USMSubsystem = GEditor->GetEditorSubsystem<UUSMSubsystem>();

	if (USMSubsystem)
	{
		// Clear the current session and user information 清空当前会话和用户信息
		USMSubsystem->ClearCurrentSession();
		USMSubsystem->ClearCurrentUserAndProjectInfo();
		USMSubsystem->ClearSelectedProject();

		// Calls methods to clear session variables and reset controls 调用清空会话变量和重置控件的方法
		ClearSessionVariables();
		ResetSlateWidgets();

		if (ProjectlistWindow.IsValid())
		{
			ProjectlistWindow->RequestDestroyWindow();
			ProjectlistWindow.Reset();
		}

		OnLogoutDelegate.ExecuteIfBound();
		
		// UE_LOG(LogTemp, Log, TEXT("已退出登录，清空会话和用户信息，并重置界面"));
	}

	return FReply::Handled();
}

FString SProjectWidget::TruncateText(const FString& OriginalText, int32 MaxLength)
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


// A function to update the contents of the SScrollBox 更新 SScrollBox 内容的函数
void SProjectWidget::UpdateScrollBoxContent(const TSharedRef<SWidget>& NewContent)
{
    if (ScrollBox.IsValid())
    {
        ScrollBox->ClearChildren();
        ScrollBox->AddSlot()
        [
            NewContent
        ];
    }
}

// Add a download queue item 添加下载队列项
void SProjectWidget::AddToDownloadQueue(const FString& SelectedFileName, const FString& SelectedURL, const FString& SelectedMD5)
{
	// Check the number of tasks in the current queue 检查当前队列中的任务数量
	int32 CurrentQueueSize = DownloadQueueContainer.IsValid() ? DownloadQueueContainer->GetChildren()->Num() : 0;

	if (CurrentQueueSize >= MaxDownloadQueueSize)
	{
		// If the queue is full, a dialog box is displayed 如果队列已满，弹出提示框
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("下载队列已达到最大限度，无法添加更多任务！")));
		return;
	}
	
    if (!DownloadQueueContainer.IsValid())
    {
        DownloadQueueContainer = SNew(SVerticalBox);
    }

    if (DownloadQueueContainer.IsValid())
    {
    	ShowDownloadingQueue();
    	
        TSharedPtr<SAssetDownloadWidget> LocalAssetDownloadWidget;

        DownloadQueueContainer->AddSlot()
        .AutoHeight()
        .Padding(5)
        [
            SAssignNew(LocalAssetDownloadWidget, SAssetDownloadWidget)
            .URL(SelectedURL)
            .FileName(SelectedFileName)
            .MD5(SelectedMD5)
            .ParentContainer(DownloadQueueContainer)
            .DownloadCompleteWidget(DownloadCompleteWidget)  // Pass the DownloadCompleteWidget to the download item 将 DownloadCompleteWidget 传递给下载项
        ];

        if (LocalAssetDownloadWidget.IsValid())
        {
            LocalAssetDownloadWidget->SetDownloadParameters(SelectedURL, SelectedFileName, SelectedMD5);
            LocalAssetDownloadWidget->OnStartOrResumeClicked();
        }
        else
        {
            // UE_LOG(LogTemp, Error, TEXT("Failed to create SAssetDownloadWidget in AddToDownloadQueue."));
        }

    	UpdateScrollBoxContent(DownloadQueueContainer.ToSharedRef());
    }
}


// The download queue window is displayed 显示下载队列窗口
void SProjectWidget::ShowDownloadQueue()
{
    if (!DownloadQueueContainer.IsValid())
    {
        DownloadQueueContainer = SNew(SVerticalBox);
    }
	
    if (!DownloadCompleteWidget.IsValid())
    {
        DownloadCompleteWidget = SNew(SDownloadCompleteWidget);
    }

    // Will DownloadCompleteWidget CompletedDownloadContainer assignment 将 DownloadCompleteWidget 赋值给 CompletedDownloadContainer
    if (!CompletedDownloadContainer.IsValid())
    {
        CompletedDownloadContainer = SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .FillHeight(1)
            [
                DownloadCompleteWidget.ToSharedRef()
            ];
    }

    if (!ScrollBox.IsValid())
    {
        ScrollBox = SNew(SScrollBox);
    }
	
    if (DownloadQueueWindow.IsValid())
    {
        DownloadQueueWindow->BringToFront();
        return;
    }

    DownloadQueueWindow = SNew(SWindow)
		.Title(LOCTEXT("TransferListTitle", "传输列表"))
        .FocusWhenFirstShown(true)
		.IsTopmostWindow(false)
        .ClientSize(FVector2D(700, 900))
		.SizingRule(ESizingRule::FixedSize)  
        .Content()
        [
            SNew(SBorder)
            .BorderImage(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadWindowBorder"))
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10, 15, 10, 15)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(0, 0, 20, 0)
                    [
                    	SNew(SBox)
                    	.WidthOverride(132.0)
                    	.HeightOverride(32.0)
                    	[
	                        SNew(SButton)
	                        .Cursor(EMouseCursor::Hand)
							.ButtonStyle(&DownloadButtonStyle) 
							.ContentPadding(0) 
	                        .OnClicked_Lambda([this]()
	                        {
	                            ShowDownloadingQueue();
	                            return FReply::Handled();
	                        })
	                        [
		                        SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.VAlign(VAlign_Center) 
								[
									SNew(STextBlock)
									.Text(LOCTEXT("DownloadText", "下载中"))
									.Justification(ETextJustify::Center)
									.Font(FCoreStyle::GetDefaultFontStyle("Regular", 15))
									.ColorAndOpacity(FSlateColor(FLinearColor::White))
								]
	                        ]
                        ]
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
	                    SNew(SBox)
						.WidthOverride(132.0)
						.HeightOverride(32.0)
						[
	                        SNew(SButton)
	                        .Cursor(EMouseCursor::Hand)
							.ButtonStyle(&CompleteButtonStyle) 
							.ContentPadding(0) 
	                        .OnClicked_Lambda([this]()
	                        {
	                            ShowCompletedQueue();
	                            return FReply::Handled();
	                        })
	                        [
		                        SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.Text(LOCTEXT("CompletedText", "已完成"))
									.Justification(ETextJustify::Center)
									.Font(FCoreStyle::GetDefaultFontStyle("Regular", 15))
									.ColorAndOpacity(FSlateColor(FLinearColor::White))
								]
	                        ]
	                    ]
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
				.Padding(15, 20, 0, 0)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Left)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NameText", "名称"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
						.ColorAndOpacity(FSlateColor(FLinearColor::White))
					]

					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(90, 0, 0, 0)
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SizeText", "大小"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
						.ColorAndOpacity(FSlateColor(FLinearColor::White))
					]

					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(0, 0, 50, 0)
					.HAlign(HAlign_Right)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("StatusText", "状态"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
						.ColorAndOpacity(FSlateColor(FLinearColor::White))
					]
				]
                
                + SVerticalBox::Slot()
                .FillHeight(1)
                [
                    ScrollBox.ToSharedRef() 
                ]
                
                + SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBox)
					.HeightOverride(50.0f)
					.WidthOverride(700.0f)
					[
						SNew(SBorder)
						.Padding(0)
						.BorderImage(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadBottom"))
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.FillWidth(1)
							
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.AutoWidth()
							.Padding(0, 5, 20, 5)
							[
								SNew(SButton)
								.Cursor(EMouseCursor::Hand)
								.ButtonStyle(&AllDownloadButtonStyle) 
								.ContentPadding(0) 
								
								.OnClicked_Lambda([this]()
								{
									StartAllDownloads();
									return FReply::Handled();
								})
								  [
                                     SNew(SBox)
                                     .WidthOverride(110)
                                     .HeightOverride(34)
                                     .Padding(5, 5, 5, 5)
                                     [
                                         SNew(SHorizontalBox)
                                         + SHorizontalBox::Slot()
                                         .AutoWidth()
                                         .VAlign(VAlign_Center)
                                         .Padding(5, 5, 5, 5)
                                         [
	                                         SNew(SBox)
											 .WidthOverride(20)
											 .HeightOverride(20)
											 [
	                                             SNew(SImage)
	                                             .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"))
	                                        ]
                                         ]
                                         + SHorizontalBox::Slot()
                                         .AutoWidth()
                                         .VAlign(VAlign_Center)
                                         .Padding(0, 5, 5, 5)
                                         [
                                             SNew(STextBlock)
                                             .Text(LOCTEXT("AllStartText", "全部开始"))
                                             .Justification(ETextJustify::Center)
                                             .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                                             .ColorAndOpacity(FSlateColor(FLinearColor(0.03922f, 0.55686f, 0.40784f, 1.0f))) 
                                         ]
                                     ]
                                 ]
							]

							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.AutoWidth()
							.Padding(0, 5, 20, 5)
							[
								SNew(SButton)
								
								.Cursor(EMouseCursor::Hand)
								.ButtonStyle(&AllDownloadButtonStyle) 
								.ContentPadding(0) 
								.OnClicked_Lambda([this]()
								{
									PauseAllDownloads();
									return FReply::Handled();
								})
								  [
                                     SNew(SBox)
                                     .WidthOverride(110)
                                     .HeightOverride(34)
                                     .Padding(5, 5, 5, 5)
                                     [
                                         SNew(SHorizontalBox)
                                         + SHorizontalBox::Slot()
                                         .AutoWidth()
                                         .VAlign(VAlign_Center)
                                         .Padding(5, 5, 5, 5)
                                         [
	                                         SNew(SBox)
											 .WidthOverride(21)
											 .HeightOverride(21)
											 [
													 SNew(SBox)
												 .WidthOverride(20)
												 .HeightOverride(20)
												 [
		                                             SNew(SImage)
		                                             .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"))
		                                        ]
                                             ]
                                         ]
                                         + SHorizontalBox::Slot()
                                         .AutoWidth()
                                         .VAlign(VAlign_Center)
                                         .Padding(0, 5, 5, 5)
                                         [
                                             SNew(STextBlock)
                                             .Text(LOCTEXT("AllPauseText", "全部暂停"))
                                             .Justification(ETextJustify::Center)
                                             .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                                             .ColorAndOpacity(FSlateColor(FLinearColor(0.03922f, 0.55686f, 0.40784f, 1.0f))) 
                                         ]
                                     ]
                                 ]
							]

							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.AutoWidth()
							.Padding(0, 5, 20, 5)
							[
								SNew(SButton)
								.Cursor(EMouseCursor::Hand)
								.ButtonStyle(&AllDownloadButtonStyle) 
								.ContentPadding(0) 
								.OnClicked_Lambda([this]()
								{
									CancelAllDownloads();
									return FReply::Handled();
								})
								  [
                                     SNew(SBox)
                                     .WidthOverride(110)
                                     .HeightOverride(34)
                                     .Padding(5, 5, 5, 5)
                                     [
                                         SNew(SHorizontalBox)
                                         + SHorizontalBox::Slot()
                                         .AutoWidth()
                                         .VAlign(VAlign_Center)
                                         .Padding(5, 5, 5, 5)
                                         [
	                                         SNew(SBox)
											  .WidthOverride(20)  // 固定宽度为20
											  .HeightOverride(20) // 固定高度为20
											  [
												  SNew(SImage)
												  .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.CancelClicked"))
											  ]
                                         ]
                                         + SHorizontalBox::Slot()
                                         .AutoWidth()
                                         .VAlign(VAlign_Center)
                                         .Padding(0, 5, 5, 5)
                                         [
                                             SNew(STextBlock)
                                             .Text(LOCTEXT("AllCancelText", "全部取消"))
                                             .Justification(ETextJustify::Center)
                                             .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                                             .ColorAndOpacity(FSlateColor(FLinearColor(0.03922f, 0.55686f, 0.40784f, 1.0f))) 
                                         ]
                                     ]
                                 ]
							]
						]
					]
				]
            ]
        ];

    DownloadQueueWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>&)
    {
    	
        DownloadQueueWindow.Reset();
    }));

    FSlateApplication::Get().AddWindow(DownloadQueueWindow.ToSharedRef());

    ShowDownloadingQueue(); 
	
	AllDownloadButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadBottom"));
	AllDownloadButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ImportHover"));
	AllDownloadButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.Importclicked"));

	DownloadButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadListButtonClicked"));
	DownloadButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadListButtonClicked"));
	DownloadButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadListButtonClicked"));

	CompleteButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadListButtonNormal"));
	CompleteButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadListButtonClicked"));
	CompleteButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadListButtonClicked"));
}

// The download queue is displayed 显示正在下载队列
void SProjectWidget::ShowDownloadingQueue()
{
    CurrentQueueState = EDownloadQueueState::Downloading;
    UpdateScrollBoxContent(DownloadQueueContainer.ToSharedRef());
	
	DownloadButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadListButtonClicked"));
	CompleteButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadListButtonNormal"));
}

// The completed queue is displayed 显示已完成队列
void SProjectWidget::ShowCompletedQueue()
{
    CurrentQueueState = EDownloadQueueState::Completed;
    UpdateScrollBoxContent(CompletedDownloadContainer.ToSharedRef());
	
	DownloadButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadListButtonNormal"));
	CompleteButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.DownloadListButtonClicked"));
}

// Add the file to DownloadCompleteWidget when the download is complete 在下载完成时将文件添加到 DownloadCompleteWidget
void SProjectWidget::OnDownloadComplete(const FString& InFileName, const FString& InFilePath, const FString& FileSizeText )
{
	if (DownloadCompleteWidget.IsValid())
	{
		DownloadCompleteWidget->AddCompletedDownload(InFileName, InFilePath, FileSizeText );
	}
}


// Close the download queue window 关闭下载队列窗口
void SProjectWidget::OnDownloadQueueWindowClosed(const TSharedRef<SWindow>& ClosedWindow)
{
    DownloadQueueWindow.Reset();
}


/*+++++++++++++++++++++++++Multitask download related  多任务下载相关+++++++++++++++++++++++++*/

void SProjectWidget::CleanInvalidPausedTasks()
{
	FScopeLock PausedTasksLock(&SAssetDownloadWidget::PausedTasksMutex);

	// Use a local array to store the index to be deleted 使用局部数组存储要删除的索引
	TArray<int32> ToRemoveIndices;

	// Iterate over the suspended task to check whether each weak reference is valid 遍历已暂停任务，检查每个弱引用是否有效
	for (int32 i = SAssetDownloadWidget::PausedTasks.Num() - 1; i >= 0; --i)
	{
		TWeakPtr<SAssetDownloadWidget> DownloadWidgetWeakPtr = SAssetDownloadWidget::PausedTasks[i];
		if (DownloadWidgetWeakPtr.Pin() == nullptr)  // Use Pin() to check if the weak reference is valid 使用 Pin() 检查弱引用是否有效
			{
			ToRemoveIndices.Add(i);  // Adds an invalid index to the removal list 将无效的索引添加到移除列表
			}
	}

	// Perform the removal of invalid elements 执行移除无效元素
	for (int32 Index : ToRemoveIndices)
	{
		SAssetDownloadWidget::PausedTasks.RemoveAt(Index);
	}
}


void SProjectWidget::StartAllDownloads()
{
	{
		FScopeLock PendingTasksLock(&SAssetDownloadWidget::PendingTasksMutex);

		TSharedPtr<SAssetDownloadWidget> PendingWidget;
		while (SAssetDownloadWidget::PendingTasks.Dequeue(PendingWidget))
		{
			if (PendingWidget.IsValid())
			{
				PendingWidget->OnStartOrResumeClicked();
			}
		}
	}
	
	{
		FScopeLock PausedTasksLock(&SAssetDownloadWidget::PausedTasksMutex);
		
		TArray<int32> ToRemoveIndices;

		for (int32 i = SAssetDownloadWidget::PausedTasks.Num() - 1; i >= 0; --i)
		{
			TWeakPtr<SAssetDownloadWidget> DownloadWidgetWeakPtr = SAssetDownloadWidget::PausedTasks[i];
			if (auto DownloadWidget = DownloadWidgetWeakPtr.Pin())
			{
				DownloadWidget->OnStartOrResumeClicked();
				ToRemoveIndices.Add(i);
			}
			else
			{
				ToRemoveIndices.Add(i);  // An invalid index was recorded 记录无效的索引
			}
		}

		// Removes invalid or started tasks from PausedTasks 从 PausedTasks 中删除无效或已启动的任务
		for (int32 Index : ToRemoveIndices)
		{
			SAssetDownloadWidget::PausedTasks.RemoveAt(Index);
		}
	}
}

// Improve PauseAllDownloads to ensure validity checks are performed before each visit 改进 PauseAllDownloads，确保每次访问前进行有效性检查
void SProjectWidget::PauseAllDownloads()
{
	{
		FScopeLock ActiveTasksLock(&SAssetDownloadWidget::ActiveTasksMutex);

		TArray<TWeakPtr<SAssetDownloadWidget>> ToPauseTasks;

		for (int32 i = SAssetDownloadWidget::ActiveTasks.Num() - 1; i >= 0; --i)
		{
			TWeakPtr<SAssetDownloadWidget> DownloadWidgetWeakPtr = SAssetDownloadWidget::ActiveTasks[i];
			if (TSharedPtr<SAssetDownloadWidget> DownloadWidgetSharedPtr = DownloadWidgetWeakPtr.Pin())
			{
				DownloadWidgetSharedPtr->SetPausedAllDownloads(true);
				DownloadWidgetSharedPtr->OnPauseClicked();

				if (!SAssetDownloadWidget::PausedTasks.Contains(DownloadWidgetWeakPtr))
				{
					SAssetDownloadWidget::PausedTasks.Add(DownloadWidgetWeakPtr);
				}
			}
		}

		SAssetDownloadWidget::ActiveTasks.Empty();
	}

	CleanInvalidPausedTasks(); 
}


void SProjectWidget::CancelAllDownloads()
{
	{
		FScopeLock ActiveTasksLock(&SAssetDownloadWidget::ActiveTasksMutex);
        
		for (int32 i = SAssetDownloadWidget::ActiveTasks.Num() - 1; i >= 0; --i)
		{
			TWeakPtr<SAssetDownloadWidget> DownloadWidgetWeakPtr = SAssetDownloadWidget::ActiveTasks[i];
			if (TSharedPtr<SAssetDownloadWidget> DownloadWidgetSharedPtr = DownloadWidgetWeakPtr.Pin())
			{
				DownloadWidgetSharedPtr->OnCancelClicked();
			}
		}

		SAssetDownloadWidget::ActiveTasks.Empty();
	}
	{
		FScopeLock PendingTasksLock(&SAssetDownloadWidget::PendingTasksMutex);
        
		TSharedPtr<SAssetDownloadWidget> PendingWidget;
		while (SAssetDownloadWidget::PendingTasks.Dequeue(PendingWidget))
		{
			if (PendingWidget.IsValid())
			{
				PendingWidget->OnCancelClicked();
			}
		}
	}
	{
		FScopeLock Lock(&SAssetDownloadWidget::PendingTasksMutex);
		for (auto& DownloadWidgetWeakPtr : SAssetDownloadWidget::PausedTasks)
		{
			if (auto DownloadWidget = DownloadWidgetWeakPtr.Pin())
			{
				DownloadWidget->OnCancelClicked();
			}
		}
		SAssetDownloadWidget::PausedTasks.Empty();
	}
	{
		FScopeLock Lock(&SAssetDownloadWidget::PendingTasksMutex);
		for (auto& DownloadWidgetSharedPtr : SAssetDownloadWidget::FailedTasks) 
			{
			if (DownloadWidgetSharedPtr.IsValid())
			{
				DownloadWidgetSharedPtr->OnCancelClicked();
			}
			}
		SAssetDownloadWidget::FailedTasks.Empty();
	}

}


void SProjectWidget::CloseAllOpenedWindows()
{
    TArray<TSharedRef<SWindow>> AllWindows;

    // Get root window 获取根窗口
    const TArray<TSharedRef<SWindow>>& RootWindows = FSlateApplication::Get().GetTopLevelWindows();
    for (const TSharedRef<SWindow>& RootWindow : RootWindows)
    {
        // Gets all Windows recursively, including minimized Windows 递归获取所有窗口，包括最小化的窗口
        GetAllWindowsRecursive(AllWindows, RootWindow);
    }

    for (const TSharedRef<SWindow>& Window : AllWindows)
    {
        FText WindowTitle = Window->GetTitle();

        // Determine whether the plug-in is related to the window 判断是否是插件相关窗口
        if (WindowTitle.EqualTo(FText::FromString(TEXT("传输列表")))
			|| WindowTitle.EqualTo(FText::FromString(TEXT("提示")))
            || WindowTitle.EqualTo(FText::FromString(TEXT("Video Player")))
            || WindowTitle.EqualTo(FText::FromString(TEXT("Audio Player")))
            || WindowTitle.EqualTo(FText::FromString(TEXT("Image Preview")))
            || WindowTitle.EqualTo(FText::FromString(TEXT("Download List")))
            || WindowTitle.EqualTo(FText::FromString(TEXT("Tip")))) 
            	
        {
            // UE_LOG(LogTemp, Log, TEXT("Closing plugin-related window: %s"), *WindowTitle.ToString());

            FSlateApplication::Get().RequestDestroyWindow(Window);
        }
    }
}

void SProjectWidget::GetAllWindowsRecursive(TArray<TSharedRef<SWindow>>& OutWindows, TSharedRef<SWindow> CurrentWindow)
{
	// Add the current window (whether visible or minimized) 添加当前窗口（无论是否可见或最小化）
	OutWindows.Add(CurrentWindow);

	// Traverse the child Windows of the current window 遍历当前窗口的子窗口
	const TArray<TSharedRef<SWindow>>& WindowChildren = CurrentWindow->GetChildWindows();
	for (int32 ChildIndex = 0; ChildIndex < WindowChildren.Num(); ++ChildIndex)
	{
		GetAllWindowsRecursive(OutWindows, WindowChildren[ChildIndex]);
	}
}





#undef LOCTEXT_NAMESPACE