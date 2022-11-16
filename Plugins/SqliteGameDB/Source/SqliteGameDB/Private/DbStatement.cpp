/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#include "DbStatement.h"
#include "UObject/TextProperty.h"
#include "CustomLogging.h"

void UDbStatement::Initialize(FSQLiteDatabase* InDatabase, const FString SqlQueryText)
{
	check(InDatabase && InDatabase->IsValid());
	SqliteDb = InDatabase;
	PreparedStatement = new FSQLitePreparedStatement();
	bool CreateOk = PreparedStatement->Create(*SqliteDb, *SqlQueryText, ESQLitePreparedStatementFlags::Persistent);
	check(CreateOk);
}

void UDbStatement::InitStatement(UDbBase* DbConnection, const FString SqlQueryText)
{
	Initialize(DbConnection->SqliteDb, SqlQueryText);
}

void UDbStatement::BeginDestroy()
{
	if (PreparedStatement)
	{
		PreparedStatement->Destroy();
		delete PreparedStatement;
	}
	Super::BeginDestroy();
}

#pragma region Value Binding

bool UDbStatement::SetBindingValue(const FString InBindingName, const int8 InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const uint8 InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const int16 InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const uint16 InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const int32 InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const uint32 InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const int64 InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const uint64 InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const FDateTime InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const float InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const double InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const TCHAR* InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const FString& InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const FName InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const FText& InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, TArrayView<const uint8> InBlobData, const bool bCopy)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InBlobData, bCopy);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const void* InBlobData, const int32 InBlobDataSizeBytes,
                                   const bool bCopy)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InBlobData, InBlobDataSizeBytes, bCopy);
}

bool UDbStatement::SetBindingValue(const FString InBindingName, const FGuid& InValue)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName, InValue);
}

bool UDbStatement::SetBindingValueToNull(const FString InBindingName)
{
	return PreparedStatement->SetBindingValueByName(*InBindingName);
}

void UDbStatement::SetBoolParameterValue(const FString InBindingName, const bool InValue)
{
	SetBindingValue(InBindingName, (int32)InValue);
}

void UDbStatement::SetByteParameterValue(const FString InBindingName, const uint8 InValue)
{
	SetBindingValue(InBindingName, InValue);
}

void UDbStatement::SetIntParameterValue(const FString InBindingName, const int32 InValue)
{
	SetBindingValue(InBindingName, InValue);
}

void UDbStatement::SetLongParameterValue(const FString InBindingName, const int64 InValue)
{
	SetBindingValue(InBindingName, InValue);
}

void UDbStatement::SetFloatParameterValue(const FString InBindingName, const float InValue)
{
	SetBindingValue(InBindingName, InValue);
}

void UDbStatement::SetStringParameterValue(const FString InBindingName, const FString InValue)
{
	SetBindingValue(InBindingName, InValue);
}


#pragma endregion

bool UDbStatement::ExecuteAction()
{
	if (!PreparedStatement || !PreparedStatement->IsValid())
	{
		LOG_GDB(Error, TEXT("INVALID PREPARED STATEMENT"));
		return false;
	}

	const bool result = PreparedStatement->Execute();
	if (!result)
	{
		LOG_GDB(Error, *SqliteDb->GetLastError());
	}
	PreparedStatement->Reset();
	return result;
}

FQueryResultField UDbStatement::ExecuteScalar()
{
	FQueryResult Results = ExecuteSelect();
	if (Results.Rows.Num() > 0)
		return Results.Rows[0].Fields[0];

	FQueryResultField NoData;
	return NoData;
}

