SELECT DISTINCT WT.WeaponTypeID,
                WT.Name,
                '' as Description
FROM  CharacterClassWeaponTypes CW
         LEFT OUTER JOIN  WeaponType WT
                    on CW.WeaponTypeID = WT.WeaponTypeID

WHERE CW.CharacterClassID = @CharacterClassID
ORDER BY WT.WeaponTypeID



