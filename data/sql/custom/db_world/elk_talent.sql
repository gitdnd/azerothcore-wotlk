
DELIMITER ;;
DROP PROCEDURE IF EXISTS create_elk_talent;
CREATE PROCEDURE create_elk_talent ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;
	DROP TABLE IF EXISTS elk_talent_info;
	CREATE TABLE elk_talent_info (
    id int NOT NULL,
    element tinyint NOT NULL,
    armor_class tinyint NOT NULL,
    page_position tinyint NOT NULL,
    xp_cost_rank_multi tinyint NOT NULL,
    xp_cost_base int NOT NULL,
    stat1 tinyint NOT NULL,
    stat1amount tinyint NOT NULL,
    stat2 tinyint NOT NULL,
    stat2amount tinyint NOT NULL,
    stat3 tinyint NOT NULL,
    stat3amount tinyint NOT NULL,
	UNIQUE (id)
   );
END;;
CALL create_elk_talent();;
DROP PROCEDURE  create_elk_talent;;

TRUNCATE TABLE elk_talent_info;

SET @NORMAL                 = 0;
SET @HOLY                   = 1;
SET @FIRE                   = 2;
SET @NATURE                 = 3;
SET @FROST                  = 4;
SET @SHADOW                 = 5;
SET @ARCANE                 = 6;
	
SET @CLOTH                   = 1;
SET @LEATHER                 = 2;
SET @MAIL                    = 3;
SET @PLATE                   = 4;

SET @MANA                     = 0;
SET @HEALTH                   = 1;
SET @AGILITY                  = 3;
SET @STRENGTH                 = 4;
SET @INTELLECT                = 5;
SET @SPIRIT                   = 6;
SET @STAMINA                  = 7;
SET @DEFENSE_SKILL_RATING     = 12;
SET @DODGE_RATING             = 13;
SET @PARRY_RATING             = 14;
SET @BLOCK_RATING             = 15;
SET @HIT_MELEE_RATING         = 16;
SET @HIT_RANGED_RATING        = 17;
SET @HIT_SPELL_RATING         = 18;
SET @CRIT_MELEE_RATING        = 19;
SET @CRIT_RANGED_RATING       = 20;
SET @CRIT_SPELL_RATING        = 21;
SET @HIT_TAKEN_MELEE_RATING   = 22;
SET @HIT_TAKEN_RANGED_RATING  = 23;
SET @HIT_TAKEN_SPELL_RATING   = 24;
SET @CRIT_TAKEN_MELEE_RATING  = 25;
SET @CRIT_TAKEN_RANGED_RATING = 26;
SET @CRIT_TAKEN_SPELL_RATING  = 27;
SET @HASTE_MELEE_RATING       = 28;
SET @HASTE_RANGED_RATING      = 29;
SET @HASTE_SPELL_RATING       = 30;
SET @HIT_RATING               = 31;
SET @CRIT_RATING              = 32;
SET @HIT_TAKEN_RATING         = 33;
SET @CRIT_TAKEN_RATING        = 34;
SET @RESILIENCE_RATING        = 35;
SET @HASTE_RATING             = 36;
SET @EXPERTISE_RATING         = 37;
SET @ATTACK_POWER             = 38;
SET @RANGED_ATTACK_POWER      = 39;

SET @MANA_REGENERATION        = 43;
SET @ARMOR_PENETRATION_RATING = 44;
SET @SPELL_POWER              = 45;
SET @HEALTH_REGEN             = 46;
SET @SPELL_PENETRATION        = 47;
SET @BLOCK_VALUE              = 48;

REPLACE INTO `elk_talent_info` (`id`, `element`, `armor_class`, `page_position`, `xp_cost_rank_multi`, `xp_cost_base`, `stat1`, `stat1amount`, `stat2`, `stat2amount`, `stat3`, `stat3amount`) VALUES
(1000001, @HOLY, @PLATE, 1, 55, 1000, @ATTACK_POWER, 4, @MANA_REGENERATION, -1, 0, 0),
(1000005, @HOLY, @PLATE, 5, 27, 4000, @STRENGTH, 2, @MANA_REGENERATION, -1, 0, 0),
(1000009, @HOLY, @PLATE, 9, 33, 3000, @MANA, 10, @MANA_REGENERATION, -1, 0, 0),
(1000008, @HOLY, @PLATE, 21, 36, 10000, @SPIRIT, 10, @MANA_REGENERATION, -1, 0, 0)
;