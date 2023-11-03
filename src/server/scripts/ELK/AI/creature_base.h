#pragma once 

#include <sstream>
#include <ELK/nlohmann/json.hpp>

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Unit/Unit.h"
#include "ScriptedGossip.h"
#include "GameObjectAI.h"

#include "../Quest/QuestStageFlags.h"
#include "../Spells/spell_elk_include.h" 

#define DoComboIfAvailable(x) if(EasyCast(x)) return

#define ExtractJson(val, json, index) if (json.find(index) != json.end())	\
{																			\
	val = json[index];														\
}

using json = nlohmann::json;

enum Events : uint16
{
    NONE,
    ATK_1,
    ATK_2,
    ATK_3,
    ATK_4,
    ATK_5,
    ATK_6,
    ATK_7,
    DEF_1,
    DEF_2,
    DEF_3,
    SPL_1,
    SPL_2,
    SPL_3,
    SPL_4,
    CHECK_HEALTH,
    REGULAR_CHECK,
    RETURN_CHECK,
    DYNAMIC_MOVEMENT_1,
};
struct ELKAI;

class ELKCreatureScript : public CreatureScript
{
protected:
    ELKCreatureScript(const char* name) : CreatureScript(name) {}

    struct DialogueLine
    {
        DialogueLine() {};
        DialogueLine(std::string textIndex, std::vector<std::string> flagNeededName, std::vector<std::string> flagAddedName, std::string nextResponse, void (ELKAI::* pressedScript)())
            : nextResponse(nextResponse),
            pressedScript(pressedScript)
        {
            ELKDialogue* dial = sObjectMgr->GetELKDialogue(textIndex);
            if (dial)
                textLine = dial->dialogue;
            else
                textLine = "";
            for (std::string& flag : flagNeededName)
            {
                flagNeeded.push_back(sObjectMgr->GetELKFlag(flag));
            }
            for (std::string& flag : flagAddedName)
            {
                flagAdded.push_back(sObjectMgr->GetELKFlag(flag));
            }
        }
        std::string textLine = "";
        std::vector<uint32> flagNeeded = {};
        std::vector<uint32> flagAdded = {};
        std::string nextResponse = "";
        uint8 nextIndex = 0;
        void (ELKAI::* pressedScript)() = nullptr; // ai.appearScript = &ELKAI::ScriptFunction;
    };

    struct DialogueResponse
    {
        DialogueResponse() {};
        DialogueResponse(std::string textName, std::vector<std::string> flagNeededName, std::vector<DialogueLine> lines, void (ELKAI::* scriptMethod)())
            : textName(textName),
            lines(lines),
            appearScript(scriptMethod)
        {
            ELKDialogue* dial = sObjectMgr->GetELKDialogue(textName);
            if (dial)
                textIndex = dial->id + 1000000;
            else
                textIndex = 1000001;
            for (std::string& flag : flagNeededName)
            {
                flagNeeded.push_back(sObjectMgr->GetELKFlag(flag));
            }






            // get the shit in the deebee
            //std::string nextLines = DB[string];
            //std::vector<std::string> v = SplitLines(nextLines);
        }
        std::vector<std::string> SplitLines(std::string const& str) {
            std::vector<std::string> ret;
            std::stringstream ss(str);
            std::string item;

            while (getline(ss, item, ' ')) {
                ret.push_back(item);
            }

            return ret;
        }

        std::string textName = "";
        uint32 textIndex = 0;
        std::vector<uint32> flagNeeded = {};
        std::vector<DialogueLine> lines = {};
        void (ELKAI::* appearScript)() = nullptr; // ai.appearScript = &ELKAI::ScriptFunction;
    };

    struct ResponseHolder
    {
    private:
        std::vector<DialogueResponse> responsesArray = {};
        std::map<std::string, uint16> responseIndexes = {};
    public:
        void AddResponseIndex(std::string str)
        {
            if (responseIndexes[str] == 0)
            {
                responseIndexes[str] = responseIndexes.size() + 1;
            }
        }

