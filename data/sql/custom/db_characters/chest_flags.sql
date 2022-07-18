DELIMITER ;;
CREATE PROCEDURE chest_flag_elk ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;	
	CREATE TABLE chest_flag (
    guid INT UNSIGNED NOT NULL,
    chest_flag INT UNSIGNED NOT NULL
   ); 
END;;
CALL chest_flag_elk();;
DROP PROCEDURE  chest_flag_elk;;