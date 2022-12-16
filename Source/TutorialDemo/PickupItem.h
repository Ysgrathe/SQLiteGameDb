/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#pragma once

#include "CoreMinimal.h"
#include "DbEquipmentComponent.h"
#include "DbPersistable.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "PickupItem.generated.h"

class USphereComponent;

UCLASS(BlueprintType)
class TUTORIALDEMO_API APickupItem : public AActor
{
public:
	APickupItem();
	void SaveState(int32 LevelID);
	void LoadState(int32 LevelID);
	void SetQuantity(int32 NewValue);
	void DecrementQuantity();

	/* Used to indicate that data has just been initialized from the db,
	 * and the DirtyFlag should be cleared. */
	void LoadingComplete();

	/* Returns the current state of the DirtyFlag, indicating if this object
	 * needs to persist its state to the database. */
	const bool IsDirty();

	/* What type of equipment this instance represents. */
	UPROPERTY(EditAnywhere)
	int32 EquipmentTypeID = 1;

	UPROPERTY(EditAnywhere)
	int32 Quantity = 1;


	UPROPERTY()
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY()
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY()
	URotatingMovementComponent* Rotator = nullptr;

	UPROPERTY()
	USphereComponent* InteractCollision = nullptr;

	UPROPERTY()
	UTextRenderComponent* ItemLabel = nullptr;

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/* Unique identifier used in the database. */
	FString ID = TEXT("UNASSIGNED");


	/* Determines how many of the item are 'stacked', and if the item should
	 * even appear in the level (Quantity = 0). */

	/* Indicates if it needs to persist changes back into the database.
	 * (Has anything changed since last loading.) */
	bool DirtyFlag = false;

	UPROPERTY()
	UTutorialDataAccessLayer* Dal = nullptr;

	void SetMesh();

	inline const static TArray<FString> AssetPaths = {
		TEXT("/Game/Meshes/EnergyClip"),
		TEXT("/Game/Meshes/Injector"),
		TEXT("/Game/Meshes/RationPack"),
		TEXT("/Game/Meshes/ShieldPack"),
		TEXT("/Game/Meshes/SprayCan")
	};

	static TArray<class UStaticMesh*> AssetMeshes;

	GENERATED_BODY()
};
