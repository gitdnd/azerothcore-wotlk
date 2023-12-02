
DELIMITER ;;
DROP PROCEDURE IF EXISTS create_elk_spell_info;
CREATE PROCEDURE create_elk_spell_info ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;
	DROP TABLE IF EXISTS elk_spell_info;
	CREATE TABLE elk_spell_info (
    id int NOT NULL,
    elk_category tinyint NOT NULL,
	release_type bool NOT NULL,
	UNIQUE (id)
   );
END;;
CALL create_elk_spell_info();;
DROP PROCEDURE  create_elk_spell_info;;

TRUNCATE TABLE elk_spell_info;


SET @category_attack = 1;

REPLACE INTO `elk_spell_info` (`id`, `elk_category`, `release_type`) VALUES
(100002, @category_attack, false),
(100009, @category_attack, false),
(100044, @category_attack, false),
(100046, 0, true),
(1100006, 0, true),
(1300003, 0, true);