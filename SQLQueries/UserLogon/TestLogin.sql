SELECT UserID
FROM User
WHERE UserName = @UserName
  AND PasswordHash = @PasswordHash
LIMIT 1;