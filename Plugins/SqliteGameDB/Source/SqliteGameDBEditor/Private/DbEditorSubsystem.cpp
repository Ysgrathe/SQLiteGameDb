/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#include "DbEditorSubsystem.h"
#include "CustomLogging.h"

void UDbEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LOG_GDBE(Warning, TEXT("DbEditorSubsystem Initialize"));
}

void UDbEditorSubsystem::Deinitialize()
{
	LOG_GDBE(Warning, TEXT("DbEditorSubsystem Deinitialize"));
	Super::Deinitialize();
	
}
