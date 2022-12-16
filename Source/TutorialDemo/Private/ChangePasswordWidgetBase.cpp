/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#include "TutorialDemo/Public/ChangePasswordWidgetBase.h"
#include "TutorialDemo/TutorialDataAccessLayer.h"
#include "TutorialDemo/Public/UserAccountDetails.h"

void UChangePasswordWidgetBase::OnWidgetInitialized()
{
	Super::OnWidgetInitialized();
	ChangePasswordAction->OnPressed.AddDynamic(this, &UChangePasswordWidgetBase::PerformPasswordChange);
}

void UChangePasswordWidgetBase::PerformPasswordChange()
{
	/* Before even submitting a request to the database,
	   we should check that the inputs match any requirements (minimum length, etc)
	   and that the new password inputs have the same content. */

	if (ConfirmNewPasswordInput->GetText().ToString() != NewPasswordInput->GetText().ToString())
	{
		ErrorText->SetVisibility(ESlateVisibility::Visible);
		SuccessText->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	if (Dal && CurrentUser && CurrentUser->UserID > 0)
	{
		bool PasswordChanged = Dal->ChangeUserPassword(CurrentUser->UserID,
		                                               CurrentPasswordInput->GetText().ToString(),
		                                               NewPasswordInput->GetText().ToString());

		if (PasswordChanged)
		{
			SuccessText->SetVisibility(ESlateVisibility::Visible);
			ErrorText->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			ErrorText->SetVisibility(ESlateVisibility::Visible);
			SuccessText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UChangePasswordWidgetBase::Clean()
{
	CurrentPasswordInput->SetText(FText::GetEmpty());
	NewPasswordInput->SetText(FText::GetEmpty());
	ConfirmNewPasswordInput->SetText(FText::GetEmpty());
}

void UChangePasswordWidgetBase::SetErrorMessage(FText InErrorMessage)
{
	ErrorMessage = InErrorMessage;
}

void UChangePasswordWidgetBase::SetSuccessMessage(FText InSuccessMessage)
{
	SuccessMessage = InSuccessMessage;
}
