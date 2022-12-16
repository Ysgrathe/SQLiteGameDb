CREATE TABLE [User]
(
    [UserID]       INTEGER NOT NULL,
    [UserName]     TEXT    NOT NULL UNIQUE,
    [PasswordHash] TEXT    NOT NULL,
    [TagLine]      TEXT,
    PRIMARY KEY ([UserID])
);
