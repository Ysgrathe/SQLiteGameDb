INSERT INTO User
(UserName, PasswordHash, TagLine)
VALUES
    (@UserName, @PasswordHash, @TagLine)
RETURNING UserID;