        void Insert(DialogueResponse const& response)
        {
            AddResponseIndex(response.textName);
            for (auto& line : response.lines)
            {
                if (line.nextResponse != "")
                    AddResponseIndex(line.nextResponse);
            }
            responsesArray.push_back(response);
        }
        void Refine()
        {
            for (auto& response : responsesArray)
            {
                for (auto& line : response.lines)
                {
                    line.nextIndex = responseIndexes[line.nextResponse];
                }
            }
        }
    };
};
struct ELKAI : public ScriptedAI
{
    ELKAI(Creature* creature) : ScriptedAI(creature)
    {
        reinforcementEntries.push_back(me->GetEntry());
    }
    EventMap events;

    uint8 comboing = 0;
    std::vector<WorldLocation> positions{me->GetWorldLocation()};

    uint8 reinforcementCall = RandomInt(1, 2);

    uint8 chanceAtk = RandomInt(2,3);
    uint8 chanceDef = RandomInt(0, 1);
    uint8 chanceSpell = 0;
    uint8 chanceBuff = 0;

    uint8 optionAtk = 0;
    uint8 optionDef = 0;
    uint8 optionSpell = 0;
    uint8 optionBuff = 0;
    uint8 mode = 0;

    uint8 lastCategory = 0;
    uint8 lastChoice = 0;

    std::map<uint32, uint16> baseCooldowns;
    std::map<uint32, uint16> baseCost;

    std::vector<uint32> reinforcementEntries;

    void Reset() override
    {
        ResetExtra();
        events.Reset();
        comboing = 0;

        lastCategory = 0;
        lastChoice = 0;
    }

    virtual void ResetExtra() {};

    void DelayAttack()
    {
        me->setAttackTimer(BASE_ATTACK, me->GetAttackTime(BASE_ATTACK));
    }
    bool EasyAttack(uint32 spell, uint16 event, uint16 delay)
    {
        SpellCastResult ret = me->CastSpell(me, spell, false);
        if (ret == SPELL_CAST_OK)
        {
            comboing++;
            events.ScheduleEvent(event, delay);
            return true;
        }
        comboing = 0;
        return false;
    }
    bool EasyCast(uint32 spell)
    {
        
        SpellCastResult ret = me->CastSpell(me, spell, false);
        if (ret == SPELL_CAST_OK)
        {
            me->AddSpellCooldown(spell, 0, baseCooldowns[spell]);
            return true;
        }
        return false;
    }
    bool EasyCastLocation(uint32 spell, Position point)
    {
        auto target = me->GetVictim();
        if (!target || me->GetSpellCooldown(spell))
            return false;
        SpellCastResult ret = me->CastSpell(point.GetPositionX(), point.GetPositionY(), point.GetPositionZ(), spell, false);
        if (ret == SPELL_CAST_OK)
        {
            me->AddSpellCooldown(spell, 0, baseCooldowns[spell]);
            return true;
        }
        return false;
    }
    bool EasyCastTarget(uint32 spell)
    {
        auto target = me->GetVictim();
        if (!target || me->GetSpellCooldown(spell))
            return false;
        SpellCastResult ret = me->CastSpell(target, spell, false);
        if (ret == SPELL_CAST_OK)
        {
            me->AddSpellCooldown(spell, 0, baseCooldowns[spell]);
            return true;
        }
        return false;
    }
    void EasyQueCombo(uint32 attack)
    { 
        uint32 atkTime = me->GetAttackTime(BASE_ATTACK);
        events.ScheduleEvent(attack, 0);
        me->setAttackTimer(BASE_ATTACK, atkTime);
        comboing = 1;
    }
    void ReinforcementCall()
    {
        auto target = me->GetVictim();
        if (!target)
            return;
        uint16 fighting = target->GetThreatMgr().GetThreatListSize();
        if (fighting < reinforcementCall)
        {
            std::list<Creature*> cList;
            
            for (auto i : reinforcementEntries)
            {
                me->GetCreaturesWithEntryInRange(cList, 40.0f, i);
            }
            for (auto cre : cList)
            {
                if (cre->IsInCombat() == false and cre->IsAlive() == true)
                {
                    fighting++;
                    cre->CombatStart(target);
                    cre->AddThreat(target, 1.0f);
                    target->AddThreat(cre, 1.0f);
                }
                if (fighting >= reinforcementCall)
                    break;
            }
        }
    }
    virtual void EnterCombatCustom(Unit* /*who*/)
    {
        
    }
    void JustEngagedWith(Unit* who) override
    {
        EnterCombatCustom(who);
        ReinforcementCall();
    }
    uint8 RandomOrder()
    {
        uint8 result = rand() % (chanceAtk + chanceDef + chanceSpell + chanceBuff);
        if (result < chanceAtk)
            return 1;
        result -= chanceAtk;
        if (result < chanceDef)
            return 2;
        result -= chanceDef;
        if (result < chanceSpell)
            return 3;
        result -= chanceSpell;
        if (result < chanceBuff)
            return 4;
        return 0;
    }
    virtual void MovementInform(uint32 type, uint32 point)
    {
        if (type != POINT_MOTION_TYPE)
            return;
    }

