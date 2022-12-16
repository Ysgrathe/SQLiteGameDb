/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#include "TutorialDemo/Public/DeleteAccountWidgetBase.h"

#include "UserAccountDetails.h"
#include "TutorialDemo/TutorialDataAccessLayer.h"

void UDeleteAccountWidgetBase::OnWidgetInitialized()
{
	Super::OnWidgetInitialized();
	DeleteAction->OnPressed.AddDynamic(this, &UDeleteAccountWidgetBase::DeleteUserAccount);
}

void UDeleteAccountWidgetBase::DeleteUserAccount()
{
	if(Dal && CurrentUser)
	{
		bool IsDeleted = Dal->DeleteUserAccount(CurrentUser->UserID, PasswordInput->GetText().ToString());
		if (IsDeleted)
		{
			ErrorText->SetVisibility(ESlateVisibility::Hidden);
			OnUserDeleted.Broadcast(CurrentUser);
		}
		else
		{
			ErrorText->SetVisibility(ESlateVisibility::Visible);
		}
	}

	/* Regardless of success/failure we always clear the password. */
	PasswordInput->SetText(FText::GetEmpty());
}

void UDeleteAccountWidgetBase::SetErrorMessage(FText InErrorMessage)
{
	ErrorMessage = InErrorMessage;
}
