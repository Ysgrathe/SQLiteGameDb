// Copyright (C) 2022 Graham Chabas. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DbStringSerializer.generated.h"

/* This is a base for all structs which need automatic DB serialization.
   Any USTRUCT which derives from this can be used as a UPROPERTY in a class,
   and have its field's values automatically set from a single database field
   having the same name as the property. */
USTRUCT(BlueprintType)
struct FDbStringSerializer
{
	GENERATED_BODY()
	
	virtual         ~FDbStringSerializer() = default;
	virtual void    FromDbString(FString DbString)	{};
	virtual FString ToDbString() { return TEXT(""); };

	
};
