/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#include "GameInstanceDatabaseSubsystem.h"
#include "CustomLogging.h"
#include "DbBase.h"
#include "SqliteGameDBSettings.h"

// forward declare static variables
UGameInstanceDatabaseSubsystem* UDbManagerStatics::DBSubSystem = nullptr;


/* The subsystem is being instantiated, we will attempt to:
		* Read settings supplied in the editor (USqliteGameDBSettings)
		* Retrieve the database content folder from settings
		* Check the folder exists
		* Create a database connection for each entry in the settings */
void UGameInstanceDatabaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogSqliteGameDB, Log, TEXT("Initializing the DatabaseSubSystem..."));

	const USqliteGameDBSettings* DatabaseSettings = GetDefault<USqliteGameDBSettings>();

	// Plugin will not function without valid settings values
	checkf(DatabaseSettings, TEXT("SqliteGameDBSettings were not found, or were corrupt. "));

	const FString DBFolder = !(DatabaseSettings->DatabaseContentFolder.IsEmpty())
		                         ? DatabaseSettings->DatabaseContentFolder
		                         : TEXT("Database");

	RootDbPath = FPaths::ProjectContentDir() + DBFolder + '/';

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	// Check the directory path is valid
	if (!FileManager.DirectoryExists(*RootDbPath))
	{
		if (DatabaseSettings->DatabaseContentFolder.IsEmpty())
		{
			UE_LOG(LogSqliteGameDB, Error, TEXT("Database default content path 'Database', not found."));
		}
		else
		{
			UE_LOG(LogSqliteGameDB, Error, TEXT("Database path retrieved from settings is invalid"));
		}

		IsDbDirValid = false;
	}
	else
	{
		IsDbDirValid = true;

		if (DatabaseSettings->DatabaseContentFolder.IsEmpty())
		{
			UE_LOG(LogSqliteGameDB, Log, TEXT("Database path using default value 'Database'"));
		}
		else
		{
			UE_LOG(LogSqliteGameDB, Log, TEXT("Database path using value from settings"));
		}
	}

	// If the check failed, log that the system is unavailable, and exit.
	if (!IsDbDirValid)
	{
		UE_LOG(LogSqliteGameDB, Error, TEXT("Previously reported faults prevent the DatabaseSubSystem from starting."));
		return;
	}

	/* At this point we are free to initialize any DatabaseBase derived classes
	 * that are specified in the custom settings. */
	for (const FGameDbConfig DbConfig : DatabaseSettings->DatabaseConnectionClasses)
	{
		const FString DbFilePath = RootDbPath + DbConfig.FileName;

		UDbBase* DatabaseConnection = NewObject<UDbBase>(this, DbConfig.DerivedClass);
		DatabaseConnection->Initialize(DbFilePath, DbConfig);
		DatabaseConnections.Add(DatabaseConnection);
	}

	UDbManagerStatics::Initialize(this);
}

void UGameInstanceDatabaseSubsystem::Deinitialize()
{
	UE_LOG(LogSqliteGameDB, Log, TEXT("Deinitializing the DatabaseSubSystem"));
	UE_LOG(LogSqliteGameDB, Log, TEXT("Closing open database connections..."));

	for (UDbBase* DbConnection : DatabaseConnections)
	{
		if (DbConnection)DbConnection->ConditionalBeginDestroy();
	}

	Super::Deinitialize();

	UE_LOG(LogSqliteGameDB, Log, TEXT("DatabaseSubSystem DeInitialized."));
}

#pragma region Status Indicators

UDbBase* UGameInstanceDatabaseSubsystem::GetDatabaseByFilename(FString DbFilename)
{
	for (UDbBase* Db : DatabaseConnections)
	{
		if (Db)
		{
			FString DbPath = Db->GetDbName();
			FString LeftStr;
			FString DbName;
			DbPath.Split("/", &LeftStr, &DbName, ESearchCase::CaseSensitive, ESearchDir::FromEnd);

			if (!DbName.IsEmpty() && DbName.Compare(DbFilename, ESearchCase::IgnoreCase) == 0)
			{
				return Db;
			}
		}
	}
	return nullptr;
}

bool UGameInstanceDatabaseSubsystem::IsDatabaseDirectoryValid()
{
	return IsDbDirValid;
}

UDbBase* UDbManagerStatics::GetDatabaseByFilename(FString DbFilename)
{
	/* The GameInstanceDatabaseSubsystem needs to have been spun up before we can call this. */
	check(DBSubSystem);
	UDbBase* Result = DBSubSystem->GetDatabaseByFilename(DbFilename);

	/* Not finding a database we expect to find is a critical error. */
	checkf(Result, TEXT("GetDatabaseByFilename failed to find the specified db"));
	return Result;
}

void UDbManagerStatics::Initialize(UGameInstanceDatabaseSubsystem* SubSystem)
{
	DBSubSystem = SubSystem;
}

#pragma endregion
