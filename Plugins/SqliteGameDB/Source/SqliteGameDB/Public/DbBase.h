/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DBSupport.h"
#include "GameDbConfig.h"
#include "SQLiteDatabase.h"
#include "DbBase.generated.h"

struct FGameDbAttachment;
class UDbStatement;
class UPreparedStatementGroup;
class UPreparedStatementManager;

/* The Parent Sqlite helper DB object */
UCLASS(Abstract, Blueprintable)
class SQLITEGAMEDB_API UDbBase : public UObject
{
	friend UDbStatement;
	friend UPreparedStatementGroup;
	friend UPreparedStatementManager;

public:
	UDbBase(const FObjectInitializer& ObjectInitializer);

	void Initialize(FString DatabaseFilePath, FGameDbConfig Config);

	/* This is marked 'final' so that inherited classes cannot alter it.
	 * It ensures that the required memory cleanup is performed even if a derived
	 * class 'forgets' to call the Super::BeginDestroy().
	 * Derived classes should override TearDown() for custom destruction code. */
	void BeginDestroy() override final;

	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Database Connection",
		meta = (DisplayName="Get Database Filename"))
	FString GetDbName() const;

protected:
	/* Default DB Schema name. */
	const FString SchemaMain = TEXT("MAIN");

	/* Pointer to the underlying SQLite Db object */
	FSQLiteDatabase* SqliteDb = nullptr;

	/* Folder beneath the Unreal 'Content' folder, containing db files. */
	FString DatabaseContentFolder;

	/* Full path to the DB file. */
	FString DbFilePath;

	/* Provides temp query functions and access to prepared statements. */
	UPROPERTY(BlueprintReadOnly, Category = "SQLite Database|Database Connection",
		meta = (DisplayName="Query Manager"))
	UPreparedStatementManager* QueryManager = nullptr;

	/* This function gives derived classes a place to do any specific initialization.
	 * It is the logical place to instantiate prepared statements, etc.*/
	virtual void Build(FString DatabaseFilePath, FGameDbConfig Config);

	/* This function is automatically called before the class is destroyed,
	 * you can use it in Blueprint derived classes for any custom cleanup your DB class might need. */
	virtual void TearDown();

	/* This function gives derived classes a place to do any specific initialization.
	 * It is the logical place to instantiate prepared statements, etc.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "SQLite Database|Database Connection",
		meta = (DisplayName="Startup"))
	void Startup(const FString& DatabaseFilePath,const FGameDbConfig& Config);


	
private:
	GENERATED_BODY()
};
