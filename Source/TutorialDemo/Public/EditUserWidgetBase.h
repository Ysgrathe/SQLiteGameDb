/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserAccountWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/MultiLineEditableTextBox.h"
#include "EditUserWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserEdit, UUserAccountDetails*, UserDetails);

/* */
UCLASS(Abstract)
class TUTORIALDEMO_API UEditUserWidgetBase : public UUserAccountWidgetBase
{
public:
	virtual void OnWidgetInitialized() override;

	/* This event is broadcast when a user successfully edits their account.
	   No event happens in a case of a failure. */
	UPROPERTY(BlueprintAssignable, Category = "User Account")
	FUserEdit OnUserEdit;
	
	UFUNCTION()
	virtual void SaveChanges();

	virtual void OnUserAccountSet() override;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UEditableTextBox* UserNameInput = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UMultiLineEditableTextBox* TaglineInput = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UEditableTextBox* PasswordInput = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UButton* SaveChangesAction = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UTextBlock* ErrorText = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UTextBlock* SuccessText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Messages")
	FText ErrorMessage;

	UFUNCTION(BlueprintCallable, Category="Appearance")
	void SetErrorMessage(FText InErrorMessage);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Messages")
	FText SuccessMessage;

	UFUNCTION(BlueprintCallable, Category="Appearance")
	void SetSuccessMessage(FText InSuccessMessage);

protected:
private:
	GENERATED_BODY()
};
