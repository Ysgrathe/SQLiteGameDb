/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/UnrealEditorSubsystem.h"
#include "UObject/Object.h"
#include "DbEditorSubsystem.generated.h"

/* */
UCLASS()
class SQLITEGAMEDBEDITOR_API UDbEditorSubsystem : public UUnrealEditorSubsystem
{
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
private:
	GENERATED_BODY()
};
