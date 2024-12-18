// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "ProjectContent/MediaPlayer/SVideoPlayerWidget.h"
#include "Widgets/Images/SImage.h"
#include "Brushes/SlateImageBrush.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "AudioParameter.h"
#include "RSAssetLibraryStyle.h"
#include "Engine/Engine.h"
#include "ProjectContent/ConceptDesign/ConceptDesignDisplay.h"

namespace Audio
{
    struct FCaptureDeviceInfo;
}

void FMediaPlayerManager::PlayMediaWithSystemPlayer(const FString& InPath)
{
    static TWeakPtr<SWindow> ExistingPlayerWindow;
    static TSharedPtr<SVideoPlayerWidget> SharedVideoPlayerWidget;

    if (ExistingPlayerWindow.IsValid())
    {
        TSharedPtr<SWindow> PlayerWindow = ExistingPlayerWindow.Pin();
        if (PlayerWindow.IsValid())
        {
            PlayerWindow->BringToFront();
            if (SharedVideoPlayerWidget.IsValid())
            {
                SharedVideoPlayerWidget->PlayVideo(InPath);
            }
            return;
        }
    }

    if (!SharedVideoPlayerWidget.IsValid())
    {
        SharedVideoPlayerWidget = SNew(SVideoPlayerWidget);
    }
    SharedVideoPlayerWidget->PlayVideo(InPath);

    TSharedRef<SWindow> VideoPlayerWindow = SNew(SWindow)
        .Title(FText::FromString(TEXT("Video Player")))
        .ClientSize(FVector2D(1280, 720))
        .SupportsMaximize(true)
        .SupportsMinimize(true)
        .SizingRule(ESizingRule::UserSized)
        [
            SNew(SBox)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            [
                SharedVideoPlayerWidget.ToSharedRef()
            ]
        ];

    FWindowSizeLimits SizeLimits;
    SizeLimits.SetMinWidth(640.0f);
    SizeLimits.SetMinHeight(360.0f);
    VideoPlayerWindow->SetSizeLimits(SizeLimits);

    VideoPlayerWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([](const TSharedRef<SWindow>& ClosedWindow)
    {
        if (SharedVideoPlayerWidget.IsValid())
        {
            SharedVideoPlayerWidget->StopPlay();
            SharedVideoPlayerWidget.Reset();
        }
        ExistingPlayerWindow.Reset();
    }));

    ExistingPlayerWindow = VideoPlayerWindow;

    FSlateApplication::Get().AddWindow(VideoPlayerWindow);
}

void FMediaPlayerManager::PlayAudioWithSystemPlayer(const FString& InPath)
{
    static TWeakPtr<SWindow> ExistingAudioWindow;
    static TSharedPtr<SAudioPlayerWidget> SharedAudioPlayerWidget;

    if (ExistingAudioWindow.IsValid())
    {
        TSharedPtr<SWindow> AudioPlayerWindow = ExistingAudioWindow.Pin();
        if (AudioPlayerWindow.IsValid())
        {
            AudioPlayerWindow->BringToFront();
            if (SharedAudioPlayerWidget.IsValid())
            {
                SharedAudioPlayerWidget->PlayVideo(InPath);
            }
            return;
        }
    }

    if (!SharedAudioPlayerWidget.IsValid())
    {
        SharedAudioPlayerWidget = SNew(SAudioPlayerWidget);
    }
    SharedAudioPlayerWidget->PlayVideo(InPath);

    TSharedRef<SWindow> AudioPlayerWindow = SNew(SWindow)
        .Title(FText::FromString(TEXT("Audio Player")))
        .ClientSize(FVector2D(530, 360))
        .SupportsMaximize(true)
        .SupportsMinimize(true)
        .SizingRule(ESizingRule::FixedSize)
        [
            SNew(SBox)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            [
                SharedAudioPlayerWidget.ToSharedRef()
            ]
        ];
    
    FWindowSizeLimits SizeLimits;
    SizeLimits.SetMinWidth(530.0f);
    SizeLimits.SetMinHeight(360.0f);
    SizeLimits.SetMaxWidth(530.0f); 
    SizeLimits.SetMaxHeight(360.0f);
    AudioPlayerWindow->SetSizeLimits(SizeLimits);

    AudioPlayerWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([](const TSharedRef<SWindow>& ClosedWindow)
    {
        if (SharedAudioPlayerWidget.IsValid())
        {
            SharedAudioPlayerWidget->StopPlay();
            SharedAudioPlayerWidget.Reset();
        }
        ExistingAudioWindow.Reset();
    }));

    ExistingAudioWindow = AudioPlayerWindow;

    FSlateApplication::Get().AddWindow(AudioPlayerWindow);

}

