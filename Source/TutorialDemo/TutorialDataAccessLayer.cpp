/* © Copyright 2022 Graham Chabas, All Rights Reserved. */

#include "TutorialDataAccessLayer.h"
#include "PreparedStatementManager.h"
#include "Hash/Blake3.h"
#include "Public/UserAccountDetails.h"
#include "SqliteGameDB/Public/CustomLogging.h"

#pragma region Lifetime Methods

void UTutorialDataAccessLayer::Build(FString DatabaseFilePath, FGameDbConfig Config)
{
	QueryManager->LoadStatements();
}

void UTutorialDataAccessLayer::TearDown()
{
}

#pragma endregion

#pragma region Unreal & SQL Databases - Part 3 - Framework Introduction

bool UTutorialDataAccessLayer::IsLevelManaged(FString LevelName)
{
	if (UDbStatement* Statement = QueryManager->FindStatement(Q_IsLevelManaged))
	{
		Statement->SetBindingValue(P_LevelName, LevelName);

		FQueryResultField Result = Statement->ExecuteScalar();
		return Result.BoolVal;
	}
	return false;
}

int32 UTutorialDataAccessLayer::GetLevelID(FString LevelName)
{
	if (UDbStatement* Statement = QueryManager->FindStatement(Q_GetLevelID))
	{
		Statement->SetBindingValue(P_LevelName, LevelName);

		FQueryResultField Result = Statement->ExecuteScalar();
		return Result.IntVal;
	}
	return -1;
}

void UTutorialDataAccessLayer::SaveEquipmentItem(FString InstanceID, int32 EquipmentID, int32 LevelID,
                                                 int32   Quantity)
{
	if (UDbStatement* PsSave = QueryManager->FindStatement(Q_UpsertEquipmentInstance))
	{
		PsSave->SetBindingValue(P_InstanceID, InstanceID);
		PsSave->SetBindingValue(P_EquipmentID, EquipmentID);
		PsSave->SetBindingValue(P_LevelID, LevelID);
		PsSave->SetBindingValue(P_Quantity, Quantity);
		PsSave->ExecuteAction();
	}
}

int32 UTutorialDataAccessLayer::LoadEquipmentItem(FString InstanceID, int32 LevelID)
{
	if (UDbStatement* PsLoad = QueryManager->FindStatement(Q_GetEquipmentDetails))
	{
		PsLoad->SetBindingValue(P_InstanceID, InstanceID);
		PsLoad->SetBindingValue(P_LevelID, LevelID);
		return PsLoad->ExecuteScalar().IntVal;
	}
	return -1;
}


#pragma endregion

#pragma region Unreal & SQL Databases - Part 5 - User Login

int32 UTutorialDataAccessLayer::TestUserLogin(FString UserName, FString Password)
{
	if (UDbStatement* Statement = QueryManager->FindStatement(Q_TestLogin))
	{
		/* Set the query parameters */
		Statement->SetBindingValue(P_UserName, UserName);
		Statement->SetBindingValue(P_PasswordHash, HashPassword(Password));

		/* Execute the query, and get the first column of the first row returned.
		   The query returns the count of users matching the username,
		   with password hashes matching the password.
		   This should only ever be either 0 or 1. */
		FQueryResultField Result = Statement->ExecuteScalar();

		/* Is there was a null value, it means the login fails*/
		return (Result.IsNull() || Result.IntVal == 0) ? -1 : Result.IntVal;
	}
	return -1;
}

int32 UTutorialDataAccessLayer::CreateUserAccount(FString UserName, FString Password, FString TagLine)
{
	/* Strip off any whitespace characters from the beginning and end of the username. */
	FString TrimmedUserName = UserName.TrimStartAndEnd();
	FString TrimmedTagLine  = TagLine.TrimStartAndEnd();

	/* Treat any attempt to create an account with an empty username as an automatic failure.
	   Because we trimmed the string, it also prevents names that are just whitespace. */
	if (TrimmedUserName.IsEmpty()) return -1;

	if (UDbStatement* Statement = QueryManager->FindStatement(Q_CreateAccount))
	{
		/* Set the query parameters */
		Statement->SetBindingValue(P_UserName, TrimmedUserName);
		Statement->SetBindingValue(P_PasswordHash, HashPassword(Password));
		Statement->SetBindingValue(P_TagLine, TrimmedTagLine);

		/* Execute the query, and get the first column of the first row returned.
		   The query returns the affected UserID.
		   This should either a valid UserID, or null if no record was inserted. */
		FQueryResultField Result = Statement->ExecuteScalar();

		return Result.IsNull() ? -1 : Result.IntVal;
	}
	return -1;
}

