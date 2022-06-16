/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DbEquipmentComponent.generated.h"

class APickupItem;
class UTutorialDataAccessLayer;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TUTORIALDEMO_API UDbEquipmentComponent : public UActorComponent
{
public:
	UDbEquipmentComponent();
	virtual void OnComponentCreated() override;

	UFUNCTION()
	/* Return the unique identifier for the attached Actor, based on it's name. */
	FString GetID();

	/* Causes the attached actors state to be persisted back to the database. */
	UFUNCTION()
	void SaveState();

	/* Retrieves the attached actors state from the database. */
	UFUNCTION()
	void LoadState();

protected:
	virtual void BeginPlay() override;
private:
	/* Unique identifier used in the database. */
	FString ID = TEXT("UNASSIGNED");

	/* Indicates if the actor using this component needs to
	 * persist state changes back into the database.
	 * (Has anything changed since loading.) */
	bool IsDirty = false;

	UTutorialDataAccessLayer* Dal = nullptr;
	APickupItem* Parent = nullptr;

	GENERATED_BODY()
};
