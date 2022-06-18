delimiter ;;
create procedure creature_template_elk ()
begin
    declare continue handler for 1060 begin end;	
	ALTER TABLE `creature_template`
	ADD COLUMN elk int NOT NULL DEFAULT 0;
end;;
call creature_template_elk();;
DROP PROCEDURE  creature_template_elk;;
UPDATE `creature_template`
SET `AIName` = 'CombatAI'
WHERE `AIName` = 'SmartAI';

UPDATE `creature_template`
SET `minlevel` = ( ( (`minlevel` - 1) * 5) / 8) + 11, `maxlevel` = `minlevel` + 2, elk = 1
WHERE elk != 1;

UPDATE creature_template SET ScriptName='wretched' WHERE entry=15644;
UPDATE creature_template SET ScriptName='mana_wyrm' WHERE entry=15274;