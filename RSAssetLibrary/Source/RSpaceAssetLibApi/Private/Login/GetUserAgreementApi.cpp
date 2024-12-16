// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "Login/GetUserAgreementApi.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

void UGetUserAgreementApi::SendGetUserAgreementRequest(FOnUserAgreementResponse ResponseCallback)
{
	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindLambda([ResponseCallback](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
	{
		if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
		{
			FString ResponseContent = HttpResponse->GetContentAsString();
			ResponseCallback.ExecuteIfBound(ResponseContent);
		}
		else
		{
			// UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve user agreement. Status Code: %d"), HttpResponse->GetResponseCode());
		}
	});
    
	FString ApiUrl = TEXT("https://api.meta.mg.xyz/spaceapi/am/user/user-agreement?type=1&appId=1"); 
	Request->SetURL(ApiUrl);
	Request->SetVerb(TEXT("GET")); 
	Request->ProcessRequest();
}

void UGetUserAgreementApi::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnUserAgreementResponse OnResponse)
{
	if (!bWasSuccessful || !Response.IsValid()) return;

	FString AgreementContent = Response->GetContentAsString();
	FString ParsedContent = ParseHtmlContent(AgreementContent);
    
	// 返回解析后的协议内容
	OnResponse.ExecuteIfBound(ParsedContent);
}


FString UGetUserAgreementApi::ParseHtmlContent(const FString& HtmlContent)
{
	FString ParsedText = HtmlContent;


	ParsedText = ParsedText.Replace(TEXT("<br>"), TEXT("\n"));
	ParsedText = ParsedText.Replace(TEXT("<p>"), TEXT("\n"));
	ParsedText = ParsedText.Replace(TEXT("</p>"), TEXT("\n"));


	ParsedText = ParsedText.Replace(TEXT("text-align: center;"), TEXT(""));
	ParsedText = ParsedText.Replace(TEXT("text-align: right;"), TEXT(""));
	ParsedText = ParsedText.Replace(TEXT("text-align: left;"), TEXT(""));

	ParsedText = ParsedText.Replace(TEXT("text-indent: 21pt;"), TEXT("\t")); 


	ParsedText = ParsedText.Replace(TEXT("<span style=\"color: rgb(191, 191, 191);\">"), TEXT(""));
	ParsedText = ParsedText.Replace(TEXT("</span>"), TEXT(""));


	ParsedText = ParsedText.Replace(TEXT("<strong>"), TEXT("<RichText.Bold>"));
	ParsedText = ParsedText.Replace(TEXT("</strong>"), TEXT("</RichText.Bold>"));


	ParsedText = ParsedText.Replace(TEXT("<i>"), TEXT("<RichText.Italic>"));
	ParsedText = ParsedText.Replace(TEXT("</i>"), TEXT("</RichText.Italic>"));


	ParsedText = ParsedText.Replace(TEXT("<"), TEXT("")).Replace(TEXT(">"), TEXT(""));


	ParsedText = ParsedText.Replace(TEXT("&nbsp;"), TEXT(" "));
	ParsedText = ParsedText.Replace(TEXT("&amp;"), TEXT("&"));
	ParsedText = ParsedText.Replace(TEXT("&lt;"), TEXT("<"));
	ParsedText = ParsedText.Replace(TEXT("&gt;"), TEXT(">"));
	ParsedText = ParsedText.Replace(TEXT("&quot;"), TEXT("\""));


	ParsedText.TrimStartAndEndInline();
	ParsedText = ParsedText.Replace(TEXT("\n\n"), TEXT("\n"));

	return ParsedText;
}

