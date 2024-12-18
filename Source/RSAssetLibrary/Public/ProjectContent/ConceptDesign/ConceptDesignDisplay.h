// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Brushes/SlateImageBrush.h"
#include "Widgets/DeclarativeSyntaxSupport.h"


class SImageDisplayWindow : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SImageDisplayWindow)
        : _ImagePath(TEXT(""))
    {}
    SLATE_ARGUMENT(FString, ImagePath)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs);
    

    void LoadImage(const FString& ProjectImageUrl);


    void ShowErrorMessage(const FText& ErrorMessage);


    void UpdateImageDisplay();


    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

    FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    
    FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);



private:

    FString ImagePath;


    float CurrentZoom;


    UTexture2D* LoadedTexture;


    TSharedPtr<FSlateImageBrush> ImageBrush;


    TSharedPtr<SBox> ImageBox;

private:
    FVector2D CurrentOffset; 
    FVector2D DragStartPosition;
    bool bIsDragging; 

};
