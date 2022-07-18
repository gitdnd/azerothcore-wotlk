DELIMITER ;;
CREATE PROCEDURE creature_elk ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;	
	ALTER TABLE `creature`
	ADD COLUMN elk INT NOT NULL DEFAULT 0;
END;;
CALL creature_elk();;
DROP PROCEDURE  creature_elk;;

UPDATE creature
SET position_x = position_x - 2133.3, 
position_y = position_y + 2133.33, 
map = 0 
WHERE (map = 530) AND (position_x > 5000) AND (position_y < 5000) AND (elk < 1); 

UPDATE creature
SET id1 = "15644" 
WHERE (id1 = "15645" OR id1 = "16162" )AND elk < 1;

UPDATE creature
SET elk = 1
WHERE elk < 1;

UPDATE `creature` 
SET `id1` = "15635"
WHERE (`guid` = "55051");