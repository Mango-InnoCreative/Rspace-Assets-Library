// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectContent/ConceptDesign/ConceptDesignDisplay.h"
#include "ProjectContent/Imageload/FImageLoader.h"
#include "Brushes/SlateImageBrush.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "SImageDisplayWindow"

void SImageDisplayWindow::Construct(const FArguments& InArgs)
{
    ImagePath = InArgs._ImagePath;
    CurrentZoom = 1.0f;
    LoadedTexture = nullptr;
    CurrentOffset = FVector2D::ZeroVector;
    bIsDragging = false;

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            SNew(SBorder)
            .BorderImage(nullptr)
            .Padding(FMargin(10.0f))
            .Clipping(EWidgetClipping::ClipToBounds) // Enable cropping to prevent images from overshooting 启用剪裁，防止图片超出
            [
                SAssignNew(ImageBox, SBox)
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Loading Image..."))
                    .Justification(ETextJustify::Center)
                ]
            ]
        ]
    ];
    LoadImage(ImagePath);
}



void SImageDisplayWindow::LoadImage(const FString& ProjectImageUrl)
{
    if (ImageBox.IsValid())
    {
        // 设置占位符内容，避免显示旧的错误信息
        ImageBox->SetContent(
            SNew(STextBlock)
            .Text(LOCTEXT("Loading", "加载中..."))
            .Justification(ETextJustify::Center)
        );
    }

    if (ProjectImageUrl.IsEmpty())
    {
        ShowErrorMessage(LOCTEXT("NoPreview", "无预览图"));
        return;
    }

    FImageLoader::LoadImageFromUrl(ProjectImageUrl, FOnProjectImageReady::CreateLambda([this, ProjectImageUrl](const TArray<uint8>& ImageData)
    {
        if (ImageData.Num() > 0)
        {
            LoadedTexture = FImageLoader::CreateTextureFromBytes(ImageData);
            if (LoadedTexture && ImageBox.IsValid())
            {
                UpdateImageDisplay();
            }
            else
            {
                ShowErrorMessage(LOCTEXT("LoadFailed", "加载失败"));
            }
        }
        else
        {
            // ShowErrorMessage(FString::Printf(TEXT("Failed to download image from URL: %s"), *ProjectImageUrl));
            ShowErrorMessage(LOCTEXT("LoadFailed", "加载失败"));
        }
    }));
}


void SImageDisplayWindow::ShowErrorMessage(const FText& ErrorMessage)
{
    if (ImageBox.IsValid())
    {
        ImageBox->SetContent(
            SNew(STextBlock)
            .Text(ErrorMessage)
            .Justification(ETextJustify::Center)
        );
    }
}


void SImageDisplayWindow::UpdateImageDisplay()
{
    if (!LoadedTexture || !ImageBox.IsValid())
    {
        // ShowErrorMessage(TEXT("Failed to display image"));
        ShowErrorMessage(LOCTEXT("LoadFailed", "加载失败"));
        return;
    }

    FVector2D ImageSize(LoadedTexture->GetSizeX(), LoadedTexture->GetSizeY());
    ImageBrush = MakeShareable(new FSlateImageBrush(LoadedTexture, ImageSize)); 
    
    if (ImageBox.IsValid() && ImageBrush.IsValid())
    {
        ImageBox->SetContent(
            SNew(SScaleBox)
            .Stretch(EStretch::ScaleToFit)
            .StretchDirection(EStretchDirection::Both)
            [
                SNew(SBox)
                .Padding(10)
                [
                    SNew(SImage)
                    .Image(ImageBrush.Get()) 
                ]
            ]
        );
        ImageBox->Invalidate(EInvalidateWidget::Layout); 
    }
    else
    {
        ShowErrorMessage(LOCTEXT("LoadFailed", "加载失败"));
    }
}



FReply SImageDisplayWindow::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    const float Delta = MouseEvent.GetWheelDelta();
    CurrentZoom = FMath::Clamp(CurrentZoom + Delta * 0.1f, 0.1f, 5.0f); 

    if (ImageBox.IsValid() && LoadedTexture && ImageBrush.IsValid())
    {
        FVector2D NewImageSize(LoadedTexture->GetSizeX() * CurrentZoom, LoadedTexture->GetSizeY() * CurrentZoom);
        
        ImageBrush->ImageSize = NewImageSize;
        
        ImageBox->SetContent(
            SNew(SScaleBox)
            .Stretch(EStretch::None)
            [
                SNew(SBox)
                .Padding(FMargin(CurrentOffset.X, CurrentOffset.Y, 0, 0))
                [
                    SNew(SImage)
                    .Image(ImageBrush.Get()) 
                ]
            ]
        );
        
        ImageBox->Invalidate(EInvalidateWidget::Layout);
    }

    return FReply::Handled();
}



FReply SImageDisplayWindow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsDragging = true; 
        DragStartPosition = MouseEvent.GetScreenSpacePosition();
        return FReply::Handled().CaptureMouse(SharedThis(this));
    }

    return FReply::Unhandled();
}

FReply SImageDisplayWindow::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (bIsDragging)
    {
        FVector2D DragDelta = MouseEvent.GetScreenSpacePosition() - DragStartPosition;
        DragStartPosition = MouseEvent.GetScreenSpacePosition(); 
        
        CurrentOffset += DragDelta;
        
        if (ImageBox.IsValid())
        {
            ImageBox->SetContent(
                SNew(SScaleBox)
                .Stretch(EStretch::None)
                [
                    SNew(SBox)
                    .Padding(FMargin(CurrentOffset.X, CurrentOffset.Y, 0, 0)) 
                    [
                        SNew(SImage)
                        .Image(ImageBrush.Get()) 
                    ]
                ]
            );

            ImageBox->Invalidate(EInvalidateWidget::Layout);
        }

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply SImageDisplayWindow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsDragging)
    {
        bIsDragging = false; 
        return FReply::Handled().ReleaseMouseCapture();
    }

    return FReply::Unhandled();
}




#undef LOCTEXT_NAMESPACE

