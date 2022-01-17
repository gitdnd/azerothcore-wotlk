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


enum Spells
{
    SPELL_ACTION_QUICK_ATTACK = 89993,
    SPELL_ACTION_DEFLECT      = 89999,
    SPELL_ACTION_ATTACK       = 90000,
    SPELL_ACTION_UNDEATH_STRIKE   = 90001,
    SPELL_ACTION_HOLY_SHOCK = 91002,
    SPELL_ACTION_THUNDER_CLAP = 92003,
    SPELL_ACTION_KICK = 93004,
    SPELL_ACTION_LEAP                = 100001,
    SPELL_DEVOURING_RAGE      = 100002,
    SPELL_BLOOD_HOWL          = 100003,
    SPELL_WOLF_FORM           = 100004,
    SPELL_ACTION_WOLF_BITE    = 100005,
};
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
    D_1,
    D_2,
    D_3,
    CHECK_HEALTH,
    REGULAR_CHECK
};

struct BefallenAI : public ScriptedAI
{

    BefallenAI(Creature* creature) : ScriptedAI(creature) {}
    EventMap events;

    int comboing = 0;

    void Reset() override
    {
        events.Reset();
        comboing = 0;
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
        auto atkTime = me->GetAttackTime(BASE_ATTACK);
        events.ScheduleEvent(Attack, me->getAttackTimer(BASE_ATTACK));
        me->setAttackTimer(BASE_ATTACK, atkTime);
        comboing = 1;
    }
};
