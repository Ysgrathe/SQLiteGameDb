/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#include "TutorialDemo/Public/UserAccountWidgetBase.h"
#include "GameInstanceDatabaseSubsystem.h"
#include "TutorialDemo/TutorialDataAccessLayer.h"

void UUserAccountWidgetBase::NativeOnInitialized()
{
	Dal = Cast<UTutorialDataAccessLayer>(UDbManagerStatics::GetDatabaseByFilename(TEXT("AlienWarmonger.db")));
	OnWidgetInitialized();
	Super::NativeOnInitialized();
}

void UUserAccountWidgetBase::OnWidgetInitialized()
{
}

void UUserAccountWidgetBase::SetUserDetails(UUserAccountDetails* InUserDetails)
{
	CurrentUser = InUserDetails;
	OnUserAccountSet();
}


void UUserAccountWidgetBase::OnUserAccountSet()
{
}

/* ----------- EDITOR SPECIFIC CODE ------------ */

#if WITH_EDITOR


const FText UUserAccountWidgetBase::GetPaletteCategory()
{
	return NSLOCTEXT("UMG", "CustomPaletteCategory", "GGameDev - User Logon");
}
#endif
