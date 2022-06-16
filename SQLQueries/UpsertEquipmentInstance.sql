INSERT INTO EquipmentInstance
    (InstanceID, EquipmentID, LevelID, Quantity)
VALUES (@InstanceID, @EquipmentID, @LevelID, @Quantity)
ON CONFLICT(InstanceID, LevelID)
    DO UPDATE SET EquipmentID = excluded.EquipmentID,
                  LevelID     = excluded.LevelID,
                  Quantity    = excluded.Quantity