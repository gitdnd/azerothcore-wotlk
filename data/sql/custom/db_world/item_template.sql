delimiter ;;
create procedure item_template_elk ()
begin
    declare continue handler for 1060 begin end;	
	ALTER TABLE `item_template`
	ADD COLUMN elk int NOT NULL DEFAULT 0;
end;;
call item_template_elk();;
DROP PROCEDURE  item_template_elk;;

UPDATE `item_template`
SET `RequiredLevel` = ( ( ( (`RequiredLevel` * 5)/ 8) + 10) / 5 ) * 5, elk = 1
WHERE elk != 1;
