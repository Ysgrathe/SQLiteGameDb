// Copyright Epic Games, Inc. All Rights Reserved.

#include "SqliteCoreX.h"
#include "IncludeSQLite.h"

IMPLEMENT_MODULE(FSqliteCoreX, SqliteCoreX)

void FSqliteCoreX::StartupModule()
{
	StaticInitializeSQLite();
}

void FSqliteCoreX::ShutdownModule()
{
	StaticShutdownSQLite();
}

void FSqliteCoreX::StaticInitializeSQLite()
{
	if (!bInitializedSQLite)
	{
		// SQLite using a custom platform needs to perform some additional config before calling init
#if SQLITE_OS_OTHER
		extern int sqlite3_ue_config();
		if (sqlite3_ue_config() != SQLITE_OK)
		{
			return;
		}
#endif

		bInitializedSQLite = sqlite3_initialize() == SQLITE_OK;
	}
}

void FSqliteCoreX::StaticShutdownSQLite()
{
	if (bInitializedSQLite)
	{
		sqlite3_shutdown();
	}
}

bool FSqliteCoreX::bInitializedSQLite = false;
