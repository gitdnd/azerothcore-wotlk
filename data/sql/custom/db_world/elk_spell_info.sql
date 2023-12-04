
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
	rune_cost tinyint NOT NULL,
	rune_cd smallint NOT NULL,
	UNIQUE (id)
   );
END;;
CALL create_elk_spell_info();;
DROP PROCEDURE  create_elk_spell_info;;

TRUNCATE TABLE elk_spell_info;


SET @category_attack = 1;

REPLACE INTO `elk_spell_info` (`id`, `elk_category`, `release_type`, `rune_cost`, `rune_cd`) VALUES
(100003, @category_attack, false, 1, 0),
(100006, 0, false, 1, 0),
(100009, @category_attack, false, 0, 0),
(100010, @category_attack, false, 1, 0),
(100044, @category_attack, false, 0, 0),
(100045, @category_attack, false, 1, 0),
(100046, 0, true, 1, 0),
(1100006, 0, true, 0, 0),
(1300003, 0, true, 0, 0);