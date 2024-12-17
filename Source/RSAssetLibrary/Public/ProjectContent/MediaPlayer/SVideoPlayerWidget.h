// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "SlateBasics.h"
// #include "MediaSoundComponent.h"
#include "SVideoPlayerWidget.generated.h"

class UMediaPlayerHandler;
class UMediaPlayerHandler2;


class FMediaPlayerManager
{
public:

	static void PlayMediaWithSystemPlayer(const FString& InPath);

	static void PlayAudioWithSystemPlayer(const FString& InPath);

	static void ShowImageInWindow(const FString& ImagePath);
};


class UMediaSoundComponent;
class SVideoPlayerWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SVideoPlayerWidget) {}
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs);

	~SVideoPlayerWidget();


	void PlayVideo(const FString& VideoURL);


	FReply OnVideoPlayPauseClicked();

	void OnMouseCaptureBegin();
	void OnMouseCaptureEnd();

	
	void OnSliderValueChanged(float NewValue);

	void OnMediaEndReached();

	void UpdateProgressBar();

	void UpdateAudioAndProgress();

	void StopPlay();

	void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

private:
	
	UMediaPlayer* MediaPlayer = nullptr;

	UMediaTexture* MediaTexture = nullptr;
	
	UMediaSoundComponent* MediaSound = nullptr;
	
	FTimerHandle ProgressBarTimerHandle; 

	bool bIsSliderBeingDragged = false;

	UMediaPlayerHandler* VideoHandler = nullptr;
	
	bool bIsPlaying;

	bool bISVideoPlayer = true;

	TSharedPtr<SImage> VideoImage;

	TSharedPtr<SBox> ImageBox; 

	TSharedPtr<SButton> PlayPauseButton;

	TSharedPtr<SSlider> ProgressBar;

	FButtonStyle PlayerButtonStyle;
};

class SAudioPlayerWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAudioPlayerWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	~SAudioPlayerWidget();

	void PlayVideo(const FString& VideoURL);

	FReply OnPlayPauseClicked();

	void OnSliderValueChanged(float NewValue);

	void UpdateProgressBar();

	void StopPlay();
	
	void OnMediaEndReached();

	void OnMouseCaptureBegin();

	void OnMouseCaptureEnd();

	void UpdateAudioAndProgress();

private:
	UMediaPlayer* MediaPlayer;

	UMediaPlayerHandler2* AudioHandler = nullptr;

	UMediaSoundComponent* MediaSound = nullptr;

	bool bIsPlaying;

	bool bISVideoPlayer = true;

	TSharedPtr<SImage> AudioImage;

	TSharedPtr<SBox> ImageBox; 

	TSharedPtr<SButton> PlayPauseButton;

	TSharedPtr<SSlider> ProgressBar;

	FButtonStyle PlayerButtonStyle;

	FTimerHandle ProgressBarTimerHandle;

	bool bIsSliderBeingDragged = false;
};


UCLASS()
class UMediaPlayerHandler : public UObject
{
	GENERATED_BODY()

public:
	
	void Initialize(UMediaPlayer* InMediaPlayer, TWeakPtr<SVideoPlayerWidget> InVideoPlayerWidget)
	{
		MediaPlayer = InMediaPlayer;
		VideoPlayerWidget = InVideoPlayerWidget;

		if (MediaPlayer)
		{
			MediaPlayer->OnEndReached.AddDynamic(this, &UMediaPlayerHandler::HandleMediaEndReached);
		}
	}
	
	UFUNCTION()
	void HandleMediaEndReached()
	{
		if (TSharedPtr<SVideoPlayerWidget> Widget = VideoPlayerWidget.Pin())
		{
			Widget->OnMediaEndReached();
		}
	}

private:
	UPROPERTY()
	UMediaPlayer* MediaPlayer;

	TWeakPtr<SVideoPlayerWidget> VideoPlayerWidget;
};

UCLASS()
class UMediaPlayerHandler2 : public UObject
{
	GENERATED_BODY()

public:
	
	void Initialize(UMediaPlayer* InMediaPlayer, TWeakPtr<SAudioPlayerWidget> InVideoPlayerWidget)
	{
		MediaPlayer = InMediaPlayer;
		VideoPlayerWidget = InVideoPlayerWidget;

		if (MediaPlayer)
		{
			MediaPlayer->OnEndReached.AddDynamic(this, &UMediaPlayerHandler2::HandleMediaEndReached);
		}
	}

	UFUNCTION()
	void HandleMediaEndReached()
	{
		if (TSharedPtr<SAudioPlayerWidget> Widget = VideoPlayerWidget.Pin())
		{
			Widget->OnMediaEndReached();
		}
	}

private:
	UPROPERTY()
	UMediaPlayer* MediaPlayer;

	TWeakPtr<SAudioPlayerWidget> VideoPlayerWidget;
};