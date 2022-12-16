/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#include "TutorialDemo/Public/LoginWidgetBase.h"
#include "TutorialDemo/TutorialDataAccessLayer.h"

void ULoginWidgetBase::OnWidgetInitialized()
{
	Super::OnWidgetInitialized();
	LoginAction->OnPressed.AddDynamic(this, &ULoginWidgetBase::PerformLogin);
}

void ULoginWidgetBase::PerformLogin()
{
	if (Dal)
	{
		FString UserName = UserNameInput->GetText().ToString();
		FString Password = PasswordInput->GetText().ToString();

		/* Perform any basic 'sanity checks' here.
		   This might include things like testing minimum/maximum string length,
		   stripping out/converting certain symbols or characters,
		   Or processing character case in a consistent manner. */

		int32 UserID = Dal->TestUserLogin(UserName, Password);

		if (UserID != -1)
		{
			/* Successful login, clear the password field (just to be safe),
			   and inform any listeners of the event. */
			Clean();
			OnUserLogon.Broadcast(Dal->GetUserAccountDetails(UserID));
		}
		else
		{
			/* A failed logon attempt has been made.
			   Erase the password field, and display the error message. */
			PasswordInput->SetText(FText::GetEmpty());
			ErrorText->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ULoginWidgetBase::Clean()
{
	UserNameInput->SetText(FText::GetEmpty());
	PasswordInput->SetText(FText::GetEmpty());
	ErrorText->SetVisibility(ESlateVisibility::Hidden);
	UserNameInput->SetFocus();
}

void ULoginWidgetBase::SetErrorMessage(FText InErrorMessage)
{
	ErrorMessage = InErrorMessage;
}
