// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"

class SProjectWidget;
class SLoginWidget;

DECLARE_DELEGATE(FOnLoginSuccess);

class SMainWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMainWidget) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:

	TSharedPtr<SBox> ContentBox;


	TSharedPtr<SLoginWidget> CachedLoginWidget;
	TSharedPtr<SProjectWidget> CachedProjectWidget;


	TSharedPtr<SCompoundWidget> CurrentWidget;

public:

	void ShowProjectWidget();


	void ShowLoginWidget();


	void OnLogout();


	TSharedPtr<SCompoundWidget> GetCurrentWidget() const;
};
