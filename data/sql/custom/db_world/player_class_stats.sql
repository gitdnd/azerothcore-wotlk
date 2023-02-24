
drop procedure if exists doWhile;
DELIMITER ;; 
CREATE PROCEDURE doWhile()   
BEGIN
DECLARE i INT DEFAULT 1; 
WHILE (i <= 55) DO
    REPLACE INTO `player_class_stats` (class, level, strength, agility, stamina, intellect, spirit) values (6, i, 1, 1, 1, 1, 1);
    SET i = i+1;
END WHILE;
END;
;;

CALL doWhile(); 

UPDATE `player_class_stats`
SET `Strength` = `level` * 1, `Agility` = `level` * 1, `Stamina` = `level` * 1, `Intellect` = `level` * 1, `Spirit` = `level` * 1; 