void FMediaPlayerManager::ShowImageInWindow(const FString& ImagePath)
{
    static TWeakPtr<SWindow> ExistingImageWindow;
    static TSharedPtr<SImageDisplayWindow> SharedImageDisplayWidget;

    if (ExistingImageWindow.IsValid())
    {
        TSharedPtr<SWindow> ImageWindow = ExistingImageWindow.Pin();
        if (ImageWindow.IsValid())
        {
            ImageWindow->BringToFront(); 
            if (SharedImageDisplayWidget.IsValid())
            {
                SharedImageDisplayWidget->LoadImage(ImagePath); 
            }
            return;
        }
    }

    if (!SharedImageDisplayWidget.IsValid())
    {
        SharedImageDisplayWidget = SNew(SImageDisplayWindow);
    }
    SharedImageDisplayWidget->LoadImage(ImagePath);

    TSharedRef<SWindow> ImageWindow = SNew(SWindow)
        .Title(FText::FromString(TEXT("Image Preview")))
        .AutoCenter(EAutoCenter::PreferredWorkArea) 
        .SizingRule(ESizingRule::UserSized)         
        .SupportsMaximize(true)
        .SupportsMinimize(true)
        .ClientSize(FVector2D(1280, 720))          
        [
            SharedImageDisplayWidget.ToSharedRef()
        ];

  
    FWindowSizeLimits SizeLimits;
    SizeLimits.SetMinWidth(640.0f);  
    SizeLimits.SetMinHeight(360.0f);
    ImageWindow->SetSizeLimits(SizeLimits);

    ImageWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([](const TSharedRef<SWindow>& ClosedWindow)
    {
        if (SharedImageDisplayWidget.IsValid())
        {
            SharedImageDisplayWidget.Reset(); 
        }
        ExistingImageWindow.Reset(); 
    }));

    ExistingImageWindow = ImageWindow;

    FSlateApplication::Get().AddWindow(ImageWindow);
}



void SVideoPlayerWidget::Construct(const FArguments& InArgs)
{
    PlayerButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginIcon"));
    PlayerButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
    PlayerButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
    
    MediaPlayer = NewObject<UMediaPlayer>();
    MediaPlayer->AddToRoot();
    MediaTexture = NewObject<UMediaTexture>();
  
    MediaTexture->SetMediaPlayer(MediaPlayer);

    MediaSound = NewObject<UMediaSoundComponent>();
    MediaSound->SetMediaPlayer(MediaPlayer);
    MediaSound->Initialize();
    MediaSound->AddToRoot();
    MediaSound->bIsUISound = true;
    MediaSound->bIsPreviewSound = true;
    MediaSound->SetVolumeMultiplier(1.0f);
    MediaSound->bAutoActivate = true;
    MediaSound->Activate();
    bIsPlaying = false;
    
    VideoHandler = NewObject<UMediaPlayerHandler>();
    VideoHandler->Initialize(MediaPlayer, SharedThis(this));
  
    if (GEditor && GEditor->GetEditorWorldContext().World())
    {
        GEditor->GetEditorWorldContext().World()->GetTimerManager().SetTimer(
            ProgressBarTimerHandle,
            FTimerDelegate::CreateSP(this, &SVideoPlayerWidget::UpdateAudioAndProgress),
            0.01f, 
            true  
        );
    }
    else
    {
        //UE_LOG(LogTemp, Error, TEXT("Failed to initialize audio update timer!"));
    }
    
    
   
    ImageBox = SNew(SBox)
    .WidthOverride(1280)
    .HeightOverride(720); 
  
    VideoImage = SNew(SImage)
        .Image(new FSlateImageBrush(MediaTexture, FVector2D(1280, 720)));
    
    ImageBox->SetContent(VideoImage.ToSharedRef());


    PlayPauseButton = SNew(SButton)
        .ButtonStyle(&PlayerButtonStyle)
        .OnClicked(this, &SVideoPlayerWidget::OnVideoPlayPauseClicked);


    ProgressBar = SNew(SSlider)
        .Value(0.0f)
        .OnValueChanged(this, &SVideoPlayerWidget::OnSliderValueChanged)
        .OnMouseCaptureBegin(this, &SVideoPlayerWidget::OnMouseCaptureBegin)
        .OnMouseCaptureEnd(this, &SVideoPlayerWidget::OnMouseCaptureEnd);

    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        [
            ImageBox.ToSharedRef()
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5,5,5,5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0,0,5,0)
            [
                PlayPauseButton.ToSharedRef()
            ]
            
            + SHorizontalBox::Slot()
            .FillWidth(1)
           [
            ProgressBar.ToSharedRef()
           ]
            
        ]

    ];
}

