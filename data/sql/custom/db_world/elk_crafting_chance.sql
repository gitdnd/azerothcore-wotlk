DELIMITER ;;
DROP PROCEDURE IF EXISTS create_elk_crafting_chance;
CREATE PROCEDURE create_elk_crafting_chance ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;	
	CREATE TABLE elk_crafting_chance (
    id int NOT NULL AUTO_INCREMENT,
    chance_pool varchar(64) NOT NULL,
	item_id mediumint NOT NULL,
	chance float NOT NULL,
	UNIQUE (id)
   ); 
END;;
CALL create_elk_crafting_chance();;
DROP PROCEDURE  create_elk_crafting_chance;; 

TRUNCATE TABLE elk_crafting_chance;

REPLACE INTO `elk_crafting_chance` (`chance_pool`, `item_id`, `chance`) VALUES
	("COPPER_BASIC", 2851, 1),
	("COPPER_BASIC", 2852, 1),
	("COPPER_BASIC", 2853, 1),
	("COPPER_BASIC", 2869, 1),
	("COPPER_BASIC", 10421, 1);