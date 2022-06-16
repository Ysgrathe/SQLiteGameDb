/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "DbBase.h"
#include "DBSupport.h"
#include "SQLiteDatabase.h"
#include "UObject/Object.h"
#include "DbStatement.generated.h"

class UGameDbBase;
class FSQLitePreparedStatement;

/* Further wraps FSQLitePreparedStatement, providing useful management and utility functions. */
UCLASS(BlueprintType)
class SQLITEGAMEDB_API UDbStatement : public UObject
{
public:
	void Initialize(FSQLiteDatabase* InDatabase, const FString SqlQueryText);

	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Prepared Statement",
		meta = (DisplayName="Initilize Statement"))
	void InitStatement(UDbBase* DbConnection, const FString SqlQueryText);

	void BeginDestroy() override final;

#pragma region Parameter Binding

	/* Set the given int binding from its name or index. */
	bool SetBindingValue(const FString InBindingName, const int8 InValue);
	bool SetBindingValue(const FString InBindingName, const uint8 InValue);
	bool SetBindingValue(const FString InBindingName, const int16 InValue);
	bool SetBindingValue(const FString InBindingName, const uint16 InValue);
	bool SetBindingValue(const FString InBindingName, const int32 InValue);
	bool SetBindingValue(const FString InBindingName, const uint32 InValue);
	bool SetBindingValue(const FString InBindingName, const int64 InValue);
	bool SetBindingValue(const FString InBindingName, const uint64 InValue);
	bool SetBindingValue(const FString InBindingName, const FDateTime InValue);

	/* Set the given float binding from its name or index. */
	bool SetBindingValue(const FString InBindingName, const float InValue);
	bool SetBindingValue(const FString InBindingName, const double InValue);

	/* Set the given string binding from its name or index. */
	bool SetBindingValue(const FString InBindingName, const TCHAR* InValue);
	bool SetBindingValue(const FString InBindingName, const FString& InValue);
	bool SetBindingValue(const FString InBindingName, const FName InValue);
	bool SetBindingValue(const FString InBindingName, const FText& InValue);

	/* Set the given blob binding from its name or index.
	 * @note If bCopy is set to false, then you must ensure the memory bound remains valid for the duration that the prepared statement is using it. */
	bool SetBindingValue(const FString InBindingName, TArrayView<const uint8> InBlobData, const bool bCopy = true);
	bool SetBindingValue(const FString InBindingName, const void* InBlobData, const int32 InBlobDataSizeBytes,
	                     const bool bCopy = true);
	bool SetBindingValue(const FString InBindingName, const FGuid& InValue);

	/* Set the given null binding from its name or index. */
	bool SetBindingValueToNull(const FString InBindingName);

#pragma endregion

#pragma region Blueprint Parameter Bindings

	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Prepared Statement",
		meta = (DisplayName="Bind Parameter (Boolean)"))
	void SetBoolParameterValue(const FString InBindingName, const bool InValue);

	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Prepared Statement",
		meta = (DisplayName="Bind Parameter (Byte)"))
	void SetByteParameterValue(const FString InBindingName, const uint8 InValue);

	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Prepared Statement",
		meta = (DisplayName="Bind Parameter (Integer)"))
	void SetIntParameterValue(const FString InBindingName, const int32 InValue);

	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Prepared Statement",
		meta = (DisplayName="Bind Parameter (Integer64)"))
	void SetLongParameterValue(const FString InBindingName, const int64 InValue);

	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Prepared Statement",
		meta = (DisplayName="Bind Parameter (Float)"))
	void SetFloatParameterValue(const FString InBindingName, const float InValue);

	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Prepared Statement",
		meta = (DisplayName="Bind Parameter (String)"))
	void SetStringParameterValue(const FString InBindingName, const FString InValue);

