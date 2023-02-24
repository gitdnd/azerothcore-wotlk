drop procedure if exists doWhile;
DELIMITER ;; 
CREATE PROCEDURE doWhile()   
BEGIN
DECLARE i INT DEFAULT 1; 
WHILE (i <= 55) DO
    REPLACE INTO `player_classlevelstats` (class, level, basehp, basemana) values (6, i, 1, 1);
    SET i = i+1;
END WHILE;
END;
;;

CALL doWhile(); 

UPDATE `player_classlevelstats`
SET `basehp` = `level` * 10, `basemana` = `level` * 5; 