SVideoPlayerWidget::~SVideoPlayerWidget()
{
    if (GEditor && GEditor->GetEditorWorldContext().World())
    {
        GEditor->GetEditorWorldContext().World()->GetTimerManager().ClearTimer(ProgressBarTimerHandle);
    }

}


void SVideoPlayerWidget::PlayVideo(const FString& VideoURL)
{
    if (MediaPlayer && MediaTexture)
    {
        if (MediaPlayer->OpenUrl(VideoURL))
        {
            MediaTexture->UpdateResource();
            MediaPlayer->Play();
            
            bIsPlaying = true;
            PlayerButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseIcon"));
            PlayerButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"));
            PlayerButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"));
        }
    }
}

FReply SVideoPlayerWidget::OnVideoPlayPauseClicked()
{
    if (MediaPlayer)
    {
        if (bIsPlaying)
        {
            MediaPlayer->Pause();
            PlayerButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginIcon"));
            PlayerButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
            PlayerButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
        }
        else
        {
            MediaPlayer->Play();
            PlayerButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseIcon"));
            PlayerButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"));
            PlayerButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"));
        }

        bIsPlaying = !bIsPlaying;
    }

    return FReply::Handled();
}


void SVideoPlayerWidget::OnMediaEndReached()
{

    if (MediaPlayer)
    {
        MediaPlayer->Pause();
    }


    if (ProgressBar.IsValid())
    {
        ProgressBar->SetValue(0.0f);
    }

    PlayerButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginIcon"));
    PlayerButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
    PlayerButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));

    bIsPlaying = false;

    //UE_LOG(LogTemp, Log, TEXT("Media playback completed, UI updated."));
}

void SVideoPlayerWidget::OnMouseCaptureBegin()
{
    bIsSliderBeingDragged = true;

    //UE_LOG(LogTemp, Warning, TEXT("开始拖动进度条-----"));


    if (MediaPlayer && MediaPlayer->IsPlaying())
    {
        MediaPlayer->Pause();
    }

    if (GEditor && GEditor->GetEditorWorldContext().World())
    {
        GEditor->GetEditorWorldContext().World()->GetTimerManager().ClearTimer(ProgressBarTimerHandle);
    }
}


void SVideoPlayerWidget::OnMouseCaptureEnd()
{
    bIsSliderBeingDragged = false;

    //UE_LOG(LogTemp, Warning, TEXT("松开拖动进度条！！！"));


    if (MediaPlayer)
    {
        MediaPlayer->Play();
    }

 
    if (GEditor && GEditor->GetEditorWorldContext().World())
    {
        GEditor->GetEditorWorldContext().World()->GetTimerManager().SetTimer(
            ProgressBarTimerHandle,
            FTimerDelegate::CreateSP(this, &SVideoPlayerWidget::UpdateProgressBar),
            0.01f, 
            true    
        );
    }
}

void SVideoPlayerWidget::UpdateAudioAndProgress()
{
    if (MediaSound != nullptr)
    {
        MediaSound->UpdatePlayer();
    }

    UpdateProgressBar(); 
}

void SVideoPlayerWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    
    if (!MediaPlayer || !MediaTexture || !ImageBox)
    {
        return; 
    }
    
    if (MediaPlayer->IsPlaying()) 
    {
        float AspectRatio = MediaPlayer->GetVideoTrackAspectRatio(0, 0);
        int32 VideoHeight = 720.0f;
        int32 VideoWidth = VideoHeight * AspectRatio;

        if (ImageBox)
        {
            ImageBox->SetWidthOverride(VideoWidth);
            ImageBox->SetHeightOverride(VideoHeight);
        }
    }
}


void SVideoPlayerWidget::UpdateProgressBar()
{
    if (!MediaPlayer || !MediaPlayer->IsPlaying())
    {
        return; 
    }

    if (MediaPlayer->GetDuration().GetTotalSeconds() > 0)
    {
        float CurrentTime = MediaPlayer->GetTime().GetTotalSeconds();
        float TotalTime = MediaPlayer->GetDuration().GetTotalSeconds();

        ProgressBar->SetValue(CurrentTime / TotalTime);
    }
}



void SVideoPlayerWidget::OnSliderValueChanged(float NewValue)
{
    if (MediaPlayer && MediaPlayer->GetDuration().GetTotalSeconds() > 0)
    {
        
        float NewTime = NewValue * MediaPlayer->GetDuration().GetTotalSeconds();
        MediaPlayer->Seek(FTimespan::FromSeconds(NewTime));

        //UE_LOG(LogTemp, Warning, TEXT("进度条位置更新到: %f 秒"), NewTime);
    }
}


void SVideoPlayerWidget::StopPlay()
{
    if (MediaPlayer)
    {
        MediaPlayer->Close();
    }

    if (MediaSound)
    {
        MediaSound->Stop();

    }
    
}



void SAudioPlayerWidget::Construct(const FArguments& InArgs)
{
    PlayerButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginIcon"));
    PlayerButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
    PlayerButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
    
    MediaPlayer = NewObject<UMediaPlayer>();
    MediaPlayer->AddToRoot();

    MediaSound = NewObject<UMediaSoundComponent>();
    MediaSound->SetMediaPlayer(MediaPlayer);
    MediaSound->Initialize();
    MediaSound->AddToRoot();
    MediaSound->bIsUISound = true;
    MediaSound->bIsPreviewSound = true;
    MediaSound->SetVolumeMultiplier(1.0f);
    MediaSound->bAutoActivate = true;
    MediaSound->Activate();
    bIsPlaying = false;
    
    AudioHandler = NewObject<UMediaPlayerHandler2>();
    AudioHandler->Initialize(MediaPlayer, SharedThis(this));

    if (GEditor && GEditor->GetEditorWorldContext().World())
    {
        GEditor->GetEditorWorldContext().World()->GetTimerManager().SetTimer(
            ProgressBarTimerHandle,
            FTimerDelegate::CreateSP(this, &SAudioPlayerWidget::UpdateAudioAndProgress),
            0.01f,
            true  
        );
    }
    else
    {
        //UE_LOG(LogTemp, Error, TEXT("Failed to initialize audio update timer!"));
    }
    
    ImageBox = SNew(SBox)
    .WidthOverride(585) 
    .HeightOverride(360); 
   
    AudioImage = SNew(SImage).Image(FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.AudioImage")); 
    
    ImageBox->SetContent(AudioImage.ToSharedRef());

    PlayPauseButton = SNew(SButton)
        .ButtonStyle(&PlayerButtonStyle)
        .OnClicked(this, &SAudioPlayerWidget::OnPlayPauseClicked);

    ProgressBar = SNew(SSlider)
        .Value(0.0f)
        .OnValueChanged(this, &SAudioPlayerWidget::OnSliderValueChanged);

    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        [
            ImageBox.ToSharedRef()
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5,5,5,5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0,0,5,0)
            [
                PlayPauseButton.ToSharedRef()
            ]
            
            + SHorizontalBox::Slot()
            .FillWidth(1)
           [
            ProgressBar.ToSharedRef()
           ]
            
        ]

    ];
}

SAudioPlayerWidget::~SAudioPlayerWidget()
{
    if (GEditor && GEditor->GetEditorWorldContext().World())
    {
        GEditor->GetEditorWorldContext().World()->GetTimerManager().ClearTimer(ProgressBarTimerHandle);
    }

}

