delimiter ;; 
create procedure item_instance_elk ()
begin
    declare continue handler for 1060 begin end;	
	ALTER TABLE `item_instance`
	ADD COLUMN ilvl_bonus smallint unsigned NOT NULL DEFAULT 0;
end;;
call item_instance_elk();;
DROP PROCEDURE  item_instance_elk;;
