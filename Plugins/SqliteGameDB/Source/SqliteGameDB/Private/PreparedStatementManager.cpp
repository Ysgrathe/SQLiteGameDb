/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#include "PreparedStatementManager.h"
#include "DbBase.h"
#include "DbStatement.h"


void UPreparedStatementManager::Initialize(UDbBase* InDb)
{
	Db = InDb;
}

void UPreparedStatementManager::BeginDestroy()
{
	for (TTuple<FString, UPreparedStatementGroup*> QueryGroup : QueryGroups)
	{
		QueryGroup.Value->ConditionalBeginDestroy();
	}
	Super::BeginDestroy();
}

int32 UPreparedStatementManager::LoadStatements()
{
	return LoadStatementsIntoGroup(DefaultGroupName);
}

int32 UPreparedStatementManager::LoadStatementsIntoGroup(const FString SchemaName)
{
	UPreparedStatementGroup* NewGroup = NewObject<UPreparedStatementGroup>(this);
	verify(NewGroup && Db.IsValid());

	/* Do we already have a group with this name? */
	if (QueryGroups.Contains(SchemaName))
	{
		/* Remove existing group with same name. */
		UPreparedStatementGroup* ExistingGroup = QueryGroups.FindAndRemoveChecked(SchemaName);
		ExistingGroup->ConditionalBeginDestroy();
	}

	NewGroup->Initialize(Db.Get());

	/* If we are using the default group name, pass an empty string to the function,
	 * Otherwise use the schema name provided. */
	const FString ActualSchemaName = SchemaName == DefaultGroupName ? TEXT("") : SchemaName;

	/* Did we find any queries? */
	if (NewGroup->FillFromDatabase(ActualSchemaName) > 0)
	{
		QueryGroups.Add(SchemaName, NewGroup);
		return NewGroup->Num();
	}

	return 0;
}

void UPreparedStatementManager::DisconnectStatements()
{
	DisconnectGroupStatements(DefaultGroupName);
}

void UPreparedStatementManager::DisconnectGroupStatements(const FString GroupName)
{
	if (QueryGroups.Contains(GroupName))
	{
		QueryGroups[GroupName]->RefillFromDatabase();
	}
}

void UPreparedStatementManager::ReconnectStatements()
{
	ReconnectGroupStatements(DefaultGroupName);
}

void UPreparedStatementManager::ReconnectGroupStatements(const FString GroupName)
{
	if (QueryGroups.Contains(GroupName))
	{
		QueryGroups[GroupName]->RefillFromDatabase();
	}
}

bool UPreparedStatementManager::AttachDatabase(const FString DatabaseFilePath, const FString SchemaName) const
{
	/* If  attached to a schema with the given name, just return 'false'.
	 * As it must be manually detached first. */
	if (IsSchemaAttached(SchemaName)) return false;

	/* Make a temporary prepared statement. */
	UDbStatement* TempStatement = NewObject<UDbStatement>();
	TempStatement->Initialize(Db->SqliteDb, Q_AttachDb);

	/* Bind parameter values */
	TempStatement->SetBindingValue(P_DbFileName, DatabaseFilePath);
	TempStatement->SetBindingValue(P_SchemaName, SchemaName);

	/* Execute it. */
	const bool Result = TempStatement->ExecuteAction();

	/* Clean up. */
	TempStatement->ConditionalBeginDestroy();

	return Result;
}

bool UPreparedStatementManager::DetachDatabase(const FString SchemaName) const
{
	/* If not attached to a schema with the given name, just return 'true'. */
	if (!IsSchemaAttached(SchemaName)) return true;

	/* Make a temporary prepared statement. */
	UDbStatement* TempStatement = NewObject<UDbStatement>();
	TempStatement->Initialize(Db->SqliteDb, Q_DetachDb);

	/* Bind parameter values */
	TempStatement->SetBindingValue(P_SchemaName, SchemaName);

	/* Execute it. */
	const bool Result = TempStatement->ExecuteAction();

	/* Clean up. */
	TempStatement->ConditionalBeginDestroy();

	return Result;
}