#pragma endregion

	/* Runs an 'action' query, returns true if ok, false if an error occurs.
	 * Takes a pointer to a PreparedStatement, so it supports value binding, etc.
	 * NOTE: This method will call 'reset' on the PreparedStatement,
	 * but it will not affect any binding values. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Prepared Statement",
		meta = (DisplayName="Execute Action Query"))
	bool ExecuteAction();

	/* Executes a prepared statement that retrieves data.
	 * Returns the value contained in the first field of the first row in the resultset. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Prepared Statement",
		meta = (DisplayName="Execute Scalar Query"))
	FQueryResultField ExecuteScalar();

	/* Executes a prepared statement and returns any resultant data.
	This overload takes a pointer to the prepared statement you wish to run.
	The statement is 'reset' AFTER executing it. Any bound parameters are left as they are. */
	UFUNCTION(BlueprintCallable, Category = "SQLite Database|Prepared Statement",
		meta = (DisplayName="Execute Resultset Query"))
	FQueryResult ExecuteSelect();

#pragma region Extended Functions

	/* Executes a resultset-returning prepared statement, creates a new instance of an
	 * object class (per template T), and proceeds to set any property in the class flagged
	 * with the SaveGame specifier, with the data from the similarly named field in the result-set.
	 * If no data is returned for the query, or an error occurs, a nullptr is returned. */
	template <class T>
	T* CreateObjectFromData()
	{
		T* ResultObject = NewObject<T>();
		ReadIntoObject(ResultObject);
		return ResultObject;
	}

	/* Executes a resultset-returning prepared statement, and fills a TArray<T>,
	 * with one new instance of T per result row.
	 * For each new T, any property in the class flagged with the SaveGame specifier,
	 * is set with the data from the similarly named field in the resultset.
	 * If no data is returned for the query, or an error occurs, an empty TArray<T> is returned. */
	template <class T>
	void CreateObjectArrayFromData(TArray<T*>* Array)
	{
		Array->Empty();

		// Weakly typed UObject array.
		TArray<UObject*> WeakArray;
		{
			T* TmpObject = NewObject<T>();
			ReadIntoObjectArray(&WeakArray, TmpObject->GetClass());
			for (UObject* WeakObject : WeakArray)
			{
				T* StrongObject = static_cast<T*>(WeakObject);
				Array->Add(StrongObject);
			}
		}
	}

	template <class T>
	T* SpawnActorFromData()
	{
		return nullptr;
	}

	template <class T>
	void FillArrayWithSpawnedActors(TArray<T*>* Array)
	{
		//return nullptr;
	}

#pragma endregion

protected:
	
private:
	/* Reference to the database owning this prepared statement. */
	FSQLiteDatabase* SqliteDb = nullptr;

	FSQLitePreparedStatement* PreparedStatement = nullptr;

	/* For a given UClass - the 'type object' of a class (often retrieved with ClassName::StaticClass())
	 * This method iterates through all the properties, and returns an array of pointers to those
	 * which have the SaveGame flag set on them. */
	static TArray<FProperty*> FindSaveProperties(UStruct* ThisClass);

	/* Executes the prepared statement and tries to fill the provided object
	 * with the returned data. */
	void ReadIntoObject(UObject* ObjectToFill);

	/*void ReadIntoStruct(UScriptStruct* StructType, void* Thing);*/

	/* Attempts to persist an object's data back to the database.
	 * Each object property with a SaveGame specifier is assumed to have a
	 * matching @parameter in the query.
	 * The object's property values are bound to the statement, which is then executed. */
	void WriteFromObject(UObject* ObjectToSave);

	/* Attempts to fill the provided array, with objects instantiated from data
	 * returned by the prepared statement.
	 * Each row of returned data will become an object. */
	void ReadIntoObjectArray(TArray<UObject*>* ArrayToFill, UClass* ObjectClass);

	/* Utility function to set a property value on a given object using reflection. */
	void SetPropertyValue(UObject* ObjectToFill, FProperty* PropertyToSet, FQueryResultField Value) const;

	GENERATED_BODY()
};
