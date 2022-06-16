/* © Copyright 2022 Graham Chabas, All Rights Reserved. */



#include "TutorialStreamingGameMode.h"
#include "CustomLogging.h"
#include "Engine/LevelStreaming.h"

void ATutorialStreamingGameMode::StartPlay()
{
	Super::StartPlay();
	UWorld* World = GetWorld();
	const TArray<ULevelStreaming*>& SubLevels = World->GetStreamingLevels();
	LOG_TUT(Warning, TEXT("StartPlay"));
	LOG_TUT(Warning, *World->GetName());

	for (ULevelStreaming* SubLevel : SubLevels)
	{
		SubLevel->OnLevelLoaded.AddDynamic( this, &ATutorialStreamingGameMode::SubLevelLoaded);
		SubLevel->OnLevelUnloaded.AddDynamic( this, &ATutorialStreamingGameMode::SubLevelUnloaded);
	}


	//LevelStream1->OnLevelLoaded.AddDynamic(this, &AMyClass::OnLevelInstanceLoaded);
}

void ATutorialStreamingGameMode::StartToLeaveMap()
{
	LOG_TUT(Warning, TEXT("LeaveMap"));
	LOG_TUT(Warning, *GetWorld()->GetName());

	Super::StartToLeaveMap();
}

void ATutorialStreamingGameMode::SubLevelLoaded()
{
	LOG_TUT(Warning, TEXT("SubLevelLoaded"));
}

void ATutorialStreamingGameMode::SubLevelUnloaded()
{
	LOG_TUT(Warning, TEXT("SubLevelUnloaded"));
}
