/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DBSupport.generated.h"

/* The kinds of data sqlite will return. */
UENUM(BlueprintType)
enum class EDbValueType : uint8
{
	Integer,
	Float,
	String,
	Blob,
	Null
};

/* A single field of data returned from a query. */
USTRUCT(BlueprintType)
struct FQueryResultField
{
	GENERATED_BODY()

	/* Overloaded constructors for each type of data we want to store. */
	FQueryResultField()	{}		/* DB NULL */
	FQueryResultField(int64 Value) { IntVal = Value; BoolVal = IntVal != 0; Type = EDbValueType::Integer;}
	FQueryResultField(double Value)	{ DblVal = Value; Type = EDbValueType::Float;}
	FQueryResultField(FString Value) { StrVal = Value; Type = EDbValueType::String;}
	FQueryResultField(TArray<uint8> Value) { BlobVal = Value; Type = EDbValueType::Blob;}

	UPROPERTY(BlueprintReadOnly, Category = "SQLite Database|Query Result",
		meta = (DisplayName="Bool Value"))
	bool BoolVal = false;		/* INT value */
	
	UPROPERTY(BlueprintReadOnly, Category = "SQLite Database|Query Result",
		meta = (DisplayName="Integer Value"))
	int64 IntVal = 0;			/* INT value */
	
	UPROPERTY(BlueprintReadOnly, Category = "SQLite Database|Query Result",
		meta = (DisplayName="Float Value"))
	double DblVal = 0.0f;		/* FLOAT value */
	
	UPROPERTY(BlueprintReadOnly, Category = "SQLite Database|Query Result",
		meta = (DisplayName="String Value"))
	FString StrVal = TEXT("");	/* STRING value */
	
	UPROPERTY(BlueprintReadOnly, Category = "SQLite Database|Query Result",
		meta = (DisplayName="Blob Value"))
	TArray<uint8> BlobVal;		/* BINARY ARRAY (BLOB) value */

	/* The name of the database field */
	UPROPERTY(BlueprintReadOnly, Category = "SQLite Database|Query Result",
		meta = (DisplayName="Column Name"))
	FString ColName;

	/* The actual type reported by the database */
	UPROPERTY(BlueprintReadOnly, Category = "SQLite Database|Query Result",
		meta = (DisplayName="SQLite Value Type"))
	EDbValueType Type = EDbValueType::Null; 

	/* Convenience function to test for a DB NULL value.
	 * Just makes the code look 'cleaner' */
	FORCEINLINE bool IsNull() { return Type == EDbValueType::Null; }
	
	/* Depending on the underlying DB Type, returns the value as an FString.
	 * Remember; it's only aware of the DATABASE type, not the range of c++ types
	 * that you might eventually use it as. So even though the struct supports 'bool',
	 * it isn't supported by this method.
	 * (Intended *mainly* as debug helper, for log messages.) */
	FString ToString()
	{
		switch (Type)
		{
		case EDbValueType::Integer:
			return FString::Printf(TEXT("%lld"), IntVal);
		case EDbValueType::Float:
			return FString::Printf(TEXT("%f"), DblVal);
		case EDbValueType::String:
			return StrVal;
		default:
			return TEXT("");
		}
	}
};

/* A row of data returned from a query. */
USTRUCT(BlueprintType)
struct FQueryResultRow
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FQueryResultField> Fields;
};

/* Resultset of a select query, basically a collection of rows. */
USTRUCT(BlueprintType)
struct FQueryResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FQueryResultRow> Rows;
};
