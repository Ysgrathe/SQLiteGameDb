/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#include "PlayDb.h"
#include "CustomLogging.h"
#include "GameDbConfig.h"
#include "PreparedStatementManager.h"
#include "SqliteGameDBSettings.h"

USplitDbBase::USplitDbBase(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void USplitDbBase::Build(FString DatabaseFilePath, FGameDbConfig Config)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FGameDbAttachment LogAttachment;
	FindAttachmentOfType(Config.Attachments, EDbFilePurpose::LogTemplate, LogAttachment);

	FGameDbAttachment PlayAttachment;
	FindAttachmentOfType(Config.Attachments, EDbFilePurpose::PlayTemplate, PlayAttachment);

	verifyf((LogAttachment.Purpose!=EDbFilePurpose::None && PlayAttachment.Purpose!= EDbFilePurpose::None),
	        TEXT("Missing log/play template database settings for a playdb class."));

	// TODO: Fix this when you are feeling less stupid!
	/* Fiasco just to concatenate a '/' between a couple of strings... but the format might change. */
	LogTemplateDbFilePath = FString::Format(*TemplatePath, {DatabaseContentFolder, LogAttachment.FileName});
	PlayTemplateDbFilePath = FString::Format(*TemplatePath, {DatabaseContentFolder, PlayAttachment.FileName});
	PlayStagingDbFilePath = FString::Format(*PlayInstancePath,
	                                        TMap<FString, FStringFormatArg>{
		                                        {TEXT("SaveDir"), FPaths::ProjectSavedDir()},
		                                        {TEXT("LogIndex"), TEXT("STAGING")}
	                                        });
	WorkingCopyPlayDbPath = FString::Format(*PlayInstancePath,
	                                        TMap<FString, FStringFormatArg>{
		                                        {TEXT("SaveDir"), FPaths::ProjectSavedDir()},
		                                        {TEXT("LogIndex"), TEXT("WC")}
	                                        });

	QueryManager->LoadStatementsIntoGroup(SchemaMain);

	/* Check that the given filenames actually exist as files in storage. */
	verifyf(FileManager.FileExists(*LogTemplateDbFilePath), TEXT("The 'Log Template' db file was not found."));
	verifyf(FileManager.FileExists(*PlayTemplateDbFilePath), TEXT("The 'Play Template' db file was not found."));

	/* Immediately connect to the logdb - there is only one per 'player'. */
	ConnectLogDb();
}

void USplitDbBase::TearDown()
{
	Super::TearDown();
}

void USplitDbBase::ConnectLogDb()
{
	/* Do we already have an instance of the LogDb?
	 * This is player specific and located in the save game directory. */
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	InstancedLogDbPath = FPaths::ProjectSavedDir() + TEXT("Log.db");

	if (!FileManager.FileExists(*InstancedLogDbPath))
	{
		/* No log.db file found at the indicated location, so create one. */
		verifyf(FileManager.CopyFile(*InstancedLogDbPath, *LogTemplateDbFilePath),
		        TEXT("Unable to copy log template file to destination directory."));
	}

	if (QueryManager->AttachDatabase(InstancedLogDbPath, SchemaLog))
	{
		QueryManager->LoadStatementsIntoGroup(SchemaLog);

		/* Clean the log DB. */
		UDbStatement* qCleanLog = QueryManager->FindStatementInGroup(SchemaLog, LOG_CleanLog);
		qCleanLog->ExecuteAction();
	}
}

void USplitDbBase::DisconnectLogDb()
{
	QueryManager->DisconnectGroupStatements(SchemaLog);
	QueryManager->DetachDatabase(SchemaLog);
}

bool USplitDbBase::ConnectPlayDb(int32 LogIndex)
{
	/* What we need to do here, is create a COPY of the indicated playdb, and connect to that.
	 * We only ever connect to clones of saved files, as we will want to make changes to them,
	 * and dont want to change the original.
	 * This clone is called the *Working Copy* and is always the same filename. */

	/* Build the path to the source file. */
	CurrentSourcePlayDbPath = FString::Format(*PlayInstancePath,
	                                          TMap<FString, FStringFormatArg>{
		                                          {TEXT("SaveDir"), FPaths::ProjectSavedDir() + TEXT("SavedGames/")},
		                                          {TEXT("LogIndex"), FString::FromInt(LogIndex)}
	                                          });

	/* Does a play file for the given index actually exist in the file system? */
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (!FileManager.FileExists(*CurrentSourcePlayDbPath))
	{
		UE_LOG(LogSqliteGameDB, Warning, TEXT("Indexed PlayDb file not found. %s"), *CurrentSourcePlayDbPath);
		return false;
	}

	/* Detach from the current PlayDb, if there is one. */
	verifyf(QueryManager->DetachDatabase(SchemaPlay),
	        TEXT("Unable to detatch from the current playdb. reason: %s"), *SqliteDb->GetLastError());

	/* Delete the current working copy file, if there is one. */
	if (FileManager.FileExists(*WorkingCopyPlayDbPath))
	{
		verifyf(FileManager.DeleteFile(*WorkingCopyPlayDbPath),
		        TEXT("Unable to delete the current working copy playdb."));
	}

	/* Clone the selected file to the working copy. */
	verifyf(FileManager.CopyFile(*WorkingCopyPlayDbPath, *CurrentSourcePlayDbPath ),
	        TEXT("Unable to make a working copy of playdb."));

	/* Attach to the new one. */
	if (QueryManager->AttachDatabase(WorkingCopyPlayDbPath, SchemaPlay))
	{
		QueryManager->LoadStatementsIntoGroup(SchemaPlay);
		return true;
	}
	return false;
}

