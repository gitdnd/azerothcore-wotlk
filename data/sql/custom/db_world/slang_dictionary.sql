DELIMITER ;;
CREATE PROCEDURE slang_dictionary_elk ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;	
	CREATE TABLE slang_dictionary (
    word varchar(15) NOT NULL,
    orc varchar(15) NOT NULL,
    troll varchar(15) NOT NULL
   ); 
END;;
CALL slang_dictionary_elk();;
DROP PROCEDURE  slang_dictionary_elk;; 