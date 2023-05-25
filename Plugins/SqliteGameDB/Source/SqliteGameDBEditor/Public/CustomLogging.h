/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"

// Custom log category so that DB related messages can be filtered
DECLARE_LOG_CATEGORY_CLASS(LogSqliteGameDBEditor, Log, All);
#define LOG_GDBE(verbosity, text) UE_LOG(LogSqliteGameDBEditor, verbosity, TEXT("%s"), text)
