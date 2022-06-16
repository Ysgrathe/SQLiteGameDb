UPDATE PlayerCharacter
SET BackgroundID = @BackgroundID,
    CharacterClassID = @CharacterClassID, 
    Name = @Name
WHERE PlayerCharacterID = 1