    void RandomAction()
    {
        uint8 action = RandomOrder();

        switch (action)
        {
        case 1:
        {
            uint8 rnd = rand() % optionAtk;
            for (uint8 i = 0; i < optionAtk; i++)
            {
                uint8 choice = (i + rnd) % (optionAtk);
                if (action == lastCategory && lastChoice == choice)
                    continue;
                lastCategory = action;
                lastChoice = choice;
                bool exit = false;
                RandomAtk(choice, exit);
                if (exit)
                {
                    break;
                }
            }
            break;
        }
        case 2:
        {
            uint8 rnd = rand() % optionDef;
            for (uint8 i = 0; i < optionDef; i++)
            {
                uint8 choice = (i + rnd) % (optionAtk);
                if (action == lastCategory && lastChoice == choice)
                    continue;
                lastCategory = action;
                lastChoice = choice;
                bool exit = false;
                RandomDef(choice, exit);
                if (exit)
                {
                    break;
                }
            }
            break;
        }
        case 3:
        { 
            uint8 rnd = rand() % optionSpell;
            for (uint8 i = 0; i < optionSpell; i++)
            {
                uint8 choice = (i + rnd) % (optionAtk);
                if (action == lastCategory && lastChoice == choice)
                    continue;
                lastCategory = action;
                lastChoice = choice;
                bool exit = false;
                RandomSpell(choice, exit);
                if (exit)
                {
                    break;
                }
            }
            break;
        }
        case 4:
        {
            uint8 rnd = rand() % optionBuff;
            for (uint8 i = 0; i < optionBuff; i++)
            {
                uint8 choice = (i + rnd) % (optionAtk);
                if (action == lastCategory && lastChoice == choice)
                    continue;
                lastCategory = action;
                lastChoice = choice;
                bool exit = false;
                RandomBuff(choice, exit);
                if (exit)
                {
                    break;
                }
            }
            break;
        }
        default:
            break;
        }
    }
    virtual void RandomAtk(uint8, bool&) {};
    virtual void RandomDef(uint8, bool&) {};
    virtual void RandomSpell(uint8, bool&) {};
    virtual void RandomBuff(uint8, bool&) {};


};