void SAudioPlayerWidget::PlayVideo(const FString& VideoURL)
{
    if (MediaPlayer)
    {
        if (MediaPlayer->OpenUrl(VideoURL))
        {
            MediaPlayer->Play();
            
            bIsPlaying = true;
            PlayerButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseIcon"));
            PlayerButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"));
            PlayerButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"));
        }
    }
}

FReply SAudioPlayerWidget::OnPlayPauseClicked()
{
    if (MediaPlayer)
    {
        if (bIsPlaying)
        {
            MediaPlayer->Pause();
            PlayerButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginIcon"));
            PlayerButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
            PlayerButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
        }
        else
        {
            MediaPlayer->Play();
            PlayerButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseIcon"));
            PlayerButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"));
            PlayerButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.PauseClicked"));
        }

        bIsPlaying = !bIsPlaying;
    }

    return FReply::Handled();
}

void SAudioPlayerWidget::OnMediaEndReached()
{
   
    if (MediaPlayer)
    {
        MediaPlayer->Pause();
    }

    if (ProgressBar.IsValid())
    {
        ProgressBar->SetValue(0.0f);
    }

    PlayerButtonStyle.SetNormal(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginIcon"));
    PlayerButtonStyle.SetHovered(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));
    PlayerButtonStyle.SetPressed(*FRSAssetLibraryStyle::Get().GetBrush("RSAssetLibrary.BeginClicked"));

    bIsPlaying = false;

    //UE_LOG(LogTemp, Log, TEXT("Media playback completed, UI updated."));
}



void SAudioPlayerWidget::OnMouseCaptureBegin()
{
    bIsSliderBeingDragged = true;

    //UE_LOG(LogTemp, Warning, TEXT("开始拖动进度条-----"));

    if (MediaPlayer && MediaPlayer->IsPlaying())
    {
        MediaPlayer->Pause();
    }

    if (GEditor && GEditor->GetEditorWorldContext().World())
    {
        GEditor->GetEditorWorldContext().World()->GetTimerManager().ClearTimer(ProgressBarTimerHandle);
    }
}


void SAudioPlayerWidget::OnMouseCaptureEnd()
{
    bIsSliderBeingDragged = false;

    //UE_LOG(LogTemp, Warning, TEXT("松开拖动进度条！！！"));

    if (MediaPlayer)
    {
        MediaPlayer->Play();
    }

    if (GEditor && GEditor->GetEditorWorldContext().World())
    {
        GEditor->GetEditorWorldContext().World()->GetTimerManager().SetTimer(
            ProgressBarTimerHandle,
            FTimerDelegate::CreateSP(this, &SAudioPlayerWidget::UpdateProgressBar),
            0.01f,  
            true    
        );
    }
}

void SAudioPlayerWidget::UpdateAudioAndProgress()
{
    if (MediaSound != nullptr)
    {
        MediaSound->UpdatePlayer(); 
    }

    UpdateProgressBar(); 
}

void SAudioPlayerWidget::UpdateProgressBar()
{
    if (!MediaPlayer || !MediaPlayer->IsPlaying())
    {
        return; 
    }

    if (MediaPlayer->GetDuration().GetTotalSeconds() > 0)
    {
        float CurrentTime = MediaPlayer->GetTime().GetTotalSeconds();
        float TotalTime = MediaPlayer->GetDuration().GetTotalSeconds();

        ProgressBar->SetValue(CurrentTime / TotalTime);
    }
}

void SAudioPlayerWidget::OnSliderValueChanged(float NewValue)
{
    if (MediaPlayer && MediaPlayer->GetDuration().GetTotalSeconds() > 0)
    {
        
        float NewTime = NewValue * MediaPlayer->GetDuration().GetTotalSeconds();
        MediaPlayer->Seek(FTimespan::FromSeconds(NewTime));
    }
}

void SAudioPlayerWidget::StopPlay()
{
    if (MediaPlayer)
    {
        MediaPlayer->Close();
    }

    if (MediaSound)
    {
        MediaSound->Stop();
    }
}