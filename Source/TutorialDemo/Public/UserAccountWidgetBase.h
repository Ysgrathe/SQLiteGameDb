/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserAccountWidgetBase.generated.h"

class UUserAccountDetails;
class UTutorialDataAccessLayer;
/* */
UCLASS()
class TUTORIALDEMO_API UUserAccountWidgetBase : public UUserWidget
{
public:
	/* Devirtualize and mark as final.
	   We dont want subclasses forgetting to call the parent code,
	   as it might stop things from working.
	   By doing this, and calling new virtual 'inner' functions,
	   subclasses can provide their own implementations if required, 
	   while leaving the base class code out of their reach. */
	virtual void NativeOnInitialized() override final;

	UFUNCTION()
	virtual void OnWidgetInitialized();

	UFUNCTION(BlueprintCallable)
	void SetUserDetails(UUserAccountDetails* InUserDetails);

	UFUNCTION()
	virtual void OnUserAccountSet();

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	UPROPERTY()
	UTutorialDataAccessLayer* Dal = nullptr;

	UPROPERTY()
	UUserAccountDetails* CurrentUser = nullptr;

private:
	GENERATED_BODY()
};
