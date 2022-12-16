/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#include "TutorialDemo/Public/EditUserWidgetBase.h"
#include "TutorialDemo/TutorialDataAccessLayer.h"
#include "TutorialDemo/Public/UserAccountDetails.h"

void UEditUserWidgetBase::OnWidgetInitialized()
{
	Super::OnWidgetInitialized();
	SaveChangesAction->OnPressed.AddDynamic(this, &UEditUserWidgetBase::SaveChanges);
}

void UEditUserWidgetBase::SaveChanges()
{
	/* Several things need to happen in order to make this work correctly.
	   The user *CAN* change their username, but obviously if they leave it the same
	   then that's also ok.
	   However, if a user tries to change their username to one which is already user by a different account,
	   that is NOT ok, and needs to be reported to the player.
	
	   The easiest way to do this, is to first CHECK if the proposed username is already in use,
	   by a UserID other than that of the player requesting the change. And if so, no change is attempted. */

	if (Dal && CurrentUser)
	{
		FString UserName = UserNameInput->GetText().ToString();

		if (Dal->IsUserNameInUse(UserName, CurrentUser->UserID))
		{
			ErrorText->SetText(NSLOCTEXT("UserAccounts", "NameInUse", "Error: The selected name is already taken."));
			ErrorText->SetVisibility(ESlateVisibility::Visible);
			SuccessText->SetVisibility(ESlateVisibility::Hidden);
			return;
		}

		bool Result = Dal->EditUserAccount(CurrentUser->UserID,
		                                   UserName,
		                                   TaglineInput->GetText().ToString(),
		                                   PasswordInput->GetText().ToString());

		if (Result)
		{
			/* Refresh the CurrentUser with any changes.
			   You could do this by applying the input widget values to the CurrentUser class,
			   but it is easier and better to refresh them from the database,
			   as the DAL could have 'sanitized' certain input values, etc.	 */
			CurrentUser = Dal->GetUserAccountDetails(CurrentUser->UserID);
			OnUserEdit.Broadcast(CurrentUser);
			PasswordInput->SetText(FText::GetEmpty());
			SuccessText->SetVisibility(ESlateVisibility::Visible);
			ErrorText->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			ErrorText->SetText(NSLOCTEXT("UserAccounts", "BadPassword", "Error: The password is incorrect."));
			ErrorText->SetVisibility(ESlateVisibility::Visible);
			SuccessText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UEditUserWidgetBase::OnUserAccountSet()
{
	Super::OnUserAccountSet();
	UserNameInput->SetText(FText::FromString(CurrentUser->UserName));
	TaglineInput->SetText(FText::FromString(CurrentUser->TagLine));
}

void UEditUserWidgetBase::SetErrorMessage(FText InErrorMessage)
{
	ErrorMessage = InErrorMessage;
}

void UEditUserWidgetBase::SetSuccessMessage(FText InSuccessMessage)
{
	SuccessMessage = InSuccessMessage;
}
