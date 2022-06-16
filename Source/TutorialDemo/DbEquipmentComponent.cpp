/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#include "DbEquipmentComponent.h"
#include "GameInstanceDatabaseSubsystem.h"
#include "PickupItem.h"
#include "TutorialDataAccessLayer.h"

UDbEquipmentComponent::UDbEquipmentComponent()
{
}

void UDbEquipmentComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	/* If we are in the editor, we wont have the GameInstanceDatabaseSubsystem,
	 * So we will need to run up a connection manually */

	if (UDbManagerStatics::DbSubsystemIsValid())
	{
		Dal = Cast<UTutorialDataAccessLayer>(UDbManagerStatics::GetDatabaseByFilename(TEXT("AlienWarmonger.db")));
	}
	else
	{
	}
}

FString UDbEquipmentComponent::GetID()
{
	return ID;
}

void UDbEquipmentComponent::SaveState()
{
	if (Parent && Dal)
	{
		
	}
}

void UDbEquipmentComponent::LoadState()
{
	if (Parent && Dal)
	{
		
	}
}

void UDbEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	/* Get the actor that owns this component. */
	Parent = Cast<APickupItem>(GetOwner());
	if (Parent)
	{
		ID = Parent->GetName();
		Dal = Cast<UTutorialDataAccessLayer>(UDbManagerStatics::GetDatabaseByFilename(TEXT("AlienWarmonger.db")));
	}
}
