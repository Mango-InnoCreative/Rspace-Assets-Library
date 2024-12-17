// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectContent/ProjectList/SProjectlistWidget.h"
#include "RSAssetLibraryStyle.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"


void SProjectlistWidget::Construct(const FArguments& InArgs)
{
	UserAndProjectInfo = InArgs._UserAndProjectInfo;
	OnProjectSwitchedDelegate = InArgs._OnProjectSwitched;
	
	ChildSlot
	[
	
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)
		
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				GenerateProjectList(UserAndProjectInfo.ProjectItems.items)
			]
		]
	];
}

TSharedRef<SWidget> SProjectlistWidget::GenerateProjectList(const TArray<FProjectItem>& ProjectItems)
{
    // Gets the currently selected item 获取当前选中的项目
    UUSMSubsystem* USMSubsystem = GEditor->GetEditorSubsystem<UUSMSubsystem>();
    const FProjectItem& SelectedProject = USMSubsystem ? USMSubsystem->GetSelectedProject() : FProjectItem();
	
    TSharedRef<SVerticalBox> ProjectList = SNew(SVerticalBox);
    FSlateFontInfo ProjectListFont = FCoreStyle::GetDefaultFontStyle("Regular", 12); 

    for (const FProjectItem& ProjectItem : ProjectItems)
    {
        TSharedPtr<STextBlock> ProjectTextBlock;
        TSharedPtr<bool> bIsButtonHovered = MakeShared<bool>(false);
        TSharedPtr<bool> bIsButtonClicked = MakeShared<bool>(false);

        // Create a FSlateColor object to control the text color 创建一个 FSlateColor 对象来控制文字颜色
        TSharedPtr<FSlateColor> TextColor = MakeShared<FSlateColor>();

        // If the current project name is the same as the selected project name, set it to green 如果当前项目名称与选中的项目名称相同，设置为绿色
        if (ProjectItem.projectName == SelectedProject.projectName)
        {
            *TextColor = FSlateColor(FLinearColor(0.03922f, 0.55686f, 0.40784f, 1.0f));  
        }
        else
        {
            *TextColor = FSlateColor(FLinearColor::White);  
        }

        ProjectList->AddSlot()
        .AutoHeight()
        .Padding(FMargin(5.0f))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            .Padding(FMargin(15.0f, 0.0f, 15.0f, 10.0f)) 
            [
                SNew(SButton)
                .Cursor(EMouseCursor::Hand)
                .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder"))
                .ContentPadding(10) 
                .OnClicked_Lambda([this, ProjectItem, bIsButtonClicked](){
                    *bIsButtonClicked = true;
                    return OnProjectClicked(ProjectItem);
                })
                .OnHovered_Lambda([bIsButtonHovered, TextColor](){
                    *bIsButtonHovered = true;
                    // If the button is hovered over, the color remains green 如果按钮被悬停，颜色仍然保持绿色
                    if (*TextColor != FSlateColor(FLinearColor(0.03922f, 0.55686f, 0.40784f, 1.0f)))
                    {
                        *TextColor = FSlateColor(FLinearColor(0.03922f, 0.55686f, 0.40785f, 1.0f));  
                    }
                })
                .OnUnhovered_Lambda([bIsButtonHovered, TextColor](){
                    *bIsButtonHovered = false;
                    // At the end of the hover, if the button is not the selected item, it returns to white 悬停结束后，如果按钮不是选中的项目，则恢复为白色
                    if (*TextColor != FSlateColor(FLinearColor(0.03922f, 0.55686f, 0.40784f, 1.0f)))
                    {
                        *TextColor = FSlateColor(FLinearColor::White);
                    }
                })
                [
                    SAssignNew(ProjectTextBlock, STextBlock)
                    .Text(FText::FromString(TruncateText(ProjectItem.projectName, 22)))
                    .Font(ProjectListFont)
                    .Justification(ETextJustify::Left)
                    .ColorAndOpacity_Lambda([TextColor]() -> FSlateColor
                    {
                        // Returns the stored text color 返回存储的文字颜色
                        return *TextColor;
                    })
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(FMargin(20.0f, 0.0f, 20.0f, 0.0f)) 
            [
                SNew(SBox)
                [
                    SNew(SImage)
                    .Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.ProjectLine")) 
                ]
            ]
        ];
    }

    return ProjectList;
}

FString SProjectlistWidget::TruncateText(const FString& OriginalText, int32 MaxLength)
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


FReply SProjectlistWidget::OnProjectClicked(FProjectItem ClickedProject)
{

	UUSMSubsystem* USMSubsystem = GEditor->GetEditorSubsystem<UUSMSubsystem>();
	USMSubsystem->SetSelectedProject(ClickedProject);

	// Triggers the agent to notify the parent component of the project switch 触发代理，通知父组件项目切换
	if (OnProjectSwitchedDelegate.IsBound())
	{
		OnProjectSwitchedDelegate.Execute();
	}

	// Gets the current window and destroys it 
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
	if (ParentWindow.IsValid())
	{
		ParentWindow->RequestDestroyWindow(); 
	}

	return FReply::Handled();
}
