import openpyxl
from pathlib import Path

PrintAll = [

]

def ExcelSearch():
    for d in Path('./').rglob('*.xlsx'):
        excelFile = openpyxl.load_workbook(d)
        try:
            for sheet in excelFile.worksheets:
                rows = sheet.max_row
                columns = sheet.max_column
                for i in range(1, rows + 1):
                    id = (sheet.cell(row = i, column = 1).value)
                    text = (sheet.cell(row = i, column = 2).value)
                    if id is not None and text is not None:
                        PrintAll.append((sheet.title + "_" + id + '", "' + text))

        except:
            pass

ExcelSearch()

file = open('../elk_dialogue_text.sql', 'w')
file.write('''
DELIMITER ;;
DROP PROCEDURE IF EXISTS create_elk_dialogue_text;
CREATE PROCEDURE create_elk_dialogue_text ()
BEGIN
    DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;
	CREATE TABLE elk_dialogue_text (
    id int NOT NULL AUTO_INCREMENT,
    text_name varchar(64) NOT NULL,
    dialogue TEXT NOT NULL,
	UNIQUE (id),
	UNIQUE (text_name)
   );
END;;
CALL create_elk_dialogue_text();;
DROP PROCEDURE  create_elk_dialogue_text;;

TRUNCATE TABLE elk_dialogue_text;
DROP PROCEDURE IF EXISTS add_elk_dialogue_text;
SET @dialogueAdded = 1;


CREATE PROCEDURE add_elk_dialogue_text (IN text_nameP VARCHAR(64), dialogueP TEXT)
BEGIN
   DECLARE CONTINUE HANDLER FOR 1050 BEGIN END;
	REPLACE INTO `elk_dialogue_text` (`id`, `text_name`, `dialogue`) VALUES
	(@dialogueAdded, text_nameP, dialogueP);
	REPLACE INTO `npc_text` (`ID`, `text0_0`, `Probability0`) VALUES
	(@dialogueAdded + 1000000, dialogueP, 1);
	SET @dialogueAdded = @dialogueAdded + 1;
END;;




CALL add_elk_dialogue_text ("DOTDOTDOT", "...");


''')
for item in PrintAll:
    file.write('CALL add_elk_dialogue_text ("'+item+'");\n')

file.close()
