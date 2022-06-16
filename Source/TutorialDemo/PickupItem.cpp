/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#include "PickupItem.h"
#include "CustomLogging.h"
#include "GameInstanceDatabaseSubsystem.h"
#include "TutorialDataAccessLayer.h"
#include "Components/SphereComponent.h"

TArray<UStaticMesh*> APickupItem::AssetMeshes;

APickupItem::APickupItem()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root Comp"));
	RootComponent = SceneRoot;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	Mesh->SetupAttachment(RootComponent);

	Rotator = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotate Comp"));
	Rotator->RotationRate = FRotator(0.0f, 45.0f, 0.0f);
	
	InteractCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractCollision"));
	InteractCollision->SetSphereRadius(50.0f);
	InteractCollision->SetupAttachment(RootComponent);

	ItemLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Item Label"));
	ItemLabel->SetupAttachment(RootComponent);
	ItemLabel->SetHorizontalAlignment(EHTA_Center);
	ItemLabel->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

	/* Load the static meshes. */
	if (AssetMeshes.Num() == 0)
	{
		for (int32 i = 1; i < 6; i++)
		{
			ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(*AssetPaths[i - 1]);

			if (MeshObj.Succeeded())
			{
				UStaticMesh* MeshAsset = MeshObj.Object;
				AssetMeshes.Add(MeshAsset);
			}
		}
	}

	SetMesh();
}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();

	ID = GetName();
	Dal = Cast<UTutorialDataAccessLayer>(UDbManagerStatics::GetDatabaseByFilename(TEXT("AlienWarmonger.db")));
}

void APickupItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LOG_TUT(Warning, TEXT("EndPlay"));
	Super::EndPlay(EndPlayReason);
}

void APickupItem::LoadingComplete()
{
	DirtyFlag = false;
}

const bool APickupItem::IsDirty()
{
	return DirtyFlag;
}

void APickupItem::SaveState(int32 LevelID)
{
	if (Dal && DirtyFlag)
	{
		Dal->SaveEquipmentItem(ID, EquipmentTypeID, LevelID, Quantity);
	}
}

void APickupItem::LoadState(int32 LevelID)
{
	if (Dal)
	{
		int32 QuantityFromDb = Dal->LoadEquipmentItem(ID, LevelID);
	}
}

void APickupItem::SetQuantity(int32 NewValue)
{
	if (NewValue >= 0)
	{
		Quantity = NewValue;
		DirtyFlag = true;
		ItemLabel->SetText(FText::FromString(FString::FromInt(Quantity)));
		ItemLabel->SetVisibility(Quantity > 1);
		SetActorHiddenInGame(Quantity < 1);
	}
}

void APickupItem::DecrementQuantity()
{
	SetQuantity(Quantity - 1);
}

void APickupItem::SetMesh()
{
	if (EquipmentTypeID > 0 && EquipmentTypeID <= AssetMeshes.Num())
	{
		Mesh->SetStaticMesh(AssetMeshes[EquipmentTypeID - 1]);
		Mesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		Mesh->SetVisibility(true, true);
	}
}

#if WITH_EDITOR

void APickupItem::OnConstruction(const FTransform& Transform)
{
	SetMesh();
	SetQuantity(Quantity);
	Super::OnConstruction(Transform);
}

void APickupItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FString PropertyName = ((PropertyChangedEvent.Property != nullptr)
		                              ? PropertyChangedEvent.Property->GetFName()
		                              : NAME_None).ToString();
	const FString MemberPropertyName = ((PropertyChangedEvent.MemberProperty != nullptr)
		                                    ? PropertyChangedEvent.MemberProperty->GetFName()
		                                    : NAME_None).ToString();

	if (PropertyName == TEXT("EquipmentTypeID"))
	{
		if (EquipmentTypeID > 5 || EquipmentTypeID < 1)
		{
			EquipmentTypeID = 1;
		}
		SetMesh();
	}
	else
	{
		SetQuantity(Quantity);
	}
}

#endif
