// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UserAccountDetails.generated.h"

/* Represents a single user account. */
UCLASS(BlueprintType)
class TUTORIALDEMO_API UUserAccountDetails final : public UObject
{
public:
	UPROPERTY(BlueprintReadOnly, SaveGame)
	int32 UserID;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	FString UserName;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	FString TagLine;

protected:
	
private:
	GENERATED_BODY()
};
