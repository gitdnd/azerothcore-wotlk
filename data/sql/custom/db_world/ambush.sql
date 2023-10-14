DELIMITER ;;
CREATE PROCEDURE rev_ambush_create_table ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;	
	DROP TABLE IF EXISTS rev_ambush;
	CREATE TABLE rev_ambush (
	rev_ambush_id int UNSIGNED NOT NULL AUTO_INCREMENT,
	spawnArea TINYINT UNSIGNED NOT NULL,
	spawnKill TINYINT UNSIGNED NOT NULL,
	creature_guid INT UNSIGNED NOT NULL,
	hostile BOOLEAN NOT NULL,
	repId MEDIUMINT UNSIGNED NOT NULL,
	repCutoff TINYINT UNSIGNED NOT NULL,
	delay MEDIUMINT UNSIGNED NOT NULL,
	hpPct TINYINT UNSIGNED NOT NULL,
	lvlMin TINYINT UNSIGNED NOT NULL,
	lvlMax TINYINT UNSIGNED NOT NULL,
	cooldown SMALLINT UNSIGNED NOT NULL,
	area SMALLINT UNSIGNED NOT NULL,
	PRIMARY KEY (rev_ambush_id)
	
); 
END;;
CALL rev_ambush_create_table();;
DROP PROCEDURE  rev_ambush_create_table;;

INSERT INTO `rev_ambush` (`rev_ambush_id`, `spawnArea`, `spawnKill`, `creature_guid`, `hostile`, `repId`, `repCutoff`, `delay`, `hpPct`, `lvlMin`, `lvlMax`, `cooldown`, `area`) VALUES
	(1, 20, 35, 15643, false, 69, 0, 20, 20, 1, 40, 400, 14),
	(2, 20, 35, 24207, true, 76, 0, 20, 20, 1, 40, 400, 14);