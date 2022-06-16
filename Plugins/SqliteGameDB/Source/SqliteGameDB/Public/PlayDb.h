/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "DbBase.h"
#include "LogInfo.h"
#include "UObject/Object.h"
#include "PlayDb.generated.h"

enum class EDbFilePurpose : uint8;
struct FGameDbAttachment;
struct FLogInfo;

/* Represents a 'split' sqlite database design, with a fixed 'main.db',
 * a player centric 'log.db',
 * and multiple 'play.db's representing save states, with a single 'working copy'.
 *
 * Most of the functionality this class provides centers around the 'log.db',
 * and managing the current working copy play.db - saving, loading, etc. */
UCLASS(Abstract)
class SQLITEGAMEDB_API USplitDbBase : public UDbBase
{
public:
	USplitDbBase(const FObjectInitializer& ObjectInitializer);

	/* Return if we are currently connected to a log.db. */
	bool IsConnectedLogDb();

	/* Return if we are currently connected to a play.db. */
	bool IsConnectedPlayDb();

	//virtual void Initialize(FString DatabaseFilePath, FGameDbConfig Config) override;

	int32 CountLogEntries(TArray<EPlayDbPurpose> FilterBy = {
		EPlayDbPurpose::AutoSave,
		EPlayDbPurpose::NewGame,
		EPlayDbPurpose::ManualSave,
		EPlayDbPurpose::QuickSave
	});

	/* Return a list of log entries, optionally limited in count, and filtered by purpose. */
	TArray<FLogInfo> ListLogEntries(int32 MaxResults = 10,
	                                TArray<EPlayDbPurpose> FilterBy = {
		                                EPlayDbPurpose::AutoSave,
		                                EPlayDbPurpose::NewGame,
		                                EPlayDbPurpose::ManualSave,
		                                EPlayDbPurpose::QuickSave
	                                });

	UFUNCTION(BlueprintCallable, Category = "SqliteGameDB|Persistence")
	TArray<FLogInfo> ListLogEntries(int32 MaxResults,
	                                TArray<EPlayDbPurpose> FilterBy, bool Unused);

	TArray<FLogInfo> ListLogEntriesPaged(int32 RecordsPerPage = 10, int32 PageNumber = 0,
	                                     TArray<EPlayDbPurpose> FilterBy = {
		                                     EPlayDbPurpose::AutoSave,
		                                     EPlayDbPurpose::NewGame,
		                                     EPlayDbPurpose::ManualSave,
		                                     EPlayDbPurpose::QuickSave
	                                     });

	UFUNCTION(BlueprintCallable, Category = "SqliteGameDB|Persistence")
	TArray<FLogInfo> ListLogEntriesPaged(int32 RecordsPerPage, int32 PageNumber,
	                                     TArray<EPlayDbPurpose> FilterBy, bool Unused);

	UFUNCTION(BlueprintCallable, Category = "SqliteGameDB|Persistence")
	FString GetCurrentLogDbPath() const;

	UFUNCTION(BlueprintCallable, Category = "SqliteGameDB|Persistence")
	FString GetCurrentPlayDbPath() const;

protected:
	virtual void Build(FString DatabaseFilePath, FGameDbConfig Config) override;
	virtual void TearDown() override;

	/* Attempt to connect to the indicated LogDb,
	 * If no LogDb is found at the specified path,
	 * a new one is created from the template. */
	void ConnectLogDb();

	/* Attempt to disconnect the logdb.
	 * This is only required if the player changes. */
	void DisconnectLogDb();

	/* Attempt to load a PlayDb file.
	 * The LogIndex is both the ROWID from the log file,
	 * and the filename of the playdb instance. */
	UFUNCTION(BlueprintCallable, Category = "SqliteGameDB|Persistence")
	bool ConnectPlayDb(int32 LogIndex);

	/* Attempt to disconnect the current playdb.
	 * Probably only used in situations where you return to the main menu. */
	void DisconnectPlayDb();