void USplitDbBase::DisconnectPlayDb()
{
	QueryManager->DisconnectGroupStatements(SchemaPlay);
	QueryManager->DetachDatabase(SchemaPlay);
}

bool USplitDbBase::IsConnectedLogDb()
{
	return QueryManager->IsSchemaAttached(SchemaLog);
}

bool USplitDbBase::IsConnectedPlayDb()
{
	return QueryManager->IsSchemaAttached(SchemaPlay);
}

int32 USplitDbBase::CountLogEntries(TArray<EPlayDbPurpose> FilterBy)
{
	UDbStatement* PsLogCount = QueryManager->FindStatementInGroup(SchemaLog, LOG_GetLogFilesCount);
	return PsLogCount->ExecuteScalar().IntVal;
}

bool USplitDbBase::CreatePlayDbFromTemplate(FString Title, FString Additional,
                                       EPlayDbPurpose Purpose, bool SetAsWorkingCopy)
{
	int32 NewIndex = CreatePlayDbFromSource(PlayTemplateDbFilePath, Title, Additional, Purpose);

	/* If required, set the new playdb as the working copy. */
	if (NewIndex > 0 && SetAsWorkingCopy)
	{
		ConnectPlayDb(NewIndex);
	}

	return NewIndex != -1;
}

bool USplitDbBase::CreatePlayDbFromCurrent(FString Title, FString Additional,
                                      EPlayDbPurpose Purpose)
{
	/* Clean the current DB first, to reduce file size. */
	UDbStatement* qCleanPlay = QueryManager->FindStatementInGroup(SchemaPlay, PLAY_CleanPlay);
	qCleanPlay->ExecuteAction();

	int32 NewIndex = CreatePlayDbFromSource(WorkingCopyPlayDbPath, Title, Additional, Purpose);
	return NewIndex != -1;
}

bool USplitDbBase::CreateQuickSavePlayDb(FString Additional)
{
	int32 NewIndex = CreatePlayDbFromCurrent(QuickSaveTitle.ToString(), Additional, EPlayDbPurpose::QuickSave);

	/* If creating the quicksave was successful, then delete any previous quicksave files. */
	if (NewIndex > 0)
	{
		UDbStatement* qPurgeOldQuickSaves = QueryManager->FindStatementInGroup(SchemaLog, LOG_DeleteOldQuickSaves);
		qPurgeOldQuickSaves->SetBindingValue(P_LogID, NewIndex);
		qPurgeOldQuickSaves->ExecuteAction();
	}

	return NewIndex != -1;
}

bool USplitDbBase::ConnectResumePlayDb()
{
	UDbStatement* qGetLatestLog = QueryManager->FindStatementInGroup(SchemaLog, LOG_GetLatestLog);
	return ConnectPlayDb(qGetLatestLog->ExecuteScalar().IntVal);
}

TArray<FLogInfo> USplitDbBase::ListLogEntries(int32 MaxResults, TArray<EPlayDbPurpose> FilterBy)
{
	TArray<FLogInfo> Results;
	FString Purposes = TEXT(",");

	for (int i = 0; i < FilterBy.Num(); ++i)
	{
		EPlayDbPurpose Filter = FilterBy[i];
		Purposes = Purposes + FString::FromInt(static_cast<uint8>(Filter)) + TEXT(",");
	}

	UDbStatement* qGetLogEntries = QueryManager->FindStatementInGroup(SchemaLog, LOG_ListLogFiles);
	qGetLogEntries->SetBindingValue(P_MaxRecords, MaxResults);
	qGetLogEntries->SetBindingValue(P_Purpose, Purposes);

	FQueryResult RawResults = qGetLogEntries->ExecuteSelect();
	for (FQueryResultRow RawRow : RawResults.Rows)
	{
		FDateTime Created;
		FDateTime::Parse(RawRow.Fields[1].StrVal, Created);

		FLogInfo LogItem = FLogInfo(
			RawRow.Fields[0].IntVal,
			Created,
			RawRow.Fields[2].StrVal,
			RawRow.Fields[3].StrVal,
			static_cast<EPlayDbPurpose>(RawRow.Fields[4].IntVal)
		);
		Results.Add(LogItem);
	}

	return Results;
}

