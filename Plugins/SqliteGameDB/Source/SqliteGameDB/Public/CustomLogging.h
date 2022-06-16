/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"

// Custom log category so that DB related messages can be filtered
DECLARE_LOG_CATEGORY_CLASS(LogSqliteGameDB, Log, All);
#define LOG_GDB(verbosity, text) UE_LOG(LogSqliteGameDB, verbosity, TEXT("%s"), text)
