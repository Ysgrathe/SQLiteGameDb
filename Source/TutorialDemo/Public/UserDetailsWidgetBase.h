/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "UserAccountWidgetBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "UserDetailsWidgetBase.generated.h"

class UUserAccountDetails;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserLogout, int32, UserID);

/* */
UCLASS(Abstract)
class TUTORIALDEMO_API UUserDetailsWidgetBase : public UUserAccountWidgetBase
{
public:
	virtual void OnWidgetInitialized() override;
	
	UFUNCTION()
	virtual void PerformLogout();
	virtual void OnUserAccountSet() override;

	/* This event is broadcast when a user logs out. */
	UPROPERTY(BlueprintAssignable, Category = "User Account")
	FUserLogout OnUserLogout;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UTextBlock* UserNameLabel = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UTextBlock* TaglineLabel = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	UButton* LogoutAction = nullptr;

protected:
private:

	GENERATED_BODY()
};
