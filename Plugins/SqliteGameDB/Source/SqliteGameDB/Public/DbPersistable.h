/* © Copyright 2022 Graham Chabas, All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DbPersistable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDbPersistable : public UInterface
{
	GENERATED_BODY()
};

/* Interface for classes that can be persisted to the database. */
class SQLITEGAMEDB_API IDbPersistable
{
public:
	/* Persist anything that needs saving to the database. */
	virtual void SaveState(FString LevelName) = 0;

	/* Load state from the database and update properties. */
	virtual void LoadState() = 0;

protected:
	/* Can be used to track when persistable properties have been changed,
	 * and only perform saves in those cases. */
	virtual bool IsDirty() = 0;

private:


	GENERATED_BODY()
};
