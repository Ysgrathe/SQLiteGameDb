DELETE FROM User
WHERE UserID = @UserID
  AND PasswordHash = @CurrentPasswordHash
RETURNING UserID;