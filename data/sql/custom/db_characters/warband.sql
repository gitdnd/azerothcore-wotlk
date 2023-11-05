DELIMITER ;;
CREATE PROCEDURE warband_create_table ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;	
	DROP TABLE IF EXISTS warband;
	CREATE TABLE warband (
	character_id int UNSIGNED NOT NULL,
	warband_1_id int UNSIGNED NOT NULL,
	warband_2_id int UNSIGNED NOT NULL,
	warband_3_id int UNSIGNED NOT NULL,
	warband_4_id int UNSIGNED NOT NULL
	

); 
END;;
CALL warband_create_table();;
DROP PROCEDURE  warband_create_table;;

