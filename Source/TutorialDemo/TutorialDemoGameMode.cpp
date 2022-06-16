// Copyright Epic Games, Inc. All Rights Reserved.

#include "TutorialDemoGameMode.h"
#include "TutorialDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATutorialDemoGameMode::ATutorialDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
