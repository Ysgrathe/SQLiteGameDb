UPDATE User
SET PasswordHash = @NewPasswordHash
WHERE UserID = @UserID
  AND PasswordHash = @CurrentPasswordHash
RETURNING UserID;