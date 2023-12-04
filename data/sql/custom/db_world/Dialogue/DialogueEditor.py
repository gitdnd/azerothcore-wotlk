import shutil
import tkinter as tk
import re
from tkinter.filedialog import askopenfilename

master = tk.Tk()
master.title('Test')
master.geometry("1800x900")

currentFile = 0

fileMain = open("DialogueEditor.txt", "r")
currentFile = fileMain.readline()

class Row(object):
    Rows = 1
    Height = 1
    AllRows = []
    RowStart = 3

    def deleteMe(self):

        self.collapseButton.destroy()
        self.id.destroy()
        self.text.destroy()
        self.flagsNeeded.destroy()
        self.flagsGiven.destroy()
        self.options.destroy()
        self.delButton.destroy()
        Row.AllRows.remove(self)
        del()

    def collapseButton(self):
        if self.hidden == True:
            self.collapseButton.config(text="\/")
            self.hidden = False
        else:
            self.hidden = True
            self.collapseButton.config(text="^")
        Row.updateAll()
    @staticmethod
    def updateAll():

        def idSort(obj):
            return obj.row
        def getSlaves(obj, pos):
            if obj.row != 0:
                return pos + 1
            pos = pos + 1
            obj.row = pos
            for o in obj.slave:
                pos = getSlaves(o, pos)
            return pos

        for row in Row.AllRows:
            row.row = 0
            row.slave = []
            row.master = 0
        for rowX in Row.AllRows:
            childTags = rowX.options.get("1.0","end").split()
            for rowY in Row.AllRows:
                if rowY.id.get("1.0","end") == "\n" or rowX.options.get("1.0","end") == "\n":
                    continue
                idY = rowY.id.get("1.0","end").rstrip()
                idY.replace('\n', '')
                if idY in childTags:
                    if rowY.master == 0:
                        rowX.slave.append(rowY)
                        rowY.master = rowX

        position = Row.RowStart
        for row in Row.AllRows:
            def IsMasterHidden(obj):
                if obj.master == 0:
                    return False
                while True:
                    obj2 = obj.master
                    if obj2.hidden == True:
                        return True
                    if obj2.master == 0:
                        return False
                    obj = obj2

            if IsMasterHidden(row) == True:
                print(position)

                continue
            if row.row == 0:
                position = getSlaves(row, position)
        for row in Row.AllRows:
            row.update(row.row)

    def __init__(self):
        Row.Rows = Row.Rows + 1

        self.row = Row.Rows
        Row.AllRows.append(self)

        self.hidden = False
        self.depth = 0
        self.collapseButton = tk.Button(command=self.collapseButton,text='\/',height = self.Height, width = 1)
        self.id = tk.Text(height = self.Height, width = 12)
        self.text = tk.Text(height = self.Height, width = 64)
        self.flagsNeeded = tk.Text(height = self.Height, width = 24)
        self.flagsGiven = tk.Text(height = self.Height, width = 24)
        self.options = tk.Text(height = self.Height, width = 24)
        self.delButton = tk.Button(command=self.deleteMe,text='X',height = self.Height, width = 1)


    def update(self, position):

        self.collapseButton.grid(row=position, column=0)
        self.id.grid(row=position, column=2)
        self.text.grid(row=position, column=3)
        self.flagsNeeded.grid(row=position, column=4)
        self.flagsGiven.grid(row=position, column=5)
        self.options.grid(row=position, column=6)
        self.delButton.grid(row=position, column=7)
    def toText(self):
        def fixRow(obj):
            obj = obj.replace('\n', '')
            obj = obj + '||||'
            return obj

        ret = ''
        row = (self.id.get("1.0","end"))
        ret = ret + fixRow(row)

        row = (self.text.get("1.0","end"))
        ret = ret + fixRow(row)

        row = (self.flagsNeeded.get("1.0","end"))
        ret = ret + fixRow(row)

        row = (self.flagsGiven.get("1.0","end"))
        ret = ret + fixRow(row)

        row = (self.options.get("1.0","end"))
        ret = ret + fixRow(row)

        return ret
    def isEmpty(self):
        id = (self.id.get("1.0", "end"))
        text = (self.text.get("1.0", "end"))
        flagsNeeded = (self.flagsNeeded.get("1.0", "end"))
        flagsGiven = (self.flagsGiven.get("1.0", "end"))
        options = (self.options.get("1.0", "end"))
        if (id == "\n\n" or id == "\n" or not id) \
            and (text == "\n\n" or text == "\n" or not text) \
            and (flagsNeeded == "\n\n" or flagsNeeded == "\n" or not flagsNeeded) \
            and (flagsGiven == "\n\n" or flagsGiven == "\n" or not flagsGiven) \
            and (options == "\n\n" or options == "\n" or not options):

            return True
        return False
def AddRow():
    Row()
    Row.updateAll()

def Save():

    global currentFile
    if len(currentFile) < 3:
        currentFile = askopenfilename()
    shutil.copyfile(currentFile, currentFile[0:-4]+'backup.txt')
    file = open(currentFile,"w")
    file.truncate(0)
    for row in Row.AllRows:
        if not row.isEmpty():
            file.writelines(row.toText())
    file.close()
    global fileMain
    fileMain = open("DialogueEditor.txt", "w")
    fileMain.truncate(0)
    fileMain.write(currentFile)

def Load(filename):
    Row.AllRows.clear()
    file = open(filename,"r")
    member = 0
    row = 0
    for line in file.readlines():
        lineSplit = line.split('||||')
        row = Row()
        row.id.insert('1.0', lineSplit[1])
        row.text.insert('1.0', lineSplit[2])
        row.flagsNeeded.insert('1.0', lineSplit[3])
        row.flagsGiven.insert('1.0', lineSplit[4])
        row.options.insert('1.0', lineSplit[5])
    Row.updateAll()

def LoadFile():
    filename = askopenfilename()
    global currentFile
    if len(currentFile) > 0:
        Save()
    currentFile = filename
    Load(filename)

AddRowButton = tk.Button(command=AddRow,text='AddRow',height = 1, width = 3)
AddRowButton.grid(row=0, column=0)

SaveButton = tk.Button(command=Save,text='Save',height = 1, width = 3)
SaveButton.grid(row=0, column=1)

LoadButton = tk.Button(command=Load,text='Load',height = 1, width = 3)
LoadButton.grid(row=0, column=2)

LoadFileButton = tk.Button(command=LoadFile,text='LoadFile',height = 1, width = 3)
LoadFileButton.grid(row=0, column=3)

if len(currentFile) > 0:
    print(currentFile)
    Load(currentFile)
tk.mainloop()