	/* Attempt to create a NEW play db.
	 * This makes a copy of the playdb template, and registers it with the logdb.
	 * Return value is the new index, or -1 if an error occurred.
	 * This function is generally only used right at the start of a new playthrough. */
	UFUNCTION(BlueprintCallable, Category = "SqliteGameDB|Persistence")
	bool CreatePlayDbFromTemplate(FString Title, FString Additional,
	                              EPlayDbPurpose Purpose, bool SetAsWorkingCopy = true);

	/* This function will try to create a copy of the current 'working copy' playdb,
	 * and return the new index, or -1 if an error occurred.
	 * This is the basis of a 'save game'. */
	bool CreatePlayDbFromCurrent(FString Title, FString Additional,
	                             EPlayDbPurpose Purpose);

	/* This simply calls CreatePlayDbFromCurrent(), supplying default values. */
	UFUNCTION(BlueprintCallable, Category = "SqliteGameDB|Persistence")
	bool CreateQuickSavePlayDb(FString Additional);

	/* Sets the 'working copy' to the most recent playdb file.*/
	UFUNCTION(BlueprintCallable, Category = "SqliteGameDB|Persistence")
	bool ConnectResumePlayDb();


private:
	/* Full path to the DB files... */
	FString LogTemplateDbFilePath;
	FString PlayTemplateDbFilePath;
	FString PlayStagingDbFilePath;


	const TArray<EPlayDbPurpose> FilterByAll = {
		EPlayDbPurpose::AutoSave,
		EPlayDbPurpose::NewGame,
		EPlayDbPurpose::ManualSave,
		EPlayDbPurpose::QuickSave
	};

	/* Path to the actual log.db file we are using .*/
	FString InstancedLogDbPath;

	/* Path to the 'save' file, which was cloned to make the 'working copy' */
	FString CurrentSourcePlayDbPath;

	/* Path to the working copy, always the same path, although the file changes. */
	FString WorkingCopyPlayDbPath;

	int32 CreatePlayDbFromSource(FString Source, FString Title, FString Additional, EPlayDbPurpose Purpose);

	bool FindAttachmentOfType(TArray<FGameDbAttachment>& Source, EDbFilePurpose Purpose,
	                          FGameDbAttachment& OutAttachment) const;

	const FText QuickSaveTitle = NSLOCTEXT("LOG", "QUICKSAVE_TITLE", "QUICKSAVE");
	const FText AutoSaveTitle = NSLOCTEXT("LOG", "AUTOSAVE_TITLE", "AUTOSAVE");

	const FString TemplatePath = TEXT("{0}/{1}");
	const FString PlayInstancePath = TEXT("{SaveDir}Play_{LogIndex}.db");

	/* DB Schema names. */
	const FString SchemaLog = TEXT("LOG");
	const FString SchemaPlay = TEXT("PLAY");

	/* Common parameters. */
	const FString P_LogID = TEXT("@LogID");
	const FString P_Purpose = TEXT("@Purpose");
	const FString P_MaxRecords = TEXT("@MaxRecords");
	const FString P_RecordsPerPage = TEXT("@RecordsPerPage");
	const FString P_PageNumber = TEXT("@PageNumber");
	const FString P_Additional = TEXT("@Additional");
	const FString P_Title = TEXT("@Title");

	/* SELECT Statements. */
	const FString LOG_GetLogFilesCount = TEXT("GetLogFilesCount"); // @Purpose
	const FString LOG_ListLogFiles = TEXT("ListLogFiles"); // @Purpose, @MaxRecords
	const FString LOG_ListLogFilesPaged = TEXT("ListLogFilesPaged"); // @Purpose, @RecordsPerPage, @PageNumber
	const FString LOG_GetLatestLog = TEXT("GetLatestLog");
	const FString LOG_GetLatestQuickSave = TEXT("GetLatestQuickSave");

	/* ACTION Statements. */
	const FString LOG_AddNewLog = TEXT("AddNewLog"); // @Title, @Additional, @Purpose
	const FString LOG_DeleteLog = TEXT("DeleteLog"); // @LogID
	const FString LOG_DeleteOldQuickSaves = TEXT("DeleteOldQuickSaves"); // @LogID

	const FString LOG_CleanLog = TEXT("CleanLog");
	const FString PLAY_CleanPlay = TEXT("CleanPlay");


	GENERATED_BODY()
};
