DELETE FROM `reference_loot_template` WHERE (`Entry` = 20013);
REPLACE INTO `reference_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(20013, 10940, 0, 50, 0, 1, 1, 1, 1, 'Wraith 1-5 - Strange Dust'),
(20013, 20847, 0, 50, 0, 1, 1, 1, 1, 'Wraith 1-5 - Wraith Fragment');
