/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "TutorialLevelScriptActor.generated.h"

/* */
UCLASS()
class TUTORIALDEMO_API ATutorialLevelScriptActor : public ALevelScriptActor
{
	friend class UTutorialDataAccessLayer;

public:
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


private:
	/* These will hold the name & ID of the level, without the blueprint '_C_nnn'
	 * crap found on the end. */
	FString LevelName;
	int32 LevelID;

	/* Filled with a list of all the APickupItems found in the level.
	 * We cache this at BeginPlay, but would need other approaches
	 * if pickup items were dynamically spawned during play. */
	UPROPERTY()
	TArray<class APickupItem*> EquipmentItems;

	/* Reference to the data access layer class we use. */
	UPROPERTY()
	UTutorialDataAccessLayer* Dal = nullptr;

	GENERATED_BODY()
};
