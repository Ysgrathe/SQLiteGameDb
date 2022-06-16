/* © Copyright 2022 Graham Chabas, All Rights Reserved. */
#pragma once

#include "CoreMinimal.h"
#include "GameDbConfig.h"
#include "SQLiteDatabase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameInstanceDatabaseSubsystem.generated.h"

class UDbBase;
class UGameDbBase;

/* Manager class for SQLite databases.
It is implemented as a GameInstanceSubSystem, so that it is always available during gameplay,
and that it can use the DeInitialise() method to gracefully dispose of unmanaged resources. */
UCLASS()
class SQLITEGAMEDB_API UGameInstanceDatabaseSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	// Path to the unreal 'content' sub-folder holding db files
	FString RootDbPath;

	// Some flags set to indicate if the directory and expected files are present
	bool IsDbDirValid = false;

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	/* When this subsystem is initialized,
	  it reads the classes stored in the DatabaseConnectionClasses setting.
	  For each of those classes, it creates and instance,
	  and stores their pointers here */
	UPROPERTY()
	TArray<UDbBase*> DatabaseConnections;

	UFUNCTION(BlueprintCallable,
		Category = "SQLite Database|Database Subsystem",
		meta = (DisplayName = "Get Database By Filename"))
	UDbBase* GetDatabaseByFilename(FString DbFilename);

#pragma region Status Indicators

	UFUNCTION()
	bool IsDatabaseDirectoryValid();

#pragma endregion
};

/* This is a utility class which gives easy access to the DB related functions. */
UCLASS()
class SQLITEGAMEDB_API UDbManagerStatics : public UBlueprintFunctionLibrary
{
	friend UGameInstanceDatabaseSubsystem;

public:
	UFUNCTION(BlueprintCallable,
		Category = "SQLite Database|Database Subsystem",
		meta = (DisplayName = "Get Database By Filename"))
	static UDbBase* GetDatabaseByFilename(FString DbFilename);

	UFUNCTION(BlueprintCallable,
		Category = "SQLite Database|Database Subsystem",
		meta = (DisplayName = "Database Subsystem Valid"))
	static bool DbSubsystemIsValid() { return DBSubSystem != nullptr; };


private:
	GENERATED_BODY()

	static void Initialize(UGameInstanceDatabaseSubsystem* SubSystem);
	static void DeInitialize() { DBSubSystem = nullptr; };
	static UGameInstanceDatabaseSubsystem* DBSubSystem;
};