FQueryResult UDbStatement::ExecuteSelect()
{
	FQueryResult Results;

	check(PreparedStatement && PreparedStatement->IsValid());

	TArray<ESQLiteColumnType> ColumnTypes;
	TArray<FString> ColumnNames;
	int32 NumberOfColumns = 0;
	bool ColumnsParsed = false;

	while (PreparedStatement->Step() == ESQLitePreparedStatementStepResult::Row)
	{
		// create a new row
		FQueryResultRow newRow;

		// If we haven't already done this, count the number of columns and their types
		if (!ColumnsParsed)
		{
			ColumnNames = PreparedStatement->GetColumnNames();
			NumberOfColumns = ColumnNames.Num();
			for (int32 i = 0; i < NumberOfColumns; i++)
			{
				ESQLiteColumnType ThisColType;
				PreparedStatement->GetColumnTypeByIndex(i, ThisColType);
				ColumnTypes.Add(ThisColType);
			}
		}

		for (int columnIdx = 0; columnIdx < NumberOfColumns; columnIdx++)
		{
			/* create a new field, which is NULL by default. */
			FQueryResultField newField;

			/* get the sqlite datatype for this column. */
			ESQLiteColumnType ColType = ColumnTypes[columnIdx];

			switch (ColType)
			{
			case ESQLiteColumnType::Integer:
				{
					int64 IntValue;
					if (PreparedStatement->GetColumnValueByIndex(columnIdx, IntValue))
						newField = FQueryResultField(IntValue);
					break;
				}
			case ESQLiteColumnType::Float:
				{
					double FloatValue;
					if (PreparedStatement->GetColumnValueByIndex(columnIdx, FloatValue))
						newField = FQueryResultField(FloatValue);
					break;
				}
			case ESQLiteColumnType::String:
				{
					FString StringValue;
					if (PreparedStatement->GetColumnValueByIndex(columnIdx, StringValue))
						newField = FQueryResultField(StringValue);
					break;
				}
			case ESQLiteColumnType::Blob:
				{
					TArray<uint8> BlobValue;
					if (PreparedStatement->GetColumnValueByIndex(columnIdx, BlobValue))
						newField = FQueryResultField(BlobValue);
					break;
				}
			default:
				newField = FQueryResultField();
				break;
			}

			/* What is the name of this column? */
			newField.ColName = ColumnNames[columnIdx];

			/* Append the column to the current resultset row. */
			newRow.Fields.Add(newField);
		}

		/* add the new row to the resultset. */
		Results.Rows.Add(newRow);
	}

	FString DbErr = SqliteDb->GetLastError();
	PreparedStatement->Reset();

	return Results;
}

#pragma region Reflection Utilities

TArray<FProperty*> UDbStatement::FindSaveProperties(UStruct* ThisClass)
{
	TArray<FProperty*> SaveProperties;

	for (TFieldIterator<FProperty> prop(ThisClass); prop; ++prop)
	{
		FProperty* Property = *prop;

		if (Property->HasAnyPropertyFlags(CPF_SaveGame))
		{
			SaveProperties.Add(Property);
		}
	}

	return SaveProperties;
}

void UDbStatement::ReadIntoObject(UObject* ObjectToFill)
{
	/* NOTE: ObjectToFill->StaticClass() wont work here, as the pointer is UObject*,
	we would get the UClass* for UObject and not the underlying class.
	Instead we use GetClass() which returns the UClass for the 'actual' derived class */
	UClass* ObjectClass = ObjectToFill->GetClass();
	TArray<FProperty*> SaveGameProps = FindSaveProperties(ObjectClass);

	check(PreparedStatement && PreparedStatement->IsValid());

	/* We are only interested in the first row of data (if any). */
	if (PreparedStatement->Step() == ESQLitePreparedStatementStepResult::Row)
	{
		TArray<FString> ColumnNames = PreparedStatement->GetColumnNames();

		for (int32 PropIdx = 0; PropIdx < SaveGameProps.Num(); PropIdx++)
		{
			FProperty* Property = SaveGameProps[PropIdx];
			FString PropName = Property->GetAuthoredName();

			for (int32 ColNameIdx = 0; ColNameIdx < ColumnNames.Num(); ColNameIdx++)
			{
				FString ColName = ColumnNames[ColNameIdx]; 
				if (PropName.Compare(*ColName, ESearchCase::IgnoreCase) == 0)
				{
					/* We found a property flagged as SaveGame, with a matching column name
					 * in the resultset row. Set the property's value to the column data. */
					FQueryResultField Value;

					/* get the sqlite datatype for this column. */
					ESQLiteColumnType ColType;
					PreparedStatement->GetColumnTypeByName(*ColName, ColType);

					switch (ColType)
					{
					case ESQLiteColumnType::Integer:
						{
							Value.Type = EDbValueType::Integer;
							PreparedStatement->GetColumnValueByName(*ColName, Value.IntVal);
							break;
						}
					case ESQLiteColumnType::Float:
						{
							Value.Type = EDbValueType::Float;
							PreparedStatement->GetColumnValueByName(*ColName, Value.DblVal);
							break;
						}
					case ESQLiteColumnType::String:
						{
							Value.Type = EDbValueType::String;
							PreparedStatement->GetColumnValueByName(*ColName, Value.StrVal);
							break;
						}
					default:
						{
							Value.Type = EDbValueType::Null;
							break;
						}
					}

					// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
					SetPropertyValue(ObjectToFill, Property, Value);
					break;
				}
			}
		}
	}

	/* Reset the statement when we are done with it. */
	PreparedStatement->Reset();
}

