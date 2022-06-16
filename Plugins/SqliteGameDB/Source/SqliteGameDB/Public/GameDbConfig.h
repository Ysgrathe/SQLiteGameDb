#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SubclassOf.h"
#include "GameDbConfig.generated.h"

struct FGameDbAttachment;
class UDbBase;

UENUM(BlueprintType)
enum class EDbFilePurpose : uint8
{
	None = 0,
	Custom,
	Main,
	LogTemplate,
	PlayTemplate
};

USTRUCT(BlueprintType, Category = "SqliteGameDB")
struct SQLITEGAMEDB_API FGameDbAttachment
{
	GENERATED_BODY()

	// Filename of the sqlite file to connect
	UPROPERTY(config, EditAnywhere, Category = "Sqlite Database File|Attachment")
	FString FileName;

	UPROPERTY(config, EditAnywhere, Category = "Sqlite Database File|Attachment")
	EDbFilePurpose Purpose = EDbFilePurpose::None;
};

USTRUCT(BlueprintType, Category = "SqliteGameDB")
struct SQLITEGAMEDB_API FGameDbConfig
{
	GENERATED_BODY()

	// The derived UGameDbBase class you want to use
	UPROPERTY(config, EditAnywhere, Category = "Sqlite Database File")
	TSubclassOf<UDbBase> DerivedClass;

	// Filename of the sqlite file to connect
	UPROPERTY(config, EditAnywhere, Category = "Sqlite Database File")
	FString FileName;

	UPROPERTY(config, EditAnywhere, Category = "Sqlite Database File")
	TArray<FGameDbAttachment> Attachments;
};


