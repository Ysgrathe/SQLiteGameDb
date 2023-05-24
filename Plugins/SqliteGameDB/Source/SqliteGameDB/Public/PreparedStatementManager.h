/* © Copyright 2022 Graham Chabas, All Rights Reserved. */
#pragma once

#include "CoreMinimal.h"
#include "DbStatement.h"
#include "DBSupport.h"
#include "PreparedStatementGroup.h"
#include "UObject/Object.h"
#include "PreparedStatementManager.generated.h"

class UDbBase;

/* Aggregates different collections of prepared statements under one collection. */
UCLASS()
class SQLITEGAMEDB_API UPreparedStatementManager : public UObject
{
public:
	virtual void Initialize(UDbBase* InDb);
	virtual void BeginDestroy() override;

	/* Attempt to load all queries from the database,
	 * and store them in a default collection (named 'DEFAULT_QUERIES').
	 * Return value is the number of statements loaded. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Load All Queries Into Statements"))
	int32 LoadStatements();

	/* Attempt to load queries from the database, and store them in a named collection.
	 * QueryTag allows only queries who's KeyName either starts or ends
	 * with the given string to be returned.
	 * Return value is the number of statements loaded. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Load Group Queries Into Statements"))
	int32 LoadStatementsIntoGroup(const FString SchemaName);

	/* Destroys the underlying sqlite FSQLitePreparedStatement items
	 * for all loaded statements. This allows Attach/Detach operations to be done.
	 * Use ReconnectStatements() to recreate them. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Discard All Statements"))
	void DisconnectStatements();

	/* Destroys the underlying sqlite FSQLitePreparedStatement items
	 * for all loaded statements in a named group.
	 * This allows Attach/Detach operations to be done.
	 * Use ReconnectGroupStatements() to recreate them. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Discard Group Statements"))
	void DisconnectGroupStatements(const FString GroupName);

	/* Once an 'attach/detach' operation is complete,
	 * use this to regenerate the underlying FSQLitePreparedStatements. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Reconnect All Statements"))
	void ReconnectStatements();

	/* Once an 'attach/detach' operation is complete,
	 * use this to regenerate the underlying FSQLitePreparedStatements for a named group. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Reconnect Group Statements"))
	void ReconnectGroupStatements(const FString GroupName);

	/* Connect a separate database file to the main database.
	 * Return value indicates if attachment was successful. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Attach Database"))
	bool AttachDatabase(const FString DatabaseFilePath, const FString SchemaName) const;

	/* Disconnect a previously connected database file. 
	 * Return value indicates if detachment was successful.
	 * Ok to use if not attached, just returns 'true'. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Detach Database"))
	bool DetachDatabase(const FString SchemaName) const;

	/* Returns true if there is a database attached with the given schema name. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Is Schema Attached"))
	bool IsSchemaAttached(const FString SchemaName) const;

	/* Returns a list of schema names other than the default 'main' and 'temp' */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="List Attached Schemas"))
	TArray<FString> ListAttachedSchemas() const;

	/* Looks for the given statement key in the specified group, and returns the statement.
	 * If either the group does not exist, or the statement isn't found, nullptr is returned. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Find Group Statement"))
	UDbStatement* FindStatementInGroup(const FString GroupName, const FString StatementName) const;

	/* Looks for the given statement key in all groups, and returns the first occurrence.
	 * If the statement isn't found, nullptr is returned. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Find Statement"))
	UDbStatement* FindStatement(const FString StatementName) const;

	/* Creates a new prepared statement (UDbStatement) and adds it to the default group. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Create Statement"))
	UDbStatement* CreateStatement(const FString StatementName, const FString QuerySql);

	/* Creates a new prepared statement (UDbStatement) and adds it to the specified group,
	 * creating a new group if it does not already exist. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Create Group Statement"))
	UDbStatement* CreateStatementInGroup(const FString GroupName, const FString StatementName, const FString QuerySql);

	/* Executes an SQL select string and returns any resultant data.
	 * To do this it creates a PreparedStatement which it disposes of immediately.
	 * Any query 'parameter' values need to be included as literals in the SQL string. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Execute Temporary Select Query"))
	FQueryResult RunTempSelectQuery(const FString SqlToRun) const;

	/* Executes an SQL select statement and returns the value contained
	 * in the first field of the first row in the resultset.
	 * To do this it creates a PreparedStatement which it disposes of immediately.
	 * Any query 'parameter' values need to be included as literals in the SQL string. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Execute Temporary Scalar Query"))
	FQueryResultField RunTempScalarQuery(const FString SqlToRun) const;

	/* Execute an SQL statement, returning no data. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Execute Temporary Action Query"))
	void RunTempActionQuery(const FString SqlToRun) const;

	/* Initiates a SQLite transaction. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Begin Transaction"))
	void BeginTransaction();

	/* Commits a pending SQLite transaction. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Commit Transaction"))
	void CommitTransaction();	

	/* Rolls back a pending SQLite transaction */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Statement Manager",
		meta = (DisplayName="Rollback Transaction"))
	void RollbackTransaction();
	
private:
	TWeakObjectPtr<UDbBase> Db = nullptr;

	UPROPERTY()
	TMap<FString, UPreparedStatementGroup*> QueryGroups;

	const FString DefaultGroupName = TEXT("DEFAULT_QUERIES");

	const FString Q_TranBegin = TEXT("BEGIN;");
	const FString Q_TranCommit = TEXT("COMMIT;");
	const FString Q_TranRollback = TEXT("ROLLBACK;");
		
	const FString Q_AttachDb = TEXT("ATTACH DATABASE @DbFileName AS @SchemaName;");
	const FString Q_DetachDb = TEXT("DETACH DATABASE @SchemaName;");
	const FString Q_GetSchemas = TEXT(
		"SELECT name FROM pragma_database_list WHERE name NOT IN ('main', 'temp') order by name");
	const FString Q_SchemaExists = TEXT("SELECT count(*) FROM pragma_database_list WHERE name = @SchemaName");

	const FString P_DbFileName = TEXT("@DbFileName");
	const FString P_SchemaName = TEXT("@SchemaName");


	GENERATED_BODY()
};
