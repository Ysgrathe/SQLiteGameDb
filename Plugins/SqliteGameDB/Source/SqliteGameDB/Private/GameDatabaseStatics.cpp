/* © Copyright 2022 Graham Chabas, All Rights Reserved */

#include "GameDatabaseStatics.h"
#include "DbStatement.h"
#include "GameFramework/Actor.h"
#include "CustomLogging.h"

bool UGameDatabaseStatics::TryFillVector(FVector& Vector,
                                         const FQueryResultRow& QueryRow,
                                         const FString& FieldNameBase)
{
	int8 FilledCount = 0;

	for (FQueryResultField Field : QueryRow.Fields)
	{
		if (Field.ColName.StartsWith(FieldNameBase))
		{
			const FString EndsWith = Field.ColName.Right(1);

			if (EndsWith == "X" || EndsWith == "Y" || EndsWith == "Z")
			{
				if (EndsWith == "X")
				{
					Vector.X = Field.DblVal;
					FilledCount++;
				}
				else if (EndsWith == "Y")
				{
					Vector.Y = Field.DblVal;
					FilledCount++;
				}
				else if (EndsWith == "Z")
				{
					Vector.Y = Field.DblVal;
					FilledCount++;
				}

				if (FilledCount == 3) return true;
			}
		}
	}

	return false;
}

bool UGameDatabaseStatics::TryFillColor(FLinearColor& Color,
                                        const FQueryResultRow& QueryRow,
                                        const FString& FieldNameBase)
{
	int8 ColorCount = 0;
	int8 AlphaCount = 0;

	for (FQueryResultField Field : QueryRow.Fields)
	{
		if (Field.ColName.StartsWith(FieldNameBase))
		{
			const FString EndsWith = Field.ColName.Right(1);

			if (EndsWith == "R" || EndsWith == "G" || EndsWith == "B" || EndsWith == "A")
			{
				if (EndsWith == "R")
				{
					Color.R = Field.DblVal;
					ColorCount++;
				}
				else if (EndsWith == "G")
				{
					Color.G = Field.DblVal;
					ColorCount++;
				}
				else if (EndsWith == "B")
				{
					Color.B = Field.DblVal;
					ColorCount++;
				}
				else if (EndsWith == "A")
				{
					Color.A = Field.DblVal;
					AlphaCount++;
				}

				/* We only short-circuit if all RGBA values have been set */
				if (ColorCount + AlphaCount == 4)
					return true;
			}
		}
	}

	/* If we set all RGB values, but not the alpha, we still consider this a success
	 * as we might choose to not store alpha in the database. */
	if (ColorCount == 3)
		return true;

	return false;
}

bool UGameDatabaseStatics::TryFillRotation(FRotator& Rotation,
                                           const FQueryResultRow& QueryRow,
                                           const FString& FieldNameBase)
{
	int8 FilledCount = 0;

	for (FQueryResultField Field : QueryRow.Fields)
	{
		if (Field.ColName.StartsWith(FieldNameBase))
		{
			const FString EndsWith = Field.ColName.Right(1);

			if (EndsWith == "P" || EndsWith == "R" || EndsWith == "Y")
			{
				if (EndsWith == "P")
				{
					Rotation.Pitch = Field.DblVal;
					FilledCount++;
				}
				else if (EndsWith == "R")
				{
					Rotation.Roll = Field.DblVal;
					FilledCount++;
				}
				else if (EndsWith == "Y")
				{
					Rotation.Yaw = Field.DblVal;
					FilledCount++;
				}

				if (FilledCount == 3) return true;
			}
		}
	}

	return false;
}

bool UGameDatabaseStatics::TrySetActorLocationRotation(AActor* Actor,
                                                       const FQueryResultRow& QueryRow,
                                                       const FString& LocationFieldNameBase,
                                                       const FString& RotationFieldNameBase)
{
	FVector NewLocation;
	FRotator NewRotation;

	if (TryFillVector(NewLocation, QueryRow, LocationFieldNameBase))
	{
		if (TryFillRotation(NewRotation, QueryRow, RotationFieldNameBase))
		{
			Actor->SetActorLocationAndRotation(NewLocation, NewRotation);
			return true;
		}
	}

	return false;
}

bool UGameDatabaseStatics::TryBindActorLocationRotation(UDbStatement* PreparedStatement,
                                                        const AActor* Actor,
                                                        const FString& LocationFieldNameBase,
                                                        const FString& RotationFieldNameBase)
{
	const FVector Location = Actor->GetActorLocation();
	const FRotator Rotation = Actor->GetActorRotation();

	const FString LocationX = LocationFieldNameBase + "X";
	const FString LocationY = LocationFieldNameBase + "Y";
	const FString LocationZ = LocationFieldNameBase + "Z";

	const FString RotationP = RotationFieldNameBase + "P";
	const FString RotationR = RotationFieldNameBase + "R";
	const FString RotationY = RotationFieldNameBase + "Y";

	const bool SetLocationRotation =
		PreparedStatement->SetBindingValue(LocationX, Location.X) &&
		PreparedStatement->SetBindingValue(LocationY, Location.Y) &&
		PreparedStatement->SetBindingValue(LocationZ, Location.Z) &&
		PreparedStatement->SetBindingValue(RotationP, Rotation.Pitch) &&
		PreparedStatement->SetBindingValue(RotationR, Rotation.Roll) &&
		PreparedStatement->SetBindingValue(RotationY, Rotation.Yaw);

#if !UE_BUILD_SHIPPING
	if (!SetLocationRotation)
	{
		LOG_GDB(Error, TEXT("Cannot set location/rotation query parameter: Invalid parameter name?"));
	}
#endif

	return SetLocationRotation;
}

bool UGameDatabaseStatics::TryBindActorLocation(UDbStatement* PreparedStatement, const AActor* Actor,
                                                const FString& LocationFieldNameBase)
{
	const FVector Location = Actor->GetActorLocation();

	const FString LocationX = LocationFieldNameBase + "X";
	const FString LocationY = LocationFieldNameBase + "Y";
	const FString LocationZ = LocationFieldNameBase + "Z";

	const bool SetLocation =
		PreparedStatement->SetBindingValue(LocationX, Location.X) &&
		PreparedStatement->SetBindingValue(LocationY, Location.Y) &&
		PreparedStatement->SetBindingValue(LocationZ, Location.Z);

#if !UE_BUILD_SHIPPING
	if (!SetLocation)
	{
		LOG_GDB(Error, TEXT("Cannot set location query parameter: Invalid parameter name?"));
	}
#endif

	return SetLocation;
}

bool UGameDatabaseStatics::TryBindActorRotation(UDbStatement* PreparedStatement, const AActor* Actor,
                                                const FString& RotationFieldNameBase)
{
	const FRotator Rotation = Actor->GetActorRotation();

	const FString RotationP = RotationFieldNameBase + "P";
	const FString RotationR = RotationFieldNameBase + "R";
	const FString RotationY = RotationFieldNameBase + "Y";

	const bool SetRotation =
		PreparedStatement->SetBindingValue(RotationP, Rotation.Pitch) &&
		PreparedStatement->SetBindingValue(RotationR, Rotation.Roll) &&
		PreparedStatement->SetBindingValue(RotationY, Rotation.Yaw);

#if !UE_BUILD_SHIPPING
	if (!SetRotation)
	{
		LOG_GDB(Error, TEXT("Cannot set location/rotation query parameter: Invalid parameter name?"));
	}
#endif

	return SetRotation;
}