TArray<FLogInfo> USplitDbBase::ListLogEntries(int32 MaxResults, TArray<EPlayDbPurpose> FilterBy, bool Unused)
{
	return ListLogEntries(MaxResults, FilterBy);
}

TArray<FLogInfo> USplitDbBase::ListLogEntriesPaged(int32 RecordsPerPage, int32 PageNumber, TArray<EPlayDbPurpose> FilterBy)
{
	TArray<FLogInfo> Results;
	FString Purposes = TEXT(",");

	for (int i = 0; i < FilterBy.Num(); ++i)
	{
		EPlayDbPurpose Filter = FilterBy[i];
		Purposes = Purposes + FString::FromInt(static_cast<uint8>(Filter)) + TEXT(",");
	}

	UDbStatement* qGetLogEntries = QueryManager->FindStatementInGroup(SchemaLog, LOG_ListLogFilesPaged);
	qGetLogEntries->SetBindingValue(P_RecordsPerPage, RecordsPerPage);
	qGetLogEntries->SetBindingValue(P_PageNumber, PageNumber);
	qGetLogEntries->SetBindingValue(P_Purpose, Purposes);

	FQueryResult RawResults = qGetLogEntries->ExecuteSelect();
	for (FQueryResultRow RawRow : RawResults.Rows)
	{
		FDateTime Created;
		FDateTime::Parse(RawRow.Fields[1].StrVal, Created);

		FLogInfo LogItem = FLogInfo(
			RawRow.Fields[0].IntVal,
			Created,
			RawRow.Fields[2].StrVal,
			RawRow.Fields[3].StrVal,
			static_cast<EPlayDbPurpose>(RawRow.Fields[4].IntVal)
		);
		Results.Add(LogItem);
	}

	return Results;
}

TArray<FLogInfo> USplitDbBase::ListLogEntriesPaged(int32 RecordsPerPage, int32 PageNumber, TArray<EPlayDbPurpose> FilterBy,
	bool Unused)
{
	return ListLogEntriesPaged(RecordsPerPage, PageNumber, FilterBy);
}

FString USplitDbBase::GetCurrentLogDbPath() const
{
	return InstancedLogDbPath;
}

FString USplitDbBase::GetCurrentPlayDbPath() const
{
	return CurrentSourcePlayDbPath;
}

int32 USplitDbBase::CreatePlayDbFromSource(FString Source, FString Title, FString Additional, EPlayDbPurpose Purpose)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	/* Initialize to -1, the 'failure' value. */
	int32 NewIndex = -1;

	/* There should never be a staging file after this procedure finishes,
	 * But just in case, check for it and delete if found. */
	if (FileManager.FileExists(*PlayStagingDbFilePath))
	{
		FileManager.DeleteFile(*PlayStagingDbFilePath);
	}

	/* Copy the source file to 'staging.db'. */
	if (FileManager.CopyFile(*PlayStagingDbFilePath, *Source))
	{
		/* Create a new LOG record. */
		UDbStatement* qMakeNewLog = QueryManager->FindStatement(LOG_AddNewLog);
		qMakeNewLog->SetBindingValue(P_Title, Title);
		qMakeNewLog->SetBindingValue(P_Additional, Additional);
		qMakeNewLog->SetBindingValue(P_Purpose, static_cast<uint8>(Purpose));
		if (qMakeNewLog->ExecuteAction())
		{
			/* Retrieve the new LOG record. */
			UDbStatement* qGetNewLog = QueryManager->FindStatement(LOG_GetLatestLog);
			NewIndex = qGetNewLog->ExecuteScalar().IntVal;

			if (NewIndex > 0)
			{
				/* Rename the 'staging.db' file with the new LOG record ID. */
				FString NewPlayDbPath = FString::Format(*PlayInstancePath,
				                                        TMap<FString, FStringFormatArg>{
					                                        {TEXT("SaveDir"), FPaths::ProjectSavedDir()},
					                                        {TEXT("LogIndex"), FString::FromInt(NewIndex)}
				                                        });

				/* This should effectively 'remove' the staging file, by renaming it. */
				FileManager.MoveFile(*NewPlayDbPath, *PlayStagingDbFilePath);
			}
		}
	}
	return NewIndex;
}

bool USplitDbBase::FindAttachmentOfType(TArray<FGameDbAttachment>& Source, EDbFilePurpose Purpose,
                                   FGameDbAttachment& OutAttachment) const
{
	for (FGameDbAttachment Attachment : Source)
	{
		if (Attachment.Purpose == Purpose)
		{
			OutAttachment = Attachment;
			return true;
		}
	}
	return false;
}
