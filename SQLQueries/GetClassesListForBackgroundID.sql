SELECT C.CharacterClassID, C.Name, C.Description, C.ColorR, C.ColorG, C.ColorB, C.ColorA
FROM CharacterClass C
WHERE C.BackgroundID = @BackgroundID