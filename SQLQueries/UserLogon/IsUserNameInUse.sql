SELECT (COUNT(*) != 0) AS NameInUse
FROM User
WHERE
    UserName = @UserName AND UserID != @UserID;