delimiter ;; 
create procedure character_talent_elk ()
begin
    declare continue handler for 1060 begin end;	
	ALTER TABLE `character_talent`
	ADD COLUMN development tinyint NOT NULL DEFAULT 1;
end;;
call character_talent_elk();;
DROP PROCEDURE  character_talent_elk;;
