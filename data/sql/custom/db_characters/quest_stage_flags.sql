DELIMITER ;;
CREATE PROCEDURE quest_stage_flag_elk ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;	
	CREATE TABLE quest_stage_flag (
    guid INT UNSIGNED NOT NULL,
    quest_stage_flag INT UNSIGNED NOT NULL,
    stage_flag BOOLEAN NOT NULL
   ); 
END;;
CALL quest_stage_flag_elk();;
DROP PROCEDURE  quest_stage_flag_elk;;