/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "LogInfo.generated.h"

/* Defines a 'reason' why a copy of the playdb is made.
 * The entries 'AutoSave', and 'QuickSave' are used by the system
 * to perform those functions, and remove old save files.
 * For this to work with various copies of the database,
 * we need the numbering to remain consistent, even if a new entry is added -
 * for this reason the values have been manually set,
 * and new entries will only ever be added to the end of the list. */
UENUM(BlueprintType)
enum class EPlayDbPurpose : uint8
{
	None = 0,
	NewGame = 1,
	AutoSave = 2,
	QuickSave = 3,
	ManualSave = 4
};

// A single field of data returned from a query
USTRUCT(BlueprintType)
struct FLogInfo
{
	GENERATED_BODY()

	FLogInfo()
	{
	};

	FLogInfo(int32 InLogId, FDateTime InCreated, FString InTitle, FString InAdditional, EPlayDbPurpose InPurpose)
		: LogID(InLogId), Created(InCreated), Title(InTitle), Additional(InAdditional), Purpose(InPurpose)
	{
	}

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "SQLite Database|Log Info",
		meta = (DisplayName="Log ID"))
	int32 LogID = 0;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "SQLite Database|Log Info",
		meta = (DisplayName="Created DateTime"))
	FDateTime Created;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "SQLite Database|Log Info",
		meta = (DisplayName="Title"))
	FString Title;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "SQLite Database|Log Info",
		meta = (DisplayName="Additionl"))
	FString Additional;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "SQLite Database|Log Info",
		meta = (DisplayName="Purpose"))
	EPlayDbPurpose Purpose = EPlayDbPurpose::None;
};