/*
class magistrix_erona : public ELKCreatureScript
{
public:

    static inline ResponseHolder responses = {};

    magistrix_erona() : ELKCreatureScript("magistrix_erona")
    {
        responses.Insert(DialogueResponse(
            "ERONA_INTRO_1",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_1A", {}, {}, "ERONA_TALK_2", nullptr),
                DialogueLine("ERONA_DIALOGUE_1B", {}, {}, "ERONA_TALK_2", nullptr),
                DialogueLine("ERONA_DIALOGUE_1C", {}, {}, "ERONA_TALK_2", nullptr)
            },
            nullptr
        ));

        responses.Insert(DialogueResponse(
            "ERONA_TALK_2",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2B", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2C", {}, {}, "ERONA_TALK_2C", nullptr)
            },
            nullptr
        ));

        responses.Insert(DialogueResponse(
            "ERONA_TALK_2C",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2B", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2C", {}, {}, "ERONA_TALK_2C", nullptr)
            },
            nullptr
        ));

        Responses.emplace(EronaResponses::TALK_2C, DialogueResponse(
            "ERONA_TALK_2C",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2B", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2CA", {}, {}, "ERONA_TALK_2CA", nullptr),
                DialogueLine("ERONA_DIALOGUE_2CB", {}, {}, "ERONA_TALK_2CB", nullptr)
            },
            nullptr
        ));

        responses.Refine();







        Responses.emplace(EronaResponses::TALK_2CA, DialogueResponse(
            "ERONA_TALK_2CA",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, EronaResponses::TALK_3, nullptr),
                DialogueLine("ERONA_DIALOGUE_2B", {}, {}, EronaResponses::TALK_3, nullptr)
            },
            nullptr
        ));
        Responses.emplace(EronaResponses::TALK_2CB, DialogueResponse(
            "ERONA_TALK_2CB",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, EronaResponses::TALK_3, nullptr),
                DialogueLine("ERONA_DIALOGUE_2B", {}, {}, EronaResponses::TALK_3, nullptr)
            },
            nullptr
        ));
        Responses.emplace(EronaResponses::TALK_3, DialogueResponse(
            "ERONA_TALK_2CB",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, EronaResponses::TALK_4, nullptr)
            },
            nullptr
        ));
    }
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new magistrix_eronaAI(creature);
    }
    struct magistrix_eronaAI : public ELKAI
    {
        magistrix_eronaAI(Creature* creature) : ELKAI(creature) {}

        virtual void sGossipHello(Player* player)  override
        {
            if (player->GetQuestStageFlag(QuestStageFlags::FELENDREN_PUT_DOWN))
            {
                AddGossipItemFor(player, 0, "<Attack> You're dead.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN_ATTACK);
                AddGossipItemFor(player, 0, "<Return> Never mind, I'm being hasty. What else can I do?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_PUT_DOWN_RESET, me);
            }
        }
        void sGossipSelect(Player* player, uint32 sender, uint32 uiAction) override
        {
            ClearGossipMenuFor(player);
            switch (uiAction)
            {
            case FELENDREN_LEAVE_DIALOGUE:
            {
                AddGossipItemFor(player, 0, "What happens to be your problem?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_1);
                AddGossipItemFor(player, 0, "Speak quickly or you'll be a skinned rat soon.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_1);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<Attack> Never mind, you're just another pathetic waste of Mana.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_ASSAULTED);
                SendGossipMenuFor(player, FELENDREN_TALK_1, me);
                break;
            }
        }
    };

};
*/

class ELKCreatureTemplate
{
    struct ELKCAction
    {
        std::string name;
        uint8 min_dist;
        uint8 max_dist;
        uint16 delay;
        uint8 weight;
        // enum target;
    };
    struct ELKCAJump : ELKCAction
    {
        float angle;
        float speedXY;
        float speedZ;
    };
    struct ELKCASpell : ELKCAction
    {
        uint32 id;
    };
    // Per action, first uint8 is action starting index, second number amount of actions - calculated by Actions[first + 0...second - 1]
    std::map<enum ActionType, std::map<uint8, uint8> Combos = {};
    std::vector<ELKAction> Actions = {};
    "reinforcementCall": "0",
        "chanceAtk" : "5",
        "chanceDef" : "2",
        "chanceSpell" : "3",
        "SpellPowerBonus" : "300",

    static inline std::map <std::string, ELKCreatureTemplate*> = {};
    ELKCreatureTemplate(std::string Name)
    {
        if (json.find(index) != json.end())	\
        {																			\
            val = json[index];														\
        }
    }
}




