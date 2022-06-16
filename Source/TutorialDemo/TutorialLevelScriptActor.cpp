/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#include "TutorialLevelScriptActor.h"
#include "CustomLogging.h"
#include "GameInstanceDatabaseSubsystem.h"
#include "PickupItem.h"
#include "TutorialDataAccessLayer.h"

void ATutorialLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	LevelName = GetActorNameOrLabel();

	FString Msg = FString::Printf(TEXT("LEVEL SCRIPT: BeginPlay %s"), *LevelName);
	LOG_TUT(Warning, *Msg);
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, Msg);

	if (UDbManagerStatics::DbSubsystemIsValid())
	{
		Dal = Cast<UTutorialDataAccessLayer>(UDbManagerStatics::GetDatabaseByFilename(TEXT("AlienWarmonger.db")));
		EquipmentItems.Empty();
		if (Dal)
		{
			LevelID = Dal->GetLevelID(LevelName);
			if (LevelID > 0) // Is it in the database?
			{
				LOG_TUT(Warning, TEXT("Managed world subsystem: Load"));
				LOG_TUT(Warning, *LevelName);

				/* Get all the actors that are APickupItem, so that we can load any values for them from the database. */
				TArray<AActor*> AllEquipmentItems;

				UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickupItem::StaticClass(), AllEquipmentItems);
				for (AActor* EquipmentItem : AllEquipmentItems)
				{
					/* We are only interested in actors which belong to the current sub-level. */
					FString ActorsLevelName = EquipmentItem->GetLevel()->GetFullGroupName(true);

					if (ActorsLevelName == LevelName)
					{
						if (APickupItem* SpecificItem = Cast<APickupItem>(EquipmentItem))
						{
							EquipmentItems.Add(SpecificItem);

							const int32 QtyValue = Dal->LoadEquipmentItem(SpecificItem->GetName(), LevelID);
							/* Only change the property if a valid value was found in the database,
							 * otherwise it indicates that this item hasn't been persisted yet. */
							if (QtyValue != -1)
							{
								SpecificItem->SetQuantity(QtyValue);
								SpecificItem->LoadingComplete();
							}
						}
					}
				}
			}
		}
	}
}

void ATutorialLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FString Msg = FString::Printf(TEXT("LEVEL SCRIPT: EndPlay %s"), *LevelName);
	LOG_TUT(Warning, *Msg);
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, Msg);

	if (Dal)
	{
		LevelID = Dal->GetLevelID(LevelName);
		if (LevelID > 0) // Is it in the database?
		{
			LOG_TUT(Warning, TEXT("Managed world subsystem: Save"));
			LOG_TUT(Warning, *LevelName);

			/* Get all the actors that are APickupItem, so that we can load any values for them from the database. */
			//UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickupItem::StaticClass(), EquipmentItems);
			for (APickupItem* EquipmentItem : EquipmentItems)
			{
				EquipmentItem->SaveState(LevelID);
			}
		}
	}
}
