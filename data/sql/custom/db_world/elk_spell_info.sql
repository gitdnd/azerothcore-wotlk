
DELIMITER ;;
DROP PROCEDURE IF EXISTS create_elk_spell_info;
CREATE PROCEDURE create_elk_spell_info ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;
	CREATE TABLE elk_spell_info (
    id int NOT NULL AUTO_INCREMENT,
    elk_category tinyint NOT NULL,
	UNIQUE (id)
   );
END;;
CALL create_elk_spell_info();;
DROP PROCEDURE  create_elk_spell_info;;


TRUNCATE TABLE elk_spell_info;

SET @category_attack = 1;

REPLACE INTO `elk_spell_info` (`id`, `elk_category`) VALUES
(100002, @category_attack),
(100009, @category_attack),
(100044, @category_attack);