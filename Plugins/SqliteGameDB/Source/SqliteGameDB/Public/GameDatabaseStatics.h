/* © Copyright 2022 Graham Chabas, All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/Object.h"
#include "DBSupport.h"
#include "GameDatabaseStatics.generated.h"

class UDbStatement;
/* */
UCLASS()
class SQLITEGAMEDB_API UGameDatabaseStatics : public UBlueprintFunctionLibrary
{
public:
	/* Populate an FVector from individual query result field values using a base field name.
	 * The function looks for fields in the result row that start with that name,
	 * and end in 'X', 'Y', 'Z', and sets the relevant properties in the FVector.
	 * Returns true/false to indicate if the function was able to do so. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Utility", meta = (DisplayName="Vector From Database"))
	static bool TryFillVector(FVector& Vector,
	                          const FQueryResultRow& QueryRow,
	                          const FString& FieldNameBase = TEXT("Location"));

	/* Populate an FColor from individual query result field values using a base field name.
	 * The function looks for fields in the result row that start with that name,
	 * and end in 'R', 'G', 'B', or 'A', and sets the relevant properties in the FColor.
	 * Returns true/false to indicate if the function was able to do so. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Utility", meta = (DisplayName="Color From Database"))
	static bool TryFillColor(FLinearColor& Color,
	                         const FQueryResultRow& QueryRow,
	                         const FString& FieldNameBase = TEXT("Color"));

	/* Populate an FRotator from individual query result field values using a base field name.
	 * The function looks for fields in the result row that start with that name,
	 * and end in 'P', 'R', 'Y', and sets the relevant properties in the FRotator.
	 * Returns true/false to indicate if the function was able to do so. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Utility", meta = (DisplayName="Rotation From Database"))
	static bool TryFillRotation(FRotator& Rotation,
	                            const FQueryResultRow& QueryRow,
	                            const FString& FieldNameBase = TEXT("Rotation"));

	/* Attempts to find the 6 fields required for both location and rotation in the query result row,
	 * and set the actor's location and rotation using them.
	 * Returns true/false to indicate if the function was able to do so. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Actors", meta = (DisplayName="Set Actor's LocationRotation"))
	static bool TrySetActorLocationRotation(AActor* Actor,
	                                        const FQueryResultRow& QueryRow,
	                                        const FString& LocationFieldNameBase = TEXT("Location"),
	                                        const FString& RotationFieldNameBase = TEXT("Rotation"));

	/* Given a reference to an AActor, and a PreparedStatement,
	 * Attempts to set the prepared statement bindings to the actor's
	 * current location/rotation values. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Actors", meta = (DisplayName="Bind From Actor's LocationRotation"))
	static bool TryBindActorLocationRotation(UDbStatement* PreparedStatement,
	                                         const AActor* Actor,
	                                         const FString& LocationFieldNameBase = TEXT("Location"),
	                                         const FString& RotationFieldNameBase = TEXT("Rotation"));

	/* Given a reference to an AActor, and a PreparedStatement,
	 * Attempts to set the prepared statement bindings to the actor's
	 * current location/rotation values. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Actors", meta = (DisplayName="Bind From Actor's Location"))
	static bool TryBindActorLocation(UDbStatement* PreparedStatement,
	                                 const AActor* Actor,
	                                 const FString& LocationFieldNameBase = TEXT("Location"));

	/* Given a reference to an AActor, and a PreparedStatement,
	 * Attempts to set the prepared statement bindings to the actor's
	 * current location/rotation values. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Actors", meta = (DisplayName="Bind From Actor's Rotation"))
	static bool TryBindActorRotation(UDbStatement* PreparedStatement,
	                                 const AActor* Actor,
	                                 const FString& RotationFieldNameBase = TEXT("Rotation"));

private:
	GENERATED_BODY()
};
