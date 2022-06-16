/* This class is used to store any project specific settings that need to be used by the Database code */
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DeveloperSettings.h"
#include "SqliteGameDBSettings.generated.h"

struct FGameDbConfig;

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Sqlite Game Database"))
class SQLITEGAMEDB_API USqliteGameDBSettings : public UDeveloperSettings
{

public:
	/* The Content-Folder path containing the database files we will access */
	UPROPERTY(config, EditAnywhere, Category = "Configuration")
	FString DatabaseContentFolder;

	UPROPERTY(config, EditAnywhere, Category = "Configuration")
	TArray<FGameDbConfig> DatabaseConnectionClasses;

private:
		GENERATED_BODY()

};