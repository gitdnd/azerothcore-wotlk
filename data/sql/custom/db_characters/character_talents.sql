delimiter ;; 
create procedure character_talent_elk ()
begin
    declare continue handler for 1060 begin end;	
	ALTER TABLE `character_talent`
	ADD COLUMN development tinyint NOT NULL DEFAULT 1,
	ADD COLUMN talent_rank tinyint NOT NULL DEFAULT 1,
	ADD COLUMN crit INT NOT NULL DEFAULT 0;
end;;
call character_talent_elk();;
DROP PROCEDURE  character_talent_elk;;
