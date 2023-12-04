import math

input = '&^D2a Alright alright, just get them quickly&&&^D2b Thank you so much, you can find them across the bridge&&.' \
        ' &fINTRO Good seeing you!&&&f!INTRO Who are you anyway?&&' \
        ' &fCONTEST Werent you at the contest earlier today?&&' \
        ' &fMAGE Your spells are amazing!&&'

class Dialogue(object):
    AllDialogue = []
    def __init__(self, id, text, flagsUsed, flagsAdded, next):
        self.Text = text
        self.Id = id
        self.FlagsUsed = flagsUsed
        self.FlagsAdded = flagsAdded
        self.Next = next
        Dialogue.AllDialogue.append(self)

class GossipVariant(object):
    def __init__(self, text, neededOption, flagsVarYes, flagsVarNot):
        self.Text = text
        self.NeededOption = neededOption
        self.FlagsVarYes = flagsVarYes
        self.FlagsVarNot = flagsVarNot

class Gossip(object):
    AllGossip = []
    def __init__(self, id, flagsUsed, flagsAdded, next):
        self.Id = id
        self.FlagsUsed = flagsUsed
        self.FlagsAdded = flagsAdded
        self.Next = next
        self.Variants = []
        Gossip.AllGossip.append(self)
    def AddVariant(self, text, neededOption, flagsVarYes, flagsVarNot):
        self.Variants.append(GossipVariant(text, neededOption, flagsVarYes, flagsVarNot))

def RefineGossip(id, text, flagsUsed, flagsAdded, next):

    varianceByDialogue = []
    varianceByFlag = []
    textBlocks = []

    while True:
        begin = text.find('&')
        if begin == -1:
            textBlocks.append(
                {'type': 'always',
                 'text': text
                 }
            )
            break
        if begin > 1:
            textBlocks.append(
                {'type': 'always',
                 'text': text[1 : begin]
                 }
            )
        if text[begin + 1] == '^':
            nextWhiteSpace = text.find(' ', begin)
            macroEnd = text.find('&&')
            dialogueId = text[begin + 2 : nextWhiteSpace]
            if dialogueId not in varianceByDialogue:
                varianceByDialogue.append(dialogueId)
            textBlocks.append(
                {'type': 'byDialogue',
                 'text': text[nextWhiteSpace : macroEnd],
                 'dialogueId': dialogueId
                 }
            )
            text = text[macroEnd + 2 : ]
            continue

        if text[begin + 1] == 'f':
            if text[begin + 2] == '!':
                nextWhiteSpace = text.find(' ', begin)
                macroEnd = text.find('&&')
                flagId = text[begin + 3: nextWhiteSpace]
                if flagId not in varianceByFlag:
                    varianceByFlag.append(flagId)
                textBlocks.append(
                    {'type': 'byNotFlag',
                     'text': text[nextWhiteSpace: macroEnd],
                     'flagId': flagId
                     }
                )
                text = text[macroEnd + 2:]
            else:
                nextWhiteSpace = text.find(' ', begin)
                macroEnd = text.find('&&')
                flagId = text[begin + 2: nextWhiteSpace]
                if flagId not in varianceByFlag:
                    varianceByFlag.append(flagId)
                textBlocks.append(
                    {'type': 'byFlag',
                     'text': text[nextWhiteSpace: macroEnd],
                     'flagId': flagId
                     }
                )
                text = text[macroEnd + 2:]
            continue

    next = Gossip(id, flagsUsed, flagsAdded, next)

    if len(varianceByFlag) == 0:
        if len(varianceByDialogue) == 0:
            next.AddVariant(text, 0, [], [])
        else:
            for vD in varianceByDialogue:
                next.AddVariant(text, vD, [], [])
    elif len(varianceByDialogue) == 0:
        bitmask = pow(2, len(varianceByFlag))
        for i in range(1, bitmask + 1):
            text = ""
            flagsNeeded = []
            flagsDisallowed = []
            bits = int(max(8, math.log(i, 2)))
            out = [1 if i & (1 << (bits - 1 - n)) else 0 for n in range(bits)]
            x = -1
            for f in varianceByFlag:
                if out[x] == 1:
                    flagsNeeded.append(f)
                else:
                    flagsDisallowed.append(f)
                x = x - 1
            for tj in textBlocks:
                if tj['type'] == 'always':
                    text = text + tj['text']
                elif tj['type'] == 'byDialogue' and vD == tj['dialogueId']:
                    text = text + tj['text']
                elif tj['type'] == 'byFlag' and tj['flagId'] in flagsNeeded:
                    text = text + tj['text']
                elif tj['type'] == 'byNotFlag' and tj['flagId'] in flagsDisallowed:
                    text = text + tj['text']
            next.AddVariant(text, 0, flagsNeeded, flagsDisallowed)
    else:
        for vD in varianceByDialogue:
            bitmask = pow(2, len(varianceByFlag))
            for i in range(1, bitmask + 1):
                text = ""
                flagsNeeded = []
                flagsDisallowed = []
                bits = int(max(8, math.log(i, 2)))
                out = [1 if i & (1 << (bits - 1 - n)) else 0 for n in range(bits)]
                x = -1
                for f in varianceByFlag:
                    if out[x] == 1:
                        flagsNeeded.append(f)
                    else:
                        flagsDisallowed.append(f)
                    x = x - 1
                for tj in textBlocks:
                    if tj['type'] == 'always':
                        text = text + tj['text']
                    elif tj['type'] == 'byDialogue' and vD == tj['dialogueId']:
                        text = text + tj['text']
                    elif tj['type'] == 'byFlag' and tj['flagId'] in flagsNeeded:
                        text = text + tj['text']
                    elif tj['type'] == 'byNotFlag' and tj['flagId'] in flagsDisallowed:
                        text = text + tj['text']
                next.AddVariant(text, vD, flagsNeeded, flagsDisallowed)
def RefineDialogue(id, text, flagsUsed, flagsAdded, next):
    Dialogue(id, text, flagsUsed, flagsAdded, next)
