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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Unit/Unit.h"

enum Spells
{
    SPELL_ACTION_QUICK_ATTACK= 89993,
    SPELL_ACTION_ATTACK     = 90000,
    SPELL_LEAP              = 100001,
    SPELL_DEVOURING_RAGE    = 100002,
    SPELL_BLOOD_HOWL        = 100003,
    SPELL_WOLF_FORM           = 100004,
    SPELL_ACTION_WOLF_BITE    = 100005,
};
enum Events
{
    NONE,
    C_ATK1_1,
    C_ATK1_2,
    C_ATK1_P200,
    C_ATK1_Q1,
    CHECK_HEALTH,
    REGULAR_CHECK
};


class bastard_wolf : public CreatureScript
{
public:
    bastard_wolf() : CreatureScript("bastard_wolf") {}

    
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bastard_wolfAI(creature);
    }

    struct bastard_wolfAI : public ScriptedAI
    {
        bastard_wolfAI(Creature* creature) : ScriptedAI(creature) {}

        EventMap events;

        bool comboing = false;

        void Reset() override
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(REGULAR_CHECK, 3000);   
            if (rand() / 3 == 0)
            {
                events.ScheduleEvent(CHECK_HEALTH, 5000);
            }
        }

        void UpdateAI(uint32 diff) override
        {

            if (!UpdateVictim())
                return;

            events.Update(diff);
            auto target = me->GetVictim();
            
            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }



            if (me->isMoving())
            {
                if (me->GetDistance(target) > 6)
                    me->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_LEAP, false);
                return;
            }
            

            switch (events.ExecuteEvent())
            {
            case C_ATK1_1:
                me->setAttackTimer(BASE_ATTACK, me->GetAttackTime(BASE_ATTACK));
                me->CastSpell((Unit*) nullptr, SPELL_ACTION_ATTACK, false);
                events.ScheduleEvent(C_ATK1_2, 500);
                break;
            case C_ATK1_2:
                me->setAttackTimer(BASE_ATTACK, me->GetAttackTime(BASE_ATTACK));
                me->CastSpell((Unit*) nullptr, SPELL_ACTION_ATTACK, false);
                if (rand() % 2 == 0)
                    events.ScheduleEvent(C_ATK1_P200, 700);
                else
                    events.ScheduleEvent(C_ATK1_Q1, 500);
                break;
            case C_ATK1_P200:
                me->CastSpell((Unit*) nullptr, SPELL_ACTION_ATTACK, false);
                comboing = false;
                me->setAttackTimer(BASE_ATTACK, me->GetAttackTime(BASE_ATTACK));
                break;
            case C_ATK1_Q1:
                me->CastSpell((Unit*) nullptr, SPELL_ACTION_QUICK_ATTACK, false);
                comboing = false;
                me->setAttackTimer(BASE_ATTACK, me->GetAttackTime(BASE_ATTACK));
                break;
            case CHECK_HEALTH:
                if (comboing)
                    break;
                if (me->HealthBelowPct(60))
                {
                    me->CastSpell(me, SPELL_DEVOURING_RAGE, false);
                    break;
                }
            case REGULAR_CHECK:
                if (comboing)
                    break;
                events.ScheduleEvent(REGULAR_CHECK, 3000);
                if (me->GetDistance(target) < 3)
                {
                    int action = rand() % 10;
                    switch (action)
                    {
                    case 0:
                    {
                        me->CastSpell((Unit*) nullptr, SPELL_ACTION_WOLF_BITE, false);
                        break;
                    }
                    case 1:
                    {
                        me->CastSpell(target, SPELL_BLOOD_HOWL, false);
                        break;
                    }
                    case 2:
                    case 3:
                    case 4:
                    {
                        events.ScheduleEvent(C_ATK1_1, 500);
                        comboing = true;
                        break;
                    }
                    default:
                        me->CastSpell((Unit*) nullptr, SPELL_ACTION_ATTACK, false);
                        break;
                    }
                    break;
                }
            default:
                DoMeleeAttackIfReady();
                return;
            }
        }
        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != POINT_MOTION_TYPE)
                return;
        }
    };
};

void AddSC_animal_mobs()
{
    new bastard_wolf();
}
