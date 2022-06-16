/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


/*
#include "WorldDatabaseSubsystem.h"
#include "GameInstanceDatabaseSubsystem.h"
#include "TutorialDataAccessLayer.h"
#include "CustomLogging.h"
#include "WorldPartition/WorldPartitionSubsystem.h"

void UWorldDatabaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UWorldDatabaseSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UWorldDatabaseSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (UWorld* world = Cast<UWorld>(Outer)) // Is it a world?
	{
		if (world->IsGameWorld()) // But is it a GAME WORLD?
		{
			return true;
		}
	}

	return false;
}

void UWorldDatabaseSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (UDbManagerStatics::DbSubsystemIsValid())
	{
		if (UTutorialDataAccessLayer* DAL =
			Cast<UTutorialDataAccessLayer>(UDbManagerStatics::GetDatabaseByFilename(TEXT("AlienWarmonger.db"))))
		{
			if (DAL->IsLevelManaged(InWorld.GetName())) // But is it in the database?
			{
				LOG_TUT(Warning, TEXT("Managed world subsystem"));
				LOG_TUT(Warning, *InWorld.GetName());

			}
		}
	}

	
	
}
*/
