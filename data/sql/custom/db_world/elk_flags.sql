
DELIMITER ;;
DROP PROCEDURE IF EXISTS create_elk_flag;
CREATE PROCEDURE create_elk_flag ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;
	CREATE TABLE elk_flag (
    id int NOT NULL AUTO_INCREMENT,
    flag_name varchar(64) NOT NULL,
	UNIQUE (id)
   );
END;;
CALL create_elk_flag();;
DROP PROCEDURE  create_elk_flag;;


TRUNCATE TABLE elk_flag;
DROP PROCEDURE IF EXISTS add_elk_flag;
SET @flagAdded = 1;


CREATE PROCEDURE add_elk_flag (IN flag_nameP VARCHAR(64))
BEGIN
   DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;
	REPLACE INTO `elk_flag` (`id`, `flag_name`) VALUES
	(@flagAdded, flag_nameP);
	SET @flagAdded = @flagAdded + 1;
END;;

CALL add_elk_flag ("TEST_FLAG");

CALL add_elk_flag ("ERONA_INITIAL_INTRO");
