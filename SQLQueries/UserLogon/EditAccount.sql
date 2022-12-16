UPDATE User
SET TagLine  = @TagLine,
    UserName = @UserName
WHERE UserID = @UserID
  AND PasswordHash = @PasswordHash
RETURNING UserID;