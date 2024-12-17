// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMainWidget.h"
#include "ProjectContent/SProjectWidget.h"
#include "Widgets/SCompoundWidget.h"


struct FQrLoginResponseData;
class SProjectWidget;
class UQrLoginApi;


class SLoginWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoginWidget) {}
	SLATE_EVENT(FOnLoginSuccess, OnLoginSuccess)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	virtual ~SLoginWidget() override;

	void RestartRolling();

	void ResetLoginMsg();

private:

	FReply OnGetCaptchaClicked();
	FReply OnLoginButtonClicked();

	TSharedPtr<class SEditableTextBox> PhoneNumberTextBox;
	TSharedPtr<class SEditableTextBox> VerificationCodeTextBox;
	TSharedPtr<SCheckBox> AgreementCheckBox;
	TSharedPtr<SButton> GetCaptchaButton;
	TSharedPtr<SButton> SwitchLoginButton;
	TSharedPtr<SProjectWidget> ProjectWidget;
	
	TSharedPtr<SBox> ContentBar;

	bool bIsPhoneNumberEmpty = false;
	bool bIsVerificationCodeEmpty = false;
	bool bIsAgreementUnchecked = false;

	int32 CountdownTime = 60;
	bool bIsCountdownActive = false;

	FTimerHandle CountdownTimerHandle;

	FButtonStyle LoginButtonStyle;

	FButtonStyle QRLoginButtonStyle;
	FButtonStyle AccountLoginButtonStyle;
	

	void UpdateCountdown();
	
	TSharedRef<SWidget> CreateLoginUI();
	
	TSharedRef<SWidget> CreateQRCodeLoginUI();

	TSharedRef<SWidget> CreateAccountLoginUI();

	TSharedPtr<class STextBlock> QrCodeStatusTextBlock;

	FReply OnSwitchLoginClicked();

	void OnUserAgreementClicked();

	void UpdateContentBar(TSharedRef<SWidget> NewContent);

	bool bIsQRUI = false;

	void HandleQrCodeImageReady(const TArray<uint8>& ImageData);
	
	UTexture2D* CreateTextureFromImage(const TArray<uint8>& ImageData);

	TSharedPtr<SImage> QrCodeImage;

	TSharedPtr<STextBlock> LoginStatusMessageText;

	void HandleQrCodeStateChanged(const FQrLoginResponseData& QrCodeState);


private:
	FOnLoginSuccess OnLoginSuccess;

	TObjectPtr<UQrLoginApi> CreateQrApi;

	int32 TestSessionTimeInSeconds = 100000;

	bool bIsQrCodeValid = false;

	TSharedPtr<SButton> LoginButton;

	float LastClickTime = 0.0f;
	const float ClickCooldownTime = 1.0f;
	
};
