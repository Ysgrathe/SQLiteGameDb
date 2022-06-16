/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#include "PreparedStatementGroup.h"
#include "CustomLogging.h"
#include "DbBase.h"
#include "DbStatement.h"
#include "PreparedStatementManager.h"
#include "SQLitePreparedStatement.h"


void UPreparedStatementGroup::Initialize(UDbBase* InGameDb)
{
	check(InGameDb);
	Db = InGameDb;
	SqliteDb = Db->SqliteDb;
}

void UPreparedStatementGroup::BeginDestroy()
{
	ClearAllStatements();
	Super::BeginDestroy();
}

int32 UPreparedStatementGroup::FillFromDatabase(const FString InSchemaName)
{
	SchemaName = InSchemaName;
	return RefillFromDatabase();
}

int32 UPreparedStatementGroup::RefillFromDatabase()
{
	int32 Result = 0;

	/* Check that the database actually has a 'queries' table. */
	FQueryResult QrHasQueries = Db->QueryManager->RunTempSelectQuery(Q_HasQueries);

	if (QrHasQueries.Rows.Num() > 0 && QrHasQueries.Rows[0].Fields[0].IntVal == 1)
	{
		FString ListFilteredQueries;

		/* If a SchemaName was provided, add a where clause to filter only the desired queries. */
		if (SchemaName != TEXT(""))
		{
			/* Replace the parameter with a 'where' clause. */
			FString WhereClause = FString::Format(*Q_FilterQueries, {SchemaName});
			ListFilteredQueries = FString::Format(*Q_ListQueries, {WhereClause});
		}
		else
		{
			/* Replace the parameter with nothing. */
			ListFilteredQueries = FString::Format(*Q_ListQueries, {TEXT("")});
		}

		/* Get the query data from the database. */
		const FQueryResult QrListQueries = Db->QueryManager->RunTempSelectQuery(ListFilteredQueries);
		Result = QrListQueries.Rows.Num();

		/* Add a prepared statement for each returned query. */
		for (int i = 0; i < Result; ++i)
		{
			FQueryResultRow ThisRow = QrListQueries.Rows[i];
			AddStatement(ThisRow.Fields[0].StrVal, ThisRow.Fields[3].StrVal);
		}
	}

	return Result;
}

UDbStatement* UPreparedStatementGroup::AddStatement(const FString& Name, const FString& QuerySql)
{
	UDbStatement* NewStatement = NewObject<UDbStatement>(this, UDbStatement::StaticClass());
	NewStatement->Initialize(SqliteDb, QuerySql);
	Statements.Add(Name, NewStatement);
	return NewStatement;
}

UDbStatement* UPreparedStatementGroup::GetStatement(const FString& Name)
{
	return *Statements.Find(Name);
}

void UPreparedStatementGroup::ClearAllStatements()
{
	/* Call BeginDestroy() on any DbStatements*/
	for (const TTuple<FString, UDbStatement*> Pair : Statements)
	{
		Pair.Value->ConditionalBeginDestroy();
	}

	/* Empty the storage */
	Statements.Empty();
}
