/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#include "TutorialDataAccessLayer.h"
#include "PreparedStatementManager.h"

#pragma region Lifetime Methods

void UTutorialDataAccessLayer::Build(FString DatabaseFilePath, FGameDbConfig Config)
{
	QueryManager->LoadStatements();
}

void UTutorialDataAccessLayer::TearDown()
{
}

#pragma endregion

bool UTutorialDataAccessLayer::IsLevelManaged(FString LevelName)
{
	if (UDbStatement* Statement = QueryManager->FindStatement(Q_IsLevelManaged))
	{
		Statement->SetBindingValue(P_LevelName, LevelName);

		FQueryResultField Result = Statement->ExecuteScalar();
		return Result.BoolVal;
	}
	return false;
}

int32 UTutorialDataAccessLayer::GetLevelID(FString LevelName)
{
	if (UDbStatement* Statement = QueryManager->FindStatement(Q_GetLevelID))
	{
		Statement->SetBindingValue(P_LevelName, LevelName);

		FQueryResultField Result = Statement->ExecuteScalar();
		return Result.IntVal;
	}
	return -1;
}

void UTutorialDataAccessLayer::SaveEquipmentItem(FString InstanceID, int32 EquipmentID, int32 LevelID,
                                                 int32 Quantity)
{
	if (UDbStatement* PsSave = QueryManager->FindStatement(Q_UpsertEquipmentInstance))
	{
		PsSave->SetBindingValue(P_InstanceID, InstanceID);
		PsSave->SetBindingValue(P_EquipmentID, EquipmentID);
		PsSave->SetBindingValue(P_LevelID, LevelID);
		PsSave->SetBindingValue(P_Quantity, Quantity);
		PsSave->ExecuteAction();
	}
}

int32 UTutorialDataAccessLayer::LoadEquipmentItem(FString InstanceID, int32 LevelID)
{
	if (UDbStatement* PsLoad = QueryManager->FindStatement(Q_GetEquipmentDetails))
	{
		PsLoad->SetBindingValue(P_InstanceID, InstanceID);
		PsLoad->SetBindingValue(P_LevelID, LevelID);
		return PsLoad->ExecuteScalar().IntVal;
	}
	return -1;
}
