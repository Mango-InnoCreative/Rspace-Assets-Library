// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "RSAssetLibraryStyle.h"
#include "RSAssetLibrary.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FRSAssetLibraryStyle::StyleInstance = nullptr;

void FRSAssetLibraryStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FRSAssetLibraryStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FRSAssetLibraryStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("RSAssetLibraryStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FRSAssetLibraryStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("RSAssetLibraryStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("RSAssetLibrary")->GetBaseDir() / TEXT("Resources"));

	Style->Set("RSAssetLibrary.LoginBackground", new IMAGE_BRUSH(TEXT("MainBackground"), FVector2D(1920,1080)));

	Style->Set("RSAssetLibrary.IconTab.Pressed", new IMAGE_BRUSH(TEXT("ButtonPressed"), FVector2D(8,80)));
	Style->Set("RSAssetLibrary.IconTab.Normal", new IMAGE_BRUSH(TEXT("ButtonNormal"), FVector2D(270,60)));
	Style->Set("RSAssetLibrary.IconTab.Hovered", new IMAGE_BRUSH(TEXT("ButtonHovered"), FVector2D(8,80)));
	Style->Set("RSAssetLibrary.IconTab.Normal.NoArrow", new IMAGE_BRUSH(TEXT("ButtonNormalNoArrow"), FVector2D(8,80)));
	Style->Set("RSAssetLibrary.IconTab.Hovered.NoArrow", new IMAGE_BRUSH(TEXT("ButtonHoveredNoArrow"), FVector2D(8,80)));
	Style->Set("RSAssetLibrary.IconTab.ButtonPressedHovered", new IMAGE_BRUSH(TEXT("ButtonPressedHovered"), FVector2D(8,80)));
	Style->Set("RSAssetLibrary.IconTab.AssetSelected", new IMAGE_BRUSH(TEXT("AssetSelected"), FVector2D(8,80)));

	Style->Set("RSAssetLibrary.LoginButton_UnSelected", new IMAGE_BRUSH(TEXT("LoginButton"), FVector2D(200,50)));
	Style->Set("RSAssetLibrary.LoginButton_Selected", new IMAGE_BRUSH(TEXT("LoginButtonhovered"), FVector2D(200,50)));
	Style->Set("RSAssetLibrary.LoginButton_Hovered", new IMAGE_BRUSH(TEXT("LoginButtonhovered"), FVector2D(200,50)));

	Style->Set("RSAssetLibrary.OptionBorder", new IMAGE_BRUSH(TEXT("OptionBorder"), FVector2D(50,200)));
	Style->Set("RSAssetLibrary.ContentBorder", new IMAGE_BRUSH(TEXT("ContentBorder"), FVector2D(200,180)));
	Style->Set("RSAssetLibrary.SearchBorder", new IMAGE_BRUSH(TEXT("SearchBorder"), FVector2D(200,10)));
	Style->Set("RSAssetLibrary.DetailsBorder", new IMAGE_BRUSH(TEXT("DetailsBorder"), FVector2D(50,200)));
	Style->Set("RSAssetLibrary.DownloadBarBorder", new IMAGE_BRUSH(TEXT("DownloadBarBorder"), FVector2D(200,20)));

	Style->Set("RSAssetLibrary.QRLoginIcon", new IMAGE_BRUSH(TEXT("QRLoginIcon"), FVector2D(60,60)));
	Style->Set("RSAssetLibrary.AccountLoginIcon", new IMAGE_BRUSH(TEXT("AccountLoginIcon"), FVector2D(60,60)));


	Style->Set("RSAssetLibrary.Phone", new IMAGE_BRUSH(TEXT("Phone"), FVector2D(300,50)));
	Style->Set("RSAssetLibrary.Msg", new IMAGE_BRUSH(TEXT("Msg"), FVector2D(200,20)));
	
	Style->Set("PluginIcon.AudioIcon.Icon", new IMAGE_BRUSH(TEXT("AudioIcon"), FVector2D(64,64)));
	Style->Set("PluginIcon.ConceptIcon.Icon", new IMAGE_BRUSH(TEXT("ConceptIcon"), FVector2D(64,64)));
	Style->Set("PluginIcon.VideoIcon.Icon", new IMAGE_BRUSH(TEXT("VideoIcon"), FVector2D(64,64)));
	Style->Set("PluginIcon.ModelIcon.Icon", new IMAGE_BRUSH(TEXT("ModelIcon"), FVector2D(64,64)));
	Style->Set("PluginIcon.biaoqian.Icon", new IMAGE_BRUSH(TEXT("biaoqianicon"), FVector2D(64,64)));
	Style->Set("PluginIcon.xiazai.Icon", new IMAGE_BRUSH(TEXT("xiazaiicon"), FVector2D(64,64)));
	
	Style->Set("PluginIcon.UserProfileImage.Icon", new IMAGE_BRUSH(TEXT("UserProfileImage"), FVector2D(20,20)));
	
	Style->Set("PluginIcon.border1.Icon", new IMAGE_BRUSH(TEXT("Border1"), FVector2D(272,1000)));
	Style->Set("PluginIcon.xiangmu.Icon", new IMAGE_BRUSH(TEXT("xiangmuicon"), FVector2D(64,64)));

	Style->Set("PluginIcon.Folder.Icon", new IMAGE_BRUSH(TEXT("Folder"), FVector2D(64,64)));
	Style->Set("PluginIcon.FBXFile.Icon", new IMAGE_BRUSH(TEXT("FBXFile"), FVector2D(64,64)));
	Style->Set("PluginIcon.AudioFile.Icon", new IMAGE_BRUSH(TEXT("AudioFilefix"), FVector2D(150,150)));
	Style->Set("PluginIcon.SmallFolder.Icon", new IMAGE_BRUSH(TEXT("SmallFolder"), FVector2D(20,20)));
	Style->Set("PluginIcon.ModelFileIcon", new IMAGE_BRUSH(TEXT("Modelfileicon"), FVector2D(77,94)));
	Style->Set("PluginIcon.OtherFileIcon", new IMAGE_BRUSH(TEXT("Otherfileicon"), FVector2D(77,94)));


	Style->Set("RSAssetLibrary.LoginButton", new IMAGE_BRUSH(TEXT("LoginButton"), FVector2D(320,48)));
	Style->Set("RSAssetLibrary.LogoutButton", new IMAGE_BRUSH(TEXT("LogoutButton"), FVector2D(45,22)));

	Style->Set("RSAssetLibrary.LoginBackgroundBlack", new IMAGE_BRUSH(TEXT("PureBlack"), FVector2D(600,600)));

	Style->Set("PluginIcon.Icon", new IMAGE_BRUSH(TEXT("RSLogo"), FVector2D(128,128)));

	Style->Set("RSAssetLibrary.PluginAction", new IMAGE_BRUSH(TEXT("RSLogo"), FVector2D(128,128)));

	Style->Set("RSAssetLibrary.ProjectBorder", new IMAGE_BRUSH(TEXT("Projectborder"), FVector2D(250,360)));
	Style->Set("RSAssetLibrary.ProjectLine", new IMAGE_BRUSH(TEXT("Projectline"), FVector2D(160,1)));
	Style->Set("RSAssetLibrary.ModerBorderButton", new IMAGE_BRUSH(TEXT("Moderborderbutton"), FVector2D(128,128)));
	Style->Set("RSAssetLibrary.FileBorder", new IMAGE_BRUSH(TEXT("Fileborder"), FVector2D(160,1)));
	Style->Set("RSAssetLibrary.AudioImage", new IMAGE_BRUSH(TEXT("Audioimage"), FVector2D(439,273)));
	Style->Set("RSAssetLibrary.DetailsNameBorder", new IMAGE_BRUSH(TEXT("Detailsnameborder"), FVector2D(412,56)));
	Style->Set("RSAssetLibrary.ModerBorderButtonClicked", new IMAGE_BRUSH(TEXT("Moderborderbuttonclicked"), FVector2D(128,128)));
	Style->Set("RSAssetLibrary.BeginIcon", new IMAGE_BRUSH(TEXT("Beginicon"), FVector2D(24,24)));
	Style->Set("RSAssetLibrary.CancelIcon", new IMAGE_BRUSH(TEXT("Cancelicon"), FVector2D(24,24)));
	Style->Set("RSAssetLibrary.PauseIcon", new IMAGE_BRUSH(TEXT("Pauseicon"), FVector2D(24,24)));
	Style->Set("RSAssetLibrary.BeginClicked", new IMAGE_BRUSH(TEXT("Beginclicked"), FVector2D(24,24)));
	Style->Set("RSAssetLibrary.CancelClicked", new IMAGE_BRUSH(TEXT("Cancleclicked"), FVector2D(24,24)));
	Style->Set("RSAssetLibrary.PauseClicked", new IMAGE_BRUSH(TEXT("Pauseclicked"), FVector2D(24,24)));
	Style->Set("RSAssetLibrary.DetailDownloadIcon", new IMAGE_BRUSH(TEXT("Detaildownloadicon"), FVector2D(24,24)));
	Style->Set("RSAssetLibrary.ImportIcon", new IMAGE_BRUSH(TEXT("Importicon"), FVector2D(24,24)));
	Style->Set("RSAssetLibrary.ImportHover", new IMAGE_BRUSH(TEXT("Importhover"), FVector2D(70,36)));
	Style->Set("RSAssetLibrary.Importclicked", new IMAGE_BRUSH(TEXT("Importclicked"), FVector2D(70,36)));
	Style->Set("RSAssetLibrary.PlayCircle", new IMAGE_BRUSH(TEXT("Playcircle"), FVector2D(64,64)));
	Style->Set("RSAssetLibrary.PlayCircleHover", new IMAGE_BRUSH(TEXT("Playcirclehover"), FVector2D(64,64)));
	Style->Set("RSAssetLibrary.PlayImage", new IMAGE_BRUSH(TEXT("Playimage"), FVector2D(40,40)));
	Style->Set("RSAssetLibrary.PlayImageHover", new IMAGE_BRUSH(TEXT("Playimagehover"), FVector2D(40,40)));
	Style->Set("RSAssetLibrary.CommentCircle", new IMAGE_BRUSH(TEXT("Commentcircle"), FVector2D(15,15)));
	Style->Set("RSAssetLibrary.DownloadBottom", new IMAGE_BRUSH(TEXT("Downloadbottom"), FVector2D(640,56)));
	Style->Set("RSAssetLibrary.DownloadWindowBorder", new IMAGE_BRUSH(TEXT("Downloadwindowborder"), FVector2D(640,939)));
	Style->Set("RSAssetLibrary.DownloadListButtonNormal", new IMAGE_BRUSH(TEXT("Downloadlistbuttonnormal"), FVector2D(96,32)));
	Style->Set("RSAssetLibrary.DownloadListButtonClicked", new IMAGE_BRUSH(TEXT("Downloadlistbuttonclicked"), FVector2D(96,32)));
	Style->Set("RSAssetLibrary.TagSearchButton", new IMAGE_BRUSH(TEXT("TagSearchButton"), FVector2D(40,40)));

	Style->Set("RSAssetLibrary.Deleteiconclicked", new IMAGE_BRUSH(TEXT("Deleteiconclicked"), FVector2D(24,24)));
	Style->Set("RSAssetLibrary.Deleteiconnormal", new IMAGE_BRUSH(TEXT("Deleteiconnormal"), FVector2D(24,24)));
	Style->Set("RSAssetLibrary.Serchiconclicked", new IMAGE_BRUSH(TEXT("Serchiconclicked"), FVector2D(24,24)));
	Style->Set("RSAssetLibrary.Serchiconnormal", new IMAGE_BRUSH(TEXT("Serchiconnormal"), FVector2D(24,24)));
	

	Style->Set("RSAssetLibrary.DownloadBorder", new IMAGE_BRUSH(TEXT("Downloadborder"), FVector2D(106,4)));
	Style->Set("RSAssetLibrary.DownloadComplete", new IMAGE_BRUSH(TEXT("Downloadcomplete"), FVector2D(106,4)));
	Style->Set("RSAssetLibrary.DownloadImageFill", new IMAGE_BRUSH(TEXT("Downloadimagefill"), FVector2D(50,4)));

	Style->Set("RSAssetLibrary.TagBorder", new IMAGE_BRUSH(TEXT("TagBorder"), FVector2D(40,20)));
	Style->Set("RSAssetLibrary.NoAssets", new IMAGE_BRUSH(TEXT("NoAssets"), FVector2D(128,128)));
	Style->Set("RSAssetLibrary.NetError", new IMAGE_BRUSH(TEXT("NetError"), FVector2D(256,256)));
	Style->Set("RSAssetLibrary.ClearFilter", new IMAGE_BRUSH(TEXT("ClearFilter"), FVector2D(24,24)));

	Style->Set("RSAssetLibrary.Line", new IMAGE_BRUSH(TEXT("Line"), FVector2D(10,1)));

	// Create and set a custom progress bar style 创建并设置自定义进度条样式
	FProgressBarStyle CustomProgressBarStyle = FProgressBarStyle()
		.SetBackgroundImage(*Style->GetBrush("RSAssetLibrary.DownloadBorder")) 
		.SetFillImage(*Style->GetBrush("RSAssetLibrary.DownloadImageFill"));   

	// Adds a custom progress bar style to SlateStyleSet 将自定义进度条样式添加到 SlateStyleSet 中
	Style->Set("RSAssetLibrary.CustomProgressBar", CustomProgressBarStyle);
	
	
	return Style;
}

void FRSAssetLibraryStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FRSAssetLibraryStyle::Get()
{
	return *StyleInstance;
}
