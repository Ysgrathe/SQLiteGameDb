// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserAccountWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "DeleteAccountWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserAccountDeletion, UUserAccountDetails*, UserDetails);

/* */
UCLASS(Abstract)
class TUTORIALDEMO_API UDeleteAccountWidgetBase : public UUserAccountWidgetBase
{
public:
	virtual void OnWidgetInitialized() override;

	UFUNCTION()
	virtual void DeleteUserAccount();

	/* This event is broadcast when a user account is deleted. */
	UPROPERTY(BlueprintAssignable, Category = "User Account")
	FUserAccountDeletion OnUserDeleted;
	
	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UEditableTextBox* PasswordInput = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UButton* DeleteAction = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UTextBlock* ErrorText = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Messages")
	FText ErrorMessage;

	UFUNCTION(BlueprintCallable, Category="Appearance")
	void SetErrorMessage(FText InErrorMessage);
	
protected:


private:
	
	GENERATED_BODY()
	
};