bool UTutorialDataAccessLayer::EditUserAccount(int32 UserID, FString UserName, FString Tagline, FString Password)
{
	/* We dont need to test if the provided [UserName] is unique before submitting it to the database.
	   If the name is NOT unique, the query will fail, and we will not get a valid UserID returned.

	   USUALLY in situations like this we want to know exactly why something failed, so we can tell the user.
	   With user account/login systems, this is generally NOT the case - we dont want to let someone who is 'probing'
	   the system have specific information about why something failed, as it reveals too much data.

	   We MIGHT however be interested in such specifics if we intended to log these details somewhere.
	   In such a case, we would run IsUserNameInUse() first to check for name uniqueness, and then
	   we would assign a failure reason appropriately, and log it.	*/
	if (UDbStatement* Statement = QueryManager->FindStatement(Q_EditAccount))
	{
		/* Set the query parameters */
		Statement->SetBindingValue(P_UserID, UserID);
		Statement->SetBindingValue(P_UserName, UserName.TrimStartAndEnd());
		Statement->SetBindingValue(P_TagLine, Tagline.TrimStartAndEnd());
		Statement->SetBindingValue(P_PasswordHash, HashPassword(Password));

		/* Execute the query, and get the first column of the first row returned.
		   The query returns the affected UserID.
		   This should either a valid UserID, or null if no record was inserted. */
		FQueryResultField Result = Statement->ExecuteScalar();

		return Result.IsNull() ? false : true;
	}
	return false;
}

bool UTutorialDataAccessLayer::ChangeUserPassword(int32 UserID, FString CurrentPassword, FString NewPassword)
{
	if (UDbStatement* Statement = QueryManager->FindStatement(Q_ChangePassword))
	{
		/* Set the query parameters */
		Statement->SetBindingValue(P_UserID, UserID);
		Statement->SetBindingValue(P_NewPasswordHash, HashPassword(NewPassword));
		Statement->SetBindingValue(P_CurrentPasswordHash, HashPassword(CurrentPassword));

		/* Execute the query, and get the first column of the first row returned.
		   The query returns the affected UserID.
		   This should either a valid UserID, or null if no record was inserted. */
		FQueryResultField Result = Statement->ExecuteScalar();

		return Result.IsNull() ? false : true;
	}
	return false;
}

bool UTutorialDataAccessLayer::DeleteUserAccount(int32 UserID, FString Password)
{
	if (UDbStatement* Statement = QueryManager->FindStatement(Q_DeleteAccount))
	{
		/* Set the query parameters */
		Statement->SetBindingValue(P_UserID, UserID);
		Statement->SetBindingValue(P_CurrentPasswordHash, HashPassword(Password));

		UE_LOG(LogSqliteGameDB, Warning, TEXT("DELETE - UserID:%d PWHash:%s"), UserID, *HashPassword(Password));

		/* Execute the query, and get the first column of the first row returned.
		   The query returns the affected UserID.
		   This should either a valid UserID, or null if no record was inserted. */
		FQueryResultField Result = Statement->ExecuteScalar();

		return Result.IsNull() ? false : true;
	}
	return false;
}

bool UTutorialDataAccessLayer::IsUserNameInUse(FString UserName, int32 UserID)
{
	if (UDbStatement* Statement = QueryManager->FindStatement(Q_IsUserNameInUse))
	{
		/* Set the query parameters */
		Statement->SetBindingValue(P_UserName, UserName.TrimStartAndEnd());
		Statement->SetBindingValue(P_UserID, UserID);

		/* Execute the query, and get the first column of the first row returned.
		   The query returns 1 if the UserNAme is in use (by any use OTHER than the passed in UserId).
		   Otherwise it returns 0. */
		FQueryResultField Result = Statement->ExecuteScalar();

		/* Is there was a null value, or an error, we return TRUE,
		   indicating the name *IS* in use (even if it isn't).
		   This is an unlikely occurrence, but means any method relying on the result
		   will assume the tame is taken and not try to use it. */
		return Result.IsNull() ? true : Result.BoolVal;
	}
	return true;
}

UUserAccountDetails* UTutorialDataAccessLayer::GetUserAccountDetails(int32 UserID)
{
	UUserAccountDetails* UserAccount = nullptr;

	if (UDbStatement* Statement = QueryManager->FindStatement(Q_GetAccount))
	{
		/* Set the query parameters */
		Statement->SetBindingValue(P_UserID, UserID);

		/* Execute the query and get the results.
		   The results should be a single row, which we can instantiate and populate an object from. */
		UserAccount = Statement->CreateObjectFromData<UUserAccountDetails>();
	}
	return UserAccount;
}

FString UTutorialDataAccessLayer::HashPassword(FString Password)
{
	/* PRIVATE helper function - used internally to this class to hash passwords.
	   Not really sure this is how this hash function is supposed to be used.
	   But it seems to work, and in the vacuum of any good examples,
	   this is what I'll use until I find out otherwise. */
	FBlake3 Hash;
	Hash.Update(*Password, Password.Len());
	TStringBuilder<70> HashHex;
	HashHex << Hash.Finalize();
	const FString HashedPw = FString(HashHex.ToString());

	return HashedPw;
}

#pragma endregion
