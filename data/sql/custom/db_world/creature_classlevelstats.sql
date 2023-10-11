UPDATE `creature_classlevelstats`
SET `basehp0` = `level` * `level` + 15, `basehp1`  = `level` * `level` + 15, `basehp2`  = `level` * `level` + 15, `basemana` = `level` * 10, `basearmor` = `level` * 3, `attackpower` = `level` * 5, `rangedattackpower` = `level` * 3, `damage_base` = `level` * 1, `damage_exp1` = `level` * 1, `damage_exp2` = `level` * 1; 
