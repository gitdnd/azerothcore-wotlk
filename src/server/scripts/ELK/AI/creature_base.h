#pragma once 


#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Unit/Unit.h"
#include "ScriptedGossip.h"

#include "../Quest/QuestStageFlags.h"
#include "../Spells/spell_elk_include.h" 

#define DoComboIfAvailable(x) if(EasyCast(x)) return
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
    CHECK_HEALTH,
    REGULAR_CHECK,
    RETURN_CHECK,
};

class ELKCreatureScript : public CreatureScript
{
protected:
    ELKCreatureScript(const char* name) : CreatureScript(name) {}  
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

    uint8 reinforcementCall = RAND(1, 2);

    uint8 chanceAtk = RAND(2, 3);
    uint8 chanceDef = RAND(0, 1);
    uint8 chanceSpell = 0;
    uint8 chanceBuff = 0;

    uint8 optionAtk = 0;
    uint8 optionDef = 0;
    uint8 optionSpell = 0;
    uint8 optionBuff = 0;
    uint8 mode = 0;

    std::map<uint32, uint16> baseCooldowns;
    std::map<uint32, uint16> baseCost;

    std::vector<uint32> reinforcementEntries;

    void Reset() override
    {
        events.Reset();
        comboing = 0;
    }
    void DelayAttack()
    {
        me->setAttackTimer(BASE_ATTACK, me->GetAttackTime(BASE_ATTACK));
    }
    bool EasyCast(int spell)
    {
        
        SpellCastResult ret = me->CastSpell(me, spell, false);
        if (ret == SPELL_CAST_OK)
        {
            me->AddSpellCooldown(spell, 0, baseCooldowns[spell]);
            return true;
        }
        return false;
    }
    bool EasyCastTarget(int spell)
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
    void EasyQueCombo(int attack)
    { 
        uint32 atkTime = me->GetAttackTime(BASE_ATTACK);
        events.ScheduleEvent(attack, me->getAttackTimer(BASE_ATTACK));
        me->setAttackTimer(BASE_ATTACK, atkTime);
        comboing = 1;
    }
    void ReinforcementCall()
    {
        auto target = me->GetVictim();
        if (!target)
            return;
        uint16 fighting = target->InCombatWithHowMany();
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
    void EnterCombat(Unit* who) override
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
            uint8 rnd = RAND(1, (int)optionAtk);
            for (uint8 i = 0; i < optionAtk; i++)
            {
                bool exit = false;
                RandomAtk((i + rnd) % (optionAtk), exit);
                if (exit)
                {
                    break;
                }
            }
            break;
        }
        case 2:
        {
            uint8 rnd = RAND(1, (int)optionDef);
            for (uint8 i = 0; i < optionDef; i++)
            {
                bool exit = false;
                RandomDef((i + rnd) % (optionDef), exit);
                if (exit)
                {
                    break;
                }
            }
            break;
        }
        case 3:
        { 
            uint8 rnd = RAND(1, (int)optionSpell);
            for (uint8 i = 0; i < optionSpell; i++)
            {
                bool exit = false;
                RandomSpell((i + rnd) % (optionSpell), exit);
                if (exit)
                {
                    break;
                }
            }
            break;
        }
        case 4:
        {
            uint8 rnd = RAND(1, (int)optionBuff);
            for (uint8 i = 0; i < optionBuff; i++)
            {
                bool exit = false;
                RandomBuff((i + rnd) % (optionBuff), exit);
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
