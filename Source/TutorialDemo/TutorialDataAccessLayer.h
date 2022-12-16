/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#pragma once

#include "CoreMinimal.h"
#include "DbBase.h"
#include "TutorialDataAccessLayer.generated.h"

class UDbEquipmentComponent;
class UUserAccountDetails;

/* This is the Tutorial Data Access Layer class. */
UCLASS()
class TUTORIALDEMO_API UTutorialDataAccessLayer : public UDbBase
{
public:
	UFUNCTION(BlueprintCallable, Category = "DAL|Level",
		meta = (DisplayName="Is Level Managed By Database"))
	bool IsLevelManaged(FString LevelName);

	/* FUNCTIONS USED IN VIDEO - PART 3 */

	/* Returns the ID number for a level, given its name.
	 * or -1 if not found. */
	int32 GetLevelID(FString LevelName);

	/* Create a record, or update an existing record, for an equipment item. */
	void SaveEquipmentItem(FString InstanceID, int32 EquipmentID, int32 LevelID, int32 Quantity);

	/* Returns the quantity value for an equipment item, or -1 if it cant be found. */
	int32 LoadEquipmentItem(FString InstanceID, int32 LevelID);

	/* FUNCTIONS USED IN VIDEO - PART 5 */

	/* Validate a users login attempt, returns UserID if successful, else -1 */
	int32 TestUserLogin(FString UserName, FString Password);

	/* Create a new user account record.
	   Returns the UserId of the new user record, or -1 to indicate failure.
	   Success indicates that the user is now also logged in. */
	int32 CreateUserAccount(FString UserName, FString Password, FString TagLine);

	bool EditUserAccount(int32 UserID, FString UserName, FString Tagline, FString Password);

	bool ChangeUserPassword(int32 UserID, FString CurrentPassword, FString NewPassword);

	bool DeleteUserAccount(int32 UserID, FString Password);
	
	bool IsUserNameInUse(FString UserName, int32 UserID = 0);

	UUserAccountDetails* GetUserAccountDetails(int32 UserID);

protected:
	virtual void Build(FString DatabaseFilePath, FGameDbConfig Config) override;
	virtual void TearDown() override;

private:
	FString HashPassword(FString Password);

	/* Names of queries used, and common parameter names. */
	/* Queries used in part 3 */
	const FString Q_IsLevelManaged          = TEXT("IsLevelManaged");
	const FString Q_GetLevelID              = TEXT("GetLevelID");
	const FString Q_UpsertEquipmentInstance = TEXT("UpsertEquipmentInstance");
	const FString Q_GetEquipmentDetails     = TEXT("GetEquipmentDetails");

	/* Queries used in part 5 */
	const FString Q_CreateAccount   = TEXT("CreateAccount");
	const FString Q_EditAccount     = TEXT("EditAccount");
	const FString Q_ChangePassword  = TEXT("ChangePassword");
	const FString Q_ListAccounts    = TEXT("ListAccounts");
	const FString Q_DeleteAccount   = TEXT("DeleteAccount");
	const FString Q_TestLogin       = TEXT("TestLogin");
	const FString Q_GetAccount      = TEXT("GetAccount");
	const FString Q_IsUserNameInUse = TEXT("IsUserNameInUse");

	/* Query Parameters - Part 3 */
	const FString P_LevelName   = TEXT("@LevelName");
	const FString P_InstanceID  = TEXT("@InstanceID");
	const FString P_EquipmentID = TEXT("@EquipmentID");
	const FString P_LevelID     = TEXT("@LevelID");
	const FString P_Quantity    = TEXT("@Quantity");

	/* Query Parameters - Part 5 */
	const FString P_UserID              = TEXT("@UserID");
	const FString P_UserName            = TEXT("@UserName");
	const FString P_PasswordHash        = TEXT("@PasswordHash");
	const FString P_TagLine             = TEXT("@TagLine");
	const FString P_NewPasswordHash     = TEXT("@NewPasswordHash");
	const FString P_CurrentPasswordHash = TEXT("@CurrentPasswordHash");
	const FString P_UserNameFilter      = TEXT("@UserNameFilter");

	GENERATED_BODY()
};
