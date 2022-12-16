/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "UserAccountWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "ChangePasswordWidgetBase.generated.h"

class UUserAccountDetails;
/* */
UCLASS(Abstract)
class TUTORIALDEMO_API UChangePasswordWidgetBase : public UUserAccountWidgetBase
{
public:
	virtual void OnWidgetInitialized() override;

	UFUNCTION()
	virtual void PerformPasswordChange();

	//virtual void OnUserAccountSet() override;

	UFUNCTION()
	void Clean();


	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UEditableTextBox* CurrentPasswordInput = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UEditableTextBox* NewPasswordInput = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UEditableTextBox* ConfirmNewPasswordInput = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UButton* ChangePasswordAction = nullptr;

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
