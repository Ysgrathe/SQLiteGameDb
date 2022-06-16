/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PreparedStatementGroup.generated.h"

class FSQLiteDatabase;
class FSQLitePreparedStatement;
class UDbBase;
class UDbStatement;

/* Represents a (optionally filtered) group of prepared statements. */
UCLASS()
class SQLITEGAMEDB_API UPreparedStatementGroup final : public UObject
{
public:
	/* StatementGroup must be initialized with a reference to a database. */
	void Initialize(UDbBase* InGameDb);

	/* Clean up this Statement group, releasing resources.
	 * (Destroying each statement and deleting it's FSqlitePreparedStatement.) */
	virtual void BeginDestroy() override;

	/* Fill this StatementGroup with queries from the database,
	 * optionally filtered by a tag prefix/suffix.
	 * if UseTagAsPrefix == false, OptionalTag will be treated as a suffix.
	 * Note that ANY Statements already in the group will be Destroyed first. */
	int32 FillFromDatabase(const FString InSchemaName = TEXT(""));

	/* Repeats the last 'FillFromDatabase', using the same parameters.
	 * This is simply a convenience method,
	 * intended for use when attaching/detaching databases. */
	int32 RefillFromDatabase();

	/* Immediately destroys all statements in this group,
	 * allowing attach/detach operations to function. */
	void ClearAllStatements();

	UDbStatement* AddStatement(const FString& Name, const FString& QuerySql);

	/* Attempts to find the prepared statement with the given key name in the internal
	 * prepared statement cache. Returns a pointer if found, or nullptr if not found. */
	UDbStatement* GetStatement(const FString& Name);

	/* Returns the count of prepared statements. */
	int32 Num() const { return Statements.Num(); }

private:
	/* Reference to the database owning these prepared statements. */
	FSQLiteDatabase* SqliteDb = nullptr;
	TWeakObjectPtr<UDbBase> Db = nullptr;

	/* Storage for the prepared statements. */
	UPROPERTY()
	TMap<FString, UDbStatement*> Statements;

	FString SchemaName;
	bool UsesSchemaName = false;

	GENERATED_BODY()

	const FString Q_HasQueries = TEXT("select count(*) as HasQueries from sqlite_master where name like 'Queries'");
	const FString Q_ListQueries = TEXT("select * from queries {0} order by Key");
	const FString Q_FilterQueries = TEXT(" where SchemaName = '{0}';");
};
