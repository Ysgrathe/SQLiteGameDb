SELECT L.Name,
       LN.NPCID,
       LN.LocationX,
       LN.LocationY,
       LN.LocationZ,
       LN.RotationP,
       LN.RotationY,
       LN.RotationR,
       LN.NPCStateID,
       LN.NPCTypeID
FROM Level L
         inner join LevelNPC LN on l.LevelID = LN.LevelID
         inner join NPCType NT on nt.NPCTypeID = LN.NPCTypeID
         inner join NPCState NS on LN.NPCStateID = NS.NPCStateID
WHERE L.LevelID = @LevelID