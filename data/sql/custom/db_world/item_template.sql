delimiter ;;
create procedure item_template_elk ()
begin
    declare continue handler for 1060 begin end;	
	ALTER TABLE item_template
	ADD COLUMN elk int NOT NULL DEFAULT 0;
end;;
call item_template_elk();;
DROP PROCEDURE  item_template_elk;;

UPDATE item_template
SET elk = 1,
RequiredLevel = 
CASE WHEN RequiredLevel < 2 THEN 0 
ELSE ( ( ( (RequiredLevel * 5)/ 8) + 10) / 5 ) * 5 END
WHERE elk != 1;

UPDATE item_template SET class = 7, stackable = 30, bonding = 0 WHERE (entry = 20482);
UPDATE item_template SET class = 7, Quality = 1, spellcharges_1 = -1, spellcooldown_1 = 100, spellid_1 = 400002 WHERE (entry = 20845);
UPDATE item_template SET class = 0, Quality = 1, spellcharges_1 = -1, spellcooldown_1 = 100, spellid_1 = 400001, Flags = "1088"  WHERE (entry = 20846);
UPDATE item_template SET class = 7, name = 'Pure Wraith Essence' WHERE (entry = 20934);
UPDATE item_template SET class = 0 WHERE (entry = 20935);

/* Blood Elf starting zone */
UPDATE `item_template` SET `BuyPrice` = 20, `SellPrice` = 10, `StatsCount` = 1, `delay` = 5000, `stat_value1` = 14, `dmg_min1` = 8, `dmg_max1` = 13 WHERE (`entry` = 20849);
UPDATE `item_template` SET `BuyPrice` = 20, `SellPrice` = 10, `StatsCount` = 1, `delay` = 4000, `stat_value1` = 11, `dmg_min1` = 7, `dmg_max1` = 12 WHERE (`entry` = 20850);
UPDATE `item_template` SET `BuyPrice` = 20, `SellPrice` = 10, `StatsCount` = 1, `delay` = 3000, `stat_value1` = 8, `dmg_min1` = 6, `dmg_max1` = 11 WHERE (`entry` = 20851);
UPDATE `item_template` SET `BuyPrice` = 20, `SellPrice` = 10, `StatsCount` = 1, `delay` = 2000, `stat_value1` = 5, `dmg_min1` = 5, `dmg_max1` = 10 WHERE (`entry` = 20852);
UPDATE `item_template` SET `BuyPrice` = 20, `SellPrice` = 10, `StatsCount` = 1, `delay` = 7000, `stat_value1` = 20, `dmg_min1` = 8, `dmg_max1` = 15 WHERE (`entry` = 20853);


UPDATE `item_template` SET `BuyPrice` = 20, `SellPrice` = 10, `StatsCount` = 2, `stat_type1` = 3, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 8, `armor` = 2 WHERE (`entry` = 20911);
UPDATE `item_template` SET `BuyPrice` = 20, `SellPrice` = 10, `name` = 'Large Blood Elf Shield', `armor` = 10, `arcane_res` = 4, `block` = 3 WHERE (`entry` = 20912);

UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 4, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 1, `armor` = 4 WHERE (`entry` = 20914);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 4, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 1, `armor` = 4 WHERE (`entry` = 20915);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 4, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 1, `armor` = 4 WHERE (`entry` = 20916);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 4, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 1, `armor` = 4 WHERE (`entry` = 20917);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 4, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 1, `armor` = 4 WHERE (`entry` = 20918);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 4, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 1, `armor` = 4 WHERE (`entry` = 20919);

UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 3, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 2,  `armor` = 3 WHERE (`entry` = 20920);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 3, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 2,  `armor` = 3 WHERE (`entry` = 20921);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 3, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 2,  `armor` = 3 WHERE (`entry` = 20922);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 3, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 2,  `armor` = 3 WHERE (`entry` = 20923);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 3, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 2,  `armor` = 3 WHERE (`entry` = 20924);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 3, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 2,  `armor` = 3 WHERE (`entry` = 20925);

UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 5, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 3,  `armor` = 2 WHERE (`entry` = 20985);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 5, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 3,  `armor` = 2 WHERE (`entry` = 20986);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 5, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 3,  `armor` = 2 WHERE (`entry` = 20987);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 5, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 3,  `armor` = 2 WHERE (`entry` = 20988);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 5, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 3,  `armor` = 2 WHERE (`entry` = 20989);
UPDATE `item_template` SET `BuyPrice` = 10, `SellPrice` = 5, `StatsCount` = 2, `stat_type1` = 5, `stat_value1` = 1, `stat_type2` = 1, `stat_value2` = 3,  `armor` = 2 WHERE (`entry` = 20990);




REPLACE INTO item_template (entry, class, subclass, SoundOverrideSubclass, name, displayid, Quality, Flags, FlagsExtra, BuyCount, BuyPrice, SellPrice, InventoryType, AllowableClass, AllowableRace, ItemLevel, RequiredLevel, RequiredSkill, RequiredSkillRank, requiredspell, requiredhonorrank, RequiredCityRank, RequiredReputationFaction, RequiredReputationRank, maxcount, stackable, ContainerSlots, StatsCount, stat_type1, stat_value1, stat_type2, stat_value2, stat_type3, stat_value3, stat_type4, stat_value4, stat_type5, stat_value5, stat_type6, stat_value6, stat_type7, stat_value7, stat_type8, stat_value8, stat_type9, stat_value9, stat_type10, stat_value10, ScalingStatDistribution, ScalingStatValue, dmg_min1, dmg_max1, dmg_type1, dmg_min2, dmg_max2, dmg_type2, armor, holy_res, fire_res, nature_res, frost_res, shadow_res, arcane_res, delay, ammo_type, RangedModRange, spellid_1, spelltrigger_1, spellcharges_1, spellppmRate_1, spellcooldown_1, spellcategory_1, spellcategorycooldown_1, spellid_2, spelltrigger_2, spellcharges_2, spellppmRate_2, spellcooldown_2, spellcategory_2, spellcategorycooldown_2, spellid_3, spelltrigger_3, spellcharges_3, spellppmRate_3, spellcooldown_3, spellcategory_3, spellcategorycooldown_3, spellid_4, spelltrigger_4, spellcharges_4, spellppmRate_4, spellcooldown_4, spellcategory_4, spellcategorycooldown_4, spellid_5, spelltrigger_5, spellcharges_5, spellppmRate_5, spellcooldown_5, spellcategory_5, spellcategorycooldown_5, bonding, description, PageText, LanguageID, PageMaterial, startquest, lockid, Material, sheath, RandomProperty, RandomSuffix, block, itemset, MaxDurability, area, Map, BagFamily, TotemCategory, socketColor_1, socketContent_1, socketColor_2, socketContent_2, socketColor_3, socketContent_3, socketBonus, GemProperties, RequiredDisenchantSkill, ArmorDamageModifier, duration, ItemLimitCategory, HolidayId, ScriptName, DisenchantID, FoodType, minMoneyLoot, maxMoneyLoot, flagsCustom, VerifiedBuild, elk) VALUES

(100001, 5, 0, -1, 'Inspiration', 64062, 6, 2048, 0, 1, 0, 0, 0, 262143, 32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 1, 'You can learn as many recipes as you have inspiration. Once you unlearn all recipes, you will go back to having 3 inspiration.', 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 8192, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, '', 0, 0, 0, 0, 0, 11159, 1),
(100002, 7, 6, -1, 'Crafting Scales', 8564, 1, 0, 0, 1, 900, 225, 0, -1, -1, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, '', 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, '', 0, 0, 0, 0, 0, 12340, 1),
(100003, 0, 0, -1, 'Random Copper Item', 7391, 1, 0, 0, 1, 0, 0, 0, -1, -1, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 'Forges a random item of copper material. Has a chance to create higher tier items.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 'elk_item_random_craft', 0, 0, 0, 0, 0, 12340, 1);