bool UPreparedStatementManager::IsSchemaAttached(const FString SchemaName) const
{
	/* Make a temporary prepared statement. */
	UDbStatement* TempStatement = NewObject<UDbStatement>();
	TempStatement->Initialize(Db->SqliteDb, Q_SchemaExists);

	/* Bind parameter values */
	TempStatement->SetBindingValue(P_SchemaName, SchemaName);

	/* Execute it. */
	const bool Result = TempStatement->ExecuteScalar().BoolVal;

	/* Clean up. */
	TempStatement->ConditionalBeginDestroy();

	return Result;
}

TArray<FString> UPreparedStatementManager::ListAttachedSchemas() const
{
	TArray<FString> Results;

	const FQueryResult QResults = RunTempSelectQuery(Q_SchemaExists);

	for (int i = 0; i < QResults.Rows.Num(); ++i)
	{
		FQueryResultRow ThisRow = QResults.Rows[i];
		Results.Add(ThisRow.Fields[0].StrVal);
	}

	return Results;
}

UDbStatement* UPreparedStatementManager::FindStatementInGroup(const FString GroupName,
                                                              const FString StatementName) const
{
	if (QueryGroups.Contains(GroupName))
	{
		return QueryGroups[GroupName]->GetStatement(StatementName);
	}
	return nullptr;
}

UDbStatement* UPreparedStatementManager::FindStatement(const FString StatementName) const
{
	for (TTuple<FString, UPreparedStatementGroup*> GroupPair : QueryGroups)
	{
		UPreparedStatementGroup* Group = GroupPair.Value;
		if (UDbStatement* Statement = Group->GetStatement(StatementName))
		{
			return Statement;
		}
	}
	return nullptr;
}

UDbStatement* UPreparedStatementManager::CreateStatement(const FString StatementName,
                                                         const FString QuerySql)
{
	return CreateStatementInGroup(DefaultGroupName, StatementName, QuerySql);
}

UDbStatement* UPreparedStatementManager::CreateStatementInGroup(const FString GroupName,
                                                                const FString StatementName,
                                                                const FString QuerySql)
{
	UPreparedStatementGroup* NewGroup = nullptr;

	/* Create a new group if it doesnt already exist. */
	if (QueryGroups.Contains(GroupName))
	{
		/* Find returns a pointer to the value, which is a pointer itself,
		 * so we need to dereference it to get the actual pointer. */
		NewGroup = *QueryGroups.Find(GroupName);
	}
	else
	{
		NewGroup = NewObject<UPreparedStatementGroup>(this);
		NewGroup->Initialize(Db.Get());
		QueryGroups.Add(GroupName, NewGroup);
	}

	if (NewGroup)
	{
		return NewGroup->AddStatement(StatementName, QuerySql);
	}

	/* If any issues occurred, return nothing. */
	return nullptr;
}

FQueryResultField UPreparedStatementManager::RunTempScalarQuery(const FString SqlToRun) const
{
	FQueryResult Results = RunTempSelectQuery(SqlToRun);
	if (Results.Rows.Num() > 0)
	{
		return Results.Rows[0].Fields[0];
	}

	FQueryResultField NoData;
	NoData.Type = EDbValueType::Null;
	return NoData;
}

void UPreparedStatementManager::RunTempActionQuery(const FString SqlToRun) const
{
	Db->SqliteDb->Execute(*SqlToRun);
}

FQueryResult UPreparedStatementManager::RunTempSelectQuery(const FString SqlToRun) const
{
	UDbStatement* Temp = NewObject<UDbStatement>();
	Temp->Initialize(Db->SqliteDb, SqlToRun);
	FQueryResult Results = Temp->ExecuteSelect();
	Temp->ConditionalBeginDestroy();

	return Results;
}
