SELECT L.Name,
       LE.InstanceID,
       LE.EquipmentID,
       LE.LocationX,
       LE.LocationY,
       LE.LocationZ,
       LE.RotationP,
       LE.RotationY,
       LE.RotationR,
       LE.Quantity
FROM Level L
         inner join LevelEquipment LE on L.LevelID = LE.LevelID
         inner join Equipment E on E.EquipmentID = LE.EquipmentID
WHERE L.LevelID = @LevelID