void UDbStatement::WriteFromObject(UObject* ObjectToSave)
{
	UClass* ObjectClass = ObjectToSave->GetClass();
	TArray<FProperty*> SaveGameProps = FindSaveProperties(ObjectClass);

	check(PreparedStatement && PreparedStatement->IsValid());

	PreparedStatement->Reset();
	PreparedStatement->ClearBindings();

	/* For each property marked with the SaveGame specifier, we attempt to get an
	 * index of a parameter with the same name in the Prepared Statement */
	for (const FProperty* Prop : SaveGameProps)
	{
		FString PropName = TEXT("@");
		PropName.Append(*Prop->GetAuthoredName());

		const int32 Idx = PreparedStatement->GetBindingIndexByName(*PropName);
		if (Idx != 0)
		{
			const FFieldClass* ThisFieldClass = Prop->GetClass();
			const EClassCastFlags ThisFieldType = static_cast<EClassCastFlags>(ThisFieldClass->GetId());

			switch (ThisFieldType)
			{
			case CASTCLASS_FBoolProperty:
				{
					const FBoolProperty* PropBool = CastField<FBoolProperty>(Prop);
					bool* ValuePtr = PropBool->ContainerPtrToValuePtr<bool>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FByteProperty:
				{
					const FByteProperty* PropByte = CastField<FByteProperty>(Prop);
					int8* ValuePtr = PropByte->ContainerPtrToValuePtr<int8>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FInt8Property:
				{
					const FInt8Property* PropInt8 = CastField<FInt8Property>(Prop);
					int8* ValuePtr = PropInt8->ContainerPtrToValuePtr<int8>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FInt16Property:
				{
					const FInt16Property* PropInt16 = CastField<FInt16Property>(Prop);
					int16* ValuePtr = PropInt16->ContainerPtrToValuePtr<int16>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FUInt16Property:
				{
					const FUInt16Property* PropInt16_2 = CastField<FUInt16Property>(Prop);
					int16* ValuePtr = PropInt16_2->ContainerPtrToValuePtr<int16>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}
			case CASTCLASS_FIntProperty:
				{
					const FIntProperty* PropInt32 = CastField<FIntProperty>(Prop);
					int32* ValuePtr = PropInt32->ContainerPtrToValuePtr<int32>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FUInt32Property:
				{
					const FUInt32Property* PropInt32_2 = CastField<FUInt32Property>(Prop);
					int32* ValuePtr = PropInt32_2->ContainerPtrToValuePtr<int32>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FInt64Property:
				{
					const FInt64Property* PropInt64 = CastField<FInt64Property>(Prop);
					int64* ValuePtr = PropInt64->ContainerPtrToValuePtr<int64>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}
			case CASTCLASS_FUInt64Property:
				{
					const FUInt64Property* PropInt64_2 = CastField<FUInt64Property>(Prop);
					int64* ValuePtr = PropInt64_2->ContainerPtrToValuePtr<int64>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FFloatProperty:
				{
					const FFloatProperty* PropFloat = CastField<FFloatProperty>(Prop);
					float* ValuePtr = PropFloat->ContainerPtrToValuePtr<float>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FDoubleProperty:
				{
					const FDoubleProperty* PropDouble = CastField<FDoubleProperty>(Prop);
					double* ValuePtr = PropDouble->ContainerPtrToValuePtr<double>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FEnumProperty:
				{
					const FEnumProperty* PropEnum_2 = CastField<FEnumProperty>(Prop);
					uint8* ValuePtr = PropEnum_2->ContainerPtrToValuePtr<uint8>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FStrProperty:
				{
					const FStrProperty* PropStr = CastField<FStrProperty>(Prop);
					FString* ValuePtr = PropStr->ContainerPtrToValuePtr<FString>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FNameProperty:
				{
					const FNameProperty* Prop_Name = CastField<FNameProperty>(Prop);
					FString* ValuePtr = Prop_Name->ContainerPtrToValuePtr<FString>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			case CASTCLASS_FTextProperty:
				{
					const FTextProperty* PropText = CastField<FTextProperty>(Prop);
					FString* ValuePtr = PropText->ContainerPtrToValuePtr<FString>(ObjectToSave);
					PreparedStatement->SetBindingValueByIndex(Idx, *ValuePtr);
					break;
				}

			default: // Currently unsupported property types
				{
					LOG_GDB(Error,
					        TEXT("Attempt to call SetPropertyValue() with unsupported Property Type (CASTCLASS)"));
					checkNoEntry();
					break;
				}
			}
		}
	}

	if (!PreparedStatement->Execute())
	{
		UE_LOG(LogSqliteGameDB, Error, TEXT("Error executing SQL: %s"), *SqliteDb->GetLastError());
	}
}

void UDbStatement::ReadIntoObjectArray(TArray<UObject*>* ArrayToFill, UClass* ObjectClass)
{
	// Cache the properties of the object type we need to create, which have the 'SaveGame' specifier.
	TArray<FProperty*> SaveGameProps = FindSaveProperties(ObjectClass);

	check(PreparedStatement && PreparedStatement->IsValid());

	// We have no idea what the state of the PreparedStatement is, so reset it.
	PreparedStatement->Reset();

	// Keep asking for rows until none are returned...
	while (PreparedStatement->Step() == ESQLitePreparedStatementStepResult::Row)
	{
		UObject* NewItem = NewObject<UObject>(this, ObjectClass);

		// GetColumnNames() caches them on the first call, so no problem calling it repeatedly.
		TArray<FString> ColumnNames = PreparedStatement->GetColumnNames();

		// Fill the NewItem's properties with the database data.
		for (int32 PropIdx = 0; PropIdx < SaveGameProps.Num(); PropIdx++)
		{
			FProperty* Property = SaveGameProps[PropIdx];
			FString PropName = Property->GetAuthoredName();

			for (int32 ColNameIdx = 0; ColNameIdx < ColumnNames.Num(); ColNameIdx++)
			{
				FString ColName = ColumnNames[ColNameIdx];

				if (PropName.Compare(*ColName, ESearchCase::IgnoreCase) == 0)
				{
					/* We found a property flagged as SaveGame, with a matching column name
					in the result-set row. Set the property's value to the column data */
					FQueryResultField NewField;

					// get the sqlite datatype for this column
					ESQLiteColumnType ColType;
					PreparedStatement->GetColumnTypeByName(*ColName, ColType);

					switch (ColType)
					{
					case ESQLiteColumnType::Integer:
						{
							int64 IntValue;
							if (PreparedStatement->GetColumnValueByName(*ColName, IntValue))
								NewField = FQueryResultField(IntValue);
							break;
						}
					case ESQLiteColumnType::Float:
						{
							double FloatValue;
							if (PreparedStatement->GetColumnValueByName(*ColName, FloatValue))
								NewField = FQueryResultField(FloatValue);
							break;
						}
					case ESQLiteColumnType::String:
						{
							FString StringValue;
							if (PreparedStatement->GetColumnValueByName(*ColName, StringValue))
								NewField = FQueryResultField(StringValue);
							break;
						}
					case ESQLiteColumnType::Blob:
						{
							TArray<uint8> BlobValue;
							if (PreparedStatement->GetColumnValueByName(*ColName, BlobValue))
								NewField = FQueryResultField(BlobValue);
							break;
						}
					default:
						NewField = FQueryResultField();
						break;
					}

					// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
					SetPropertyValue(NewItem, Property, NewField);

					break;
				}
			}
		}

		// Add the NewItem to the array.
		ArrayToFill->Add(NewItem);
	}
}

void UDbStatement::SetPropertyValue(UObject* ObjectToFill, FProperty* PropertyToSet, FQueryResultField Value) const
{
	FString PropName = PropertyToSet->GetAuthoredName();

	FFieldClass* ThisFieldClass = PropertyToSet->GetClass();
	EClassCastFlags ThisFieldType = static_cast<EClassCastFlags>(ThisFieldClass->GetId());

	switch (ThisFieldType)
	{
	case CASTCLASS_FBoolProperty:
		{
			FBoolProperty* PropBool = CastField<FBoolProperty>(PropertyToSet);
			if (bool* ValuePtr = PropBool->ContainerPtrToValuePtr<bool>(ObjectToFill))
				*ValuePtr = (bool)Value.IntVal;
			break;
		}

	case CASTCLASS_FByteProperty:
		{
			FByteProperty* PropByte = CastField<FByteProperty>(PropertyToSet);
			if (int8* ValuePtr = PropByte->ContainerPtrToValuePtr<int8>(ObjectToFill))
				*ValuePtr = (int8)Value.IntVal;
			break;
		}

	case CASTCLASS_FInt8Property:
		{
			FInt8Property* PropInt8 = CastField<FInt8Property>(PropertyToSet);
			if (int8* ValuePtr = PropInt8->ContainerPtrToValuePtr<int8>(ObjectToFill))
				*ValuePtr = (int8)Value.IntVal;
			break;
		}

	case CASTCLASS_FInt16Property:
		{
			FInt16Property* PropInt16 = CastField<FInt16Property>(PropertyToSet);
			if (int16* ValuePtr = PropInt16->ContainerPtrToValuePtr<int16>(ObjectToFill))
				*ValuePtr = (int16)Value.IntVal;
			break;
		}

	case CASTCLASS_FUInt16Property:
		{
			FUInt16Property* PropInt16_2 = CastField<FUInt16Property>(PropertyToSet);
			if (int16* ValuePtr = PropInt16_2->ContainerPtrToValuePtr<int16>(ObjectToFill))
				*ValuePtr = (int16)Value.IntVal;
			break;
		}
	case CASTCLASS_FIntProperty:
		{
			FIntProperty* PropInt32 = CastField<FIntProperty>(PropertyToSet);
			if (int32* ValuePtr = PropInt32->ContainerPtrToValuePtr<int32>(ObjectToFill))
				*ValuePtr = (int32)Value.IntVal;
			break;
		}

	case CASTCLASS_FUInt32Property:
		{
			FUInt32Property* PropInt32_2 = CastField<FUInt32Property>(PropertyToSet);
			if (int32* ValuePtr = PropInt32_2->ContainerPtrToValuePtr<int32>(ObjectToFill))
				*ValuePtr = (int32)Value.IntVal;
			break;
		}

	case CASTCLASS_FInt64Property:
		{
			FInt64Property* PropInt64 = CastField<FInt64Property>(PropertyToSet);
			if (int64* ValuePtr = PropInt64->ContainerPtrToValuePtr<int64>(ObjectToFill))
				*ValuePtr = Value.IntVal;
			break;
		}
	case CASTCLASS_FUInt64Property:
		{
			FUInt64Property* PropInt64_2 = CastField<FUInt64Property>(PropertyToSet);
			if (int64* ValuePtr = PropInt64_2->ContainerPtrToValuePtr<int64>(ObjectToFill))
				*ValuePtr = Value.IntVal;
			break;
		}

	case CASTCLASS_FFloatProperty:
		{
			FFloatProperty* PropFloat = CastField<FFloatProperty>(PropertyToSet);
			if (float* ValuePtr = PropFloat->ContainerPtrToValuePtr<float>(ObjectToFill))
				*ValuePtr = (float)Value.DblVal;
			break;
		}

	case CASTCLASS_FDoubleProperty:
		{
			FDoubleProperty* PropDouble = CastField<FDoubleProperty>(PropertyToSet);
			if (double* ValuePtr = PropDouble->ContainerPtrToValuePtr<double>(ObjectToFill))
				*ValuePtr = Value.DblVal;
			break;
		}

	case CASTCLASS_FEnumProperty:
		{
			FEnumProperty* PropEnum_2 = CastField<FEnumProperty>(PropertyToSet);
			if (uint8* ValuePtr = PropEnum_2->ContainerPtrToValuePtr<uint8>(ObjectToFill))
				*ValuePtr = (uint8)Value.IntVal;
			break;
		}

	case CASTCLASS_FStrProperty:
		{
			FStrProperty* PropStr = CastField<FStrProperty>(PropertyToSet);
			if (FString* ValuePtr = PropStr->ContainerPtrToValuePtr<FString>(ObjectToFill))
				*ValuePtr = Value.StrVal;
			break;
		}

	case CASTCLASS_FNameProperty:
		{
			FNameProperty* Prop_Name = CastField<FNameProperty>(PropertyToSet);
			if (FString* ValuePtr = Prop_Name->ContainerPtrToValuePtr<FString>(ObjectToFill))
				*ValuePtr = Value.StrVal;
			break;
		}

	case CASTCLASS_FTextProperty:
		{
			FTextProperty* PropText = CastField<FTextProperty>(PropertyToSet);
			if (FString* ValuePtr = PropText->ContainerPtrToValuePtr<FString>(ObjectToFill))
				*ValuePtr = Value.StrVal;
			break;
		}


	default: // Currently unsupported property types
		{
			LOG_GDB(Error, TEXT("Attempt to call SetPropertyValue() with unsupported Property Type (CASTCLASS)"));
			checkNoEntry();
			break;
		}
	}
}

#pragma endregion
