/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#pragma once

#include "CoreMinimal.h"
#include "DbBase.h"
#include "TutorialDataAccessLayer.generated.h"

class UDbEquipmentComponent;
/* This is the Tutorial Data Access Layer class. */
UCLASS()
class TUTORIALDEMO_API UTutorialDataAccessLayer : public UDbBase
{
public:
	UFUNCTION(BlueprintCallable, Category = "DAL|Level",
		meta = (DisplayName="Is Level Managed By Database"))
	bool IsLevelManaged(FString LevelName);

	/* Returns the ID number for a level, given its name.
	 * or -1 if not found. */
	int32 GetLevelID(FString LevelName);

	/* Create a record, or update an existing record, for an equipment item. */
	void SaveEquipmentItem(FString InstanceID, int32 EquipmentID, int32 LevelID, int32 Quantity);

	/* Returns the quantity value for an equipment item, or -1 if it cant be found. */
	int32 LoadEquipmentItem(FString InstanceID, int32 LevelID);

protected:
	virtual void Build(FString DatabaseFilePath, FGameDbConfig Config) override;
	virtual void TearDown() override;

private:
	/* Names of queries used, and common parameter names. */
	const FString Q_IsLevelManaged = TEXT("IsLevelManaged");
	const FString Q_GetLevelID = TEXT("GetLevelID");
	const FString Q_UpsertEquipmentInstance = TEXT("UpsertEquipmentInstance");
	const FString Q_GetEquipmentDetails = TEXT("GetEquipmentDetails");
	const FString P_LevelName = TEXT("@LevelName");
	const FString P_InstanceID = TEXT("@InstanceID");
	const FString P_EquipmentID = TEXT("@EquipmentID");
	const FString P_LevelID = TEXT("@LevelID");
	const FString P_Quantity = TEXT("@Quantity");


	GENERATED_BODY()
};
