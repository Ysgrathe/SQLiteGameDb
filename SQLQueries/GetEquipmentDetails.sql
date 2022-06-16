select MAX(Qty) as Quantity
from (SELECT L.LevelID as LevelID, EI.Quantity as Qty
      FROM Level L
               INNER JOIN EquipmentInstance EI
                          ON L.LevelID = EI.LevelID

      WHERE EI.InstanceID = @InstanceID
        AND EI.LevelID LIKE @LevelID

      union all

      SELECT @LevelID as LevelID, -1 as Qty) X
group by LevelID
