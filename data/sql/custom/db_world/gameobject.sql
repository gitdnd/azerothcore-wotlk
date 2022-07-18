
DELIMITER ;;
CREATE PROCEDURE gameobject_elk ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;	
	ALTER TABLE `gameobject`
	ADD COLUMN elk INT NOT NULL DEFAULT 0;
END;;
CALL gameobject_elk();;
DROP PROCEDURE  gameobject_elk;;
 
UPDATE gameobject
SET id = "500001", spawntimesecs = 1
WHERE (id = "175784"); 