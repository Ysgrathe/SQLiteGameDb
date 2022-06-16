/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DatabaseExample.generated.h"

/* Sample SQLite database access code. */
UCLASS()
class TUTORIALDEMO_API UDatabaseExample : public UObject
{
public:

	void SampleActionQuery();

	void SampleSelectQuery();

protected:


private:
	
	GENERATED_BODY()
};
