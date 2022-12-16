// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialDemo/Public/CreateUserWidgetBase.h"
#include "UserAccountDetails.h"
#include "TutorialDemo/TutorialDataAccessLayer.h"

void UCreateUserWidgetBase::OnWidgetInitialized()
{
	Super::OnWidgetInitialized();
	CreateAccountAction->OnPressed.AddDynamic(this, &UCreateUserWidgetBase::CreateUserAccount);
}

void UCreateUserWidgetBase::CreateUserAccount()
{
	if (Dal)
	{
		/* Here we trim whitespace from the start/end of the username.
		   As we dont want users trying to create a UserName
		   that 'looks' like one already taken, just by adding spaces at the beginning/end. */
		FString UserName        = UserNameInput->GetText().ToString().TrimStartAndEnd();
		FString Tagline         = TaglineInput->GetText().ToString().TrimStartAndEnd();
		FString Password        = PasswordInput->GetText().ToString();
		FString ConfirmPassword = ConfirmPasswordInput->GetText().ToString();

		/* We test to see if the chosen username is already taken in the database.
		   Usually we pass in the UserId of the current user account
		   (Because that user *CAN* use it's own current username in an UPDATE query)
		   But in this case, as we are making a new account and dont have a user account yet,
		   we simply pass in 0 as the user id - this is a value that will never actually be used
		   by the database, and simply results in testing if ANY account uses the given UserName. */
		if (Dal->IsUserNameInUse(UserName, 0))
		{
			ErrorText->SetText(NSLOCTEXT("UserAccounts", "NameInUse", "Error: The selected name is already taken."));
			ErrorText->SetVisibility(ESlateVisibility::Visible);
			return;
		}

		/* At this point we could do any other checks that we wanted;
		   Password complexity,
		   testing the username/tagline against a list of banned words, etc., etc.
		   In this case, we just check that the password, and password-confirmation are the same: */
		if (Password != ConfirmPassword)
		{
			ErrorText->SetText(NSLOCTEXT("UserAccounts", "PasswordConfirmFail",
			                             "Error: The entered password, and password confirmation do not match."));
			ErrorText->SetVisibility(ESlateVisibility::Visible);
			return;
		}

		/* Once all tests are considered OK, we can just move to creating the new user account. */
		int32 NewUserID = Dal->CreateUserAccount(UserName, Password, Tagline);
		if (NewUserID != -1)
		{
			Clean();
			/* We have a valid UserID which means the account creation was successful.
			   So we can broadcast this to listeners. */
			OnAccountCreated.Broadcast(Dal->GetUserAccountDetails(NewUserID));
		}
	}
}

void UCreateUserWidgetBase::Clean()
{
	UserNameInput->SetText(FText::GetEmpty());
	TaglineInput->SetText(FText::GetEmpty());
	PasswordInput->SetText(FText::GetEmpty());
	ConfirmPasswordInput->SetText(FText::GetEmpty());
	ErrorText->SetVisibility(ESlateVisibility::Hidden);
	UserNameInput->SetFocus();
}

