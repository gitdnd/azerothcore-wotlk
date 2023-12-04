delimiter ;; 
create procedure character_spell_elk ()
begin
    declare continue handler for 1060 begin end;	
	ALTER TABLE `character_spell`
	ADD COLUMN crit INT NOT NULL DEFAULT 1;
end;;
call character_spell_elk();;
DROP PROCEDURE  character_spell_elk;;
