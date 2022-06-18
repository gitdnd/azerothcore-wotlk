/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Unit/Unit.h"

#include "../Spells/spell_elk_include.h"

enum Events
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
    REGULAR_CHECK
};

struct ELKAI : public ScriptedAI
{
    ELKAI(Creature* creature) : ScriptedAI(creature)
    {
        entries.push_back(me->GetEntry());
    }
    EventMap events;

    int comboing = 0;
    std::vector<WorldLocation> positions{me->GetWorldLocation()};

    int reinforcementCall = RAND(1, 2);

    int chanceAtk = RAND(2, 3);
    int chanceDef = RAND(0, 1);
    int chanceSpell = 0;
    int chanceBuff = 0;

    int mode = 0;

    std::vector<uint32> entries;

    void Reset() override
    {
        events.Reset();
        comboing = 0;
    }
    void DelayAttack()
    {
        me->setAttackTimer(BASE_ATTACK, me->GetAttackTime(BASE_ATTACK));
    }
    void EasyCast(int Spell)
    {
        me->CastSpell((Unit*) nullptr, Spell, false);
    }
    void EasyCastTarget(int Spell)
    {
        auto target = me->GetVictim();
        if (!target)
            return;
        me->CastSpell(target, Spell, false);
    }
    void EasyQueCombo(int Attack)
    {
        int atkTime = me->GetAttackTime(BASE_ATTACK);
        events.ScheduleEvent(Attack, me->getAttackTimer(BASE_ATTACK));
        me->setAttackTimer(BASE_ATTACK, atkTime);
        comboing = 1;
    }
    void ReinforcementCall()
    {
        auto target = me->GetVictim();
        if (!target)
            return;
        int fighting = target->InCombatWithHowMany();
        if (fighting < reinforcementCall)
        {
            std::list<Creature*> cList;
            
            for (auto i : entries)
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
    int RandomOrder()
    {
        int result = RAND(0, chanceAtk + chanceDef + chanceSpell + chanceBuff);
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
};
