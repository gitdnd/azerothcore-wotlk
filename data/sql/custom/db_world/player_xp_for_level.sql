drop procedure if exists doWhile;
DELIMITER ;; 
CREATE PROCEDURE doWhile()   
BEGIN
DECLARE i INT DEFAULT 1; 
WHILE (i <= 200) DO
    REPLACE INTO `player_xp_for_level` (Level, Experience) values (1, 20000);
    SET i = i+1;
END WHILE;
END;
;;

CALL doWhile(); 