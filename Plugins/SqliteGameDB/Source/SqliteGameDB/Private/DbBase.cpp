/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#include "DbBase.h"
#include "DbStatement.h"
#include "CustomLogging.h"
#include "PreparedStatementManager.h"
#include "SqliteGameDBSettings.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

UDbBase::UDbBase(const FObjectInitializer& ObjectInitializer)
{
	const USqliteGameDBSettings* DatabaseSettings = GetDefault<USqliteGameDBSettings>();
	DatabaseContentFolder = FPaths::ProjectContentDir() + DatabaseSettings->DatabaseContentFolder;
}

void UDbBase::Initialize(FString DatabaseFilePath, FGameDbConfig Config)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	/* Check that the given filename actually exists */
	verifyf(FileManager.FileExists(*DatabaseFilePath), TEXT("The db file was not found."));

	DbFilePath = DatabaseFilePath;

	// All the basic checks return OK, time to try to connect to the Db...
	SqliteDb = new FSQLiteDatabase();

	verifyf(SqliteDb->Open(*DbFilePath, ESQLiteDatabaseOpenMode::ReadWrite),
	        TEXT("Attempt to open DB connection failed, reason: %s"),
	        *(SqliteDb->GetLastError()));

	UE_LOG(LogSqliteGameDB, Log, TEXT("Connection to DB opened successfully. %s"), *DbFilePath);
	
	QueryManager = NewObject<UPreparedStatementManager>();
	QueryManager->Initialize(this);

	// Do any initialization specific to the derived class...
	Build(DatabaseFilePath, Config);
	Startup(DatabaseFilePath, Config);
}

void UDbBase::BeginDestroy()
{
	TearDown();
	
	if(QueryManager)
	{
		QueryManager->ConditionalBeginDestroy();
		QueryManager = nullptr;
	}
	
	if (SqliteDb && SqliteDb->IsValid())
	{
		// Attempt to close the open database
		if (!SqliteDb->Close())
		{
			UE_LOG(LogSqliteGameDB, Warning, TEXT("Attempt to close DB connection reported failed, reason: %s"),
			       *(SqliteDb->GetLastError()));
		}
		else
			UE_LOG(LogSqliteGameDB, Log, TEXT("Closed connection to database: %s"), *DbFilePath);
		delete SqliteDb;
	}
	Super::BeginDestroy();
}

void UDbBase::Build(FString DatabaseFilePath, FGameDbConfig Config)
{
	//checkNoEntry();
}

void UDbBase::TearDown()
{
	//checkNoEntry();
}

FString UDbBase::GetDbName() const
{
	if (SqliteDb && SqliteDb->IsValid())
	{
		FString Filename = SqliteDb->GetFilename();
		return Filename;
	}

	return FString(TEXT(""));
}
