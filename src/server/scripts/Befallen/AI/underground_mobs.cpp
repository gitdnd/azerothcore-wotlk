#include "action_creature.h"



class failed_immortal : public CreatureScript
{
public:
    failed_immortal() : CreatureScript("failed_immortal") {}

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new failed_immortalAI(creature);
    }

    struct failed_immortalAI : public BefallenAI
    {
        failed_immortalAI(Creature* creature) : BefallenAI(creature) {}

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(REGULAR_CHECK, 1000);
            if (rand() % 5 == 0)
            {
                events.ScheduleEvent(CHECK_HEALTH, 3000);
            }
        }

        void UpdateAI(uint32 diff) override
        {

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }

            auto target = me->GetVictim();

            if (me->isMoving())
            {
                if (me->GetDistance(target) > 9)
                    me->CastSpell(target, SPELL_ACTION_DEMONBOLT, false);
                return;
            }

            switch (events.ExecuteEvent())
            {
            case CHECK_HEALTH:
                if (comboing)
                {
                    events.ScheduleEvent(CHECK_HEALTH, 3000);
                }
                else if (me->HealthBelowPct(25))
                {
                    me->CastSpell(me, SPELL_DEVOURING_RAGE, false);
                }
                else
                    events.ScheduleEvent(CHECK_HEALTH, 3000);
                break;
            case ATK_1:
            {
                switch (comboing)
                {
                case 1:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_UNDEATH_STRIKE);
                    events.ScheduleEvent(ATK_1, 1000);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_UNDEATH_STRIKE);
                    events.ScheduleEvent(ATK_1, 1000);
                    comboing = 3;
                    break;
                case 3:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_UNDEATH_STRIKE);
                    comboing = 0;
                    break;
                }
                break;
            }
            case ATK_2:
            {
                switch (comboing)
                {
                case 1:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_UNDEATH_STRIKE);
                    events.ScheduleEvent(ATK_2, 500);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_QUICK_ATTACK);
                    events.ScheduleEvent(ATK_2, 250);
                    comboing = 3;
                    break;
                case 3:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_QUICK_ATTACK);
                    events.ScheduleEvent(ATK_2, 250);
                    comboing = 0;
                    break;
                }
                break;
            }
            case ATK_3:
            {
                switch (comboing)
                {
                case 1:
                    if (rand() % 9 == 0)
                    {
                        comboing = 0;
                    }
                    else
                    {
                        DelayAttack();
                        EasyCast(SPELL_ACTION_UNDEATH_STRIKE);
                        events.ScheduleEvent(ATK_3, 500);
                        comboing = 2;
                    }
                    break;  
                case 2:
                    if (rand() % 9 == 0)
                    {
                        comboing = 0;
                    }
                    else
                    {
                        DelayAttack();
                        EasyCast(SPELL_ACTION_QUICK_ATTACK);
                        events.ScheduleEvent(ATK_3, 250);
                        comboing = 1;
                    }
                    break;
                }
                break;
            }
            case REGULAR_CHECK:
                events.ScheduleEvent(REGULAR_CHECK, 3000);
                if (comboing)
                    break;
                if (me->GetDistance(target) < 3)
                {
                    int action = rand() % 14;
                    switch (action)
                    {
                    case 0:
                    {
                        EasyCastTarget(SPELL_ACTION_HOLY_SHOCK);
                        break;
                    }
                    case 4:
                        me->CastSpell(me, SPELL_ACTION_HOLY_SHOCK, false);
                        break;
                    case 1:
                    case 2:
                    case 3:
                    {
                        EasyQueCombo(ATK_1);
                        break;
                    }
                    case 5:
                    case 6:
                    case 7:
                    {
                        EasyQueCombo(ATK_2);
                        break;
                    }
                    case 9:
                    {
                        EasyQueCombo(ATK_3);
                        break;
                    }
                    case 10:
                    case 12:
                    {
                        EasyCast(SPELL_ACTION_THUNDER_CLAP);
                        break;
                    }
                    case 11:
                    case 8:
                    {
                        EasyCastTarget(SPELL_ACTION_KICK);
                        break;
                    }
                    }
                }
                break;
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
class terrorized_specimen : public CreatureScript
{
public:
    terrorized_specimen() : CreatureScript("terrorized_specimen") {}

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new terrorized_specimenAI(creature);
    }

    struct terrorized_specimenAI : public BefallenAI
    {
        terrorized_specimenAI(Creature* creature) : BefallenAI(creature) {}

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(REGULAR_CHECK, 1500);
            if (rand() % 2 == 0)
            {
                events.ScheduleEvent(CHECK_HEALTH, 3000);
            }
        }

        void UpdateAI(uint32 diff) override
        {

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }

            auto target = me->GetVictim();


            switch (events.ExecuteEvent())
            {
            case CHECK_HEALTH:
                if (comboing)
                {
                    events.ScheduleEvent(CHECK_HEALTH, 3000);
                    break;
                }
                if (me->HealthBelowPct(50))
                {
                    me->CastSpell(me, SPELL_DEVOURING_RAGE, false);
                    break;
                }
                else
                    events.ScheduleEvent(CHECK_HEALTH, 3000);
                break;
            case D_1:
            {
                switch (comboing)
                {
                case 1:
                    DelayAttack();
                    events.ScheduleEvent(D_1, 400);
                    EasyCast(SPELL_ACTION_DEFLECT);
                    comboing++;
                    break;
                case 2:
                    DelayAttack();
                    events.ScheduleEvent(D_1, 400);
                    EasyCast(SPELL_ACTION_DEFLECT);
                    comboing++;
                    break;
                case 3:
                    comboing = false;
                    DelayAttack();
                    if (rand() % 2 == 0)
                    {
                        EasyCast(SPELL_ACTION_ATTACK);
                    }
                    else
                    {
                        EasyCast(SPELL_ACTION_DEFLECT);
                    }
                    comboing = 0;
                    break;
                }
                break;
            }
            case ATK_1:
                switch (comboing)
                {
                case 1:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    events.ScheduleEvent(ATK_1, 500);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    if (rand() % 2 == 0)
                    {
                        events.ScheduleEvent(ATK_1, 700);
                        comboing = 3;
                    }
                    else
                    {
                        events.ScheduleEvent(ATK_1, 500);
                        comboing = 4;
                    }
                    break;
                case 3:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    comboing = 0;
                    break;
                case 4:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_QUICK_ATTACK);
                    comboing = 0;
                    break;
                }
            case ATK_2:
                switch (comboing)
                {
                case 1:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    events.ScheduleEvent(ATK_2, 500);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    events.ScheduleEvent(ATK_2, 250);
                    comboing = 3;
                    break;
                case 3:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    events.ScheduleEvent(ATK_2, 250);
                    comboing = 4;
                    break;
                case 4:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    comboing = 0;
                    break;
                }
            case REGULAR_CHECK:
                events.ScheduleEvent(REGULAR_CHECK, 3000);
                if (comboing)
                    break;
                if (me->GetDistance(target) < 3)
                {
                    int action = rand() % 10;
                    switch (action)
                    {
                    case 0:
                    {
                        EasyCast(SPELL_ACTION_WOLF_BITE);
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
                        auto atkTime = me->GetAttackTime(BASE_ATTACK);
                        events.ScheduleEvent(ATK_1, me->getAttackTimer(BASE_ATTACK));
                        me->setAttackTimer(BASE_ATTACK, atkTime);
                        comboing = true;
                        break;
                    }
                    case 5:
                    case 6:
                    {
                        auto atkTime = me->GetAttackTime(BASE_ATTACK);
                        events.ScheduleEvent(ATK_2, me->getAttackTimer(BASE_ATTACK));
                        me->setAttackTimer(BASE_ATTACK, atkTime);
                        comboing = true;
                    }
                    case 7:
                    {
                        EasyCast(SPELL_ACTION_DEFLECT);
                        break;
                    }
                    case 8:
                    {
                        auto atkTime = me->GetAttackTime(BASE_ATTACK);
                        events.ScheduleEvent(D_1, 400);
                        me->setAttackTimer(BASE_ATTACK, atkTime);
                        comboing = true;
                        break;
                    }
                    case 9:
                    {
                        if (target)
                        {
                            if (target->HasUnitState(UNIT_STATE_CASTING))
                            {
                                EasyCast(SPELL_ACTION_DEFLECT);
                            }
                        }
                    }
                    default:
                        break;
                    }
                    break;
                }
                break;
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

class labyrinth_minotaur : public CreatureScript
{
public:
    labyrinth_minotaur() : CreatureScript("labyrinth_minotaur") {}

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new labyrinth_minotaurAI(creature);
    }

    struct labyrinth_minotaurAI : public BefallenAI
    {
        labyrinth_minotaurAI(Creature* creature) : BefallenAI(creature) {}

        bool engaged = false;

        void EnterCombat(Unit* /*who*/) override
        {
            events.ScheduleEvent(REGULAR_CHECK, 3000);
            if (rand() % 3 == 0)
            {
                events.ScheduleEvent(CHECK_HEALTH, 1000);
            }
        }

        void UpdateAI(uint32 diff) override
        {

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }

            auto target = me->GetVictim();

            if (me->isMoving())
            {
                if (me->GetDistance(target) > 6)
                    me->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_ACTION_LEAP, false);
                return;
            }

            // if HP < 100% or enemy in range, set engaged to true.
            // while not engaged, will just stare at the victim.
            // will also occasionally do slow walks towards its starting destination.

            switch (events.ExecuteEvent())
            {
            case CHECK_HEALTH:
                if (comboing)
                {
                    events.ScheduleEvent(CHECK_HEALTH, 3000);
                }
                else if (me->HealthBelowPct(50))
                {
                    me->CastSpell(me, SPELL_DEVOURING_RAGE, false);
                }
                else
                    events.ScheduleEvent(CHECK_HEALTH, 3000);
                break;
            case D_1:
            {
                switch (comboing)
                {
                case 1:
                    DelayAttack();
                    events.ScheduleEvent(D_1, 400);
                    EasyCast(SPELL_ACTION_DEFLECT);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    events.ScheduleEvent(D_1, 400);
                    EasyCast(SPELL_ACTION_DEFLECT);
                    comboing = 3;
                    break;
                case 3:
                    comboing = false;
                    DelayAttack();
                    if (rand() % 2 == 0)
                    {
                        EasyCast(SPELL_ACTION_ATTACK);
                    }
                    else
                    {
                        EasyCast(SPELL_ACTION_DEFLECT);
                    }
                    comboing = 0;
                    break;
                default:
                    break;
                }
                break;
            }
            case ATK_1:
            {
                switch (comboing)
                {
                case 1:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    events.ScheduleEvent(ATK_1, 500);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    if (rand() % 2 == 0)
                    {
                        events.ScheduleEvent(ATK_1, 700);
                        comboing = 3;
                    }
                    else
                    {
                        events.ScheduleEvent(ATK_1, 500);
                        comboing = 4;
                    }
                    break;
                case 3:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    comboing = 0;
                    break;
                case 4:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_QUICK_ATTACK);
                    comboing = 0;
                    break;
                default:
                    break;
                }
                break;
            }
            case ATK_2:
            {
                switch (comboing)
                {
                case 1:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    events.ScheduleEvent(ATK_2, 500);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_QUICK_ATTACK);
                    events.ScheduleEvent(ATK_2, 250);
                    comboing = 3;
                    break;
                case 3:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_QUICK_ATTACK);
                    events.ScheduleEvent(ATK_2, 250);
                    comboing = 4;
                    break;
                case 4:
                    DelayAttack();
                    EasyCast(SPELL_ACTION_ATTACK);
                    comboing = 0;
                    break;
                default:
                    break;
                }
                break;
            }
            case REGULAR_CHECK:
                events.ScheduleEvent(REGULAR_CHECK, 3000);
                if (comboing)
                    break;
                if (me->GetDistance(target) < 3)
                {
                    int action = rand() % 10;
                    switch (action)
                    {
                    case 0:
                    {
                        EasyCast(SPELL_ACTION_WOLF_BITE);
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
                        EasyQueCombo(ATK_1);
                        break;
                    }
                    case 5:
                    case 6:
                    {
                        EasyQueCombo(ATK_2);
                    }
                    case 7:
                    {
                        EasyCast(SPELL_ACTION_DEFLECT);
                        break;
                    }
                    case 8:
                    {
                        EasyQueCombo(D_1);
                        break;
                    }
                    case 9:
                    {
                        if (target)
                        {
                            if (target->HasUnitState(UNIT_STATE_CASTING))
                            {
                                EasyCast(SPELL_ACTION_DEFLECT);
                            }
                        }
                    }
                    default:
                        break;
                    }
                    break;
                }
                break;
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

void AddSC_underground_mobs()
{
    new failed_immortal();
    new labyrinth_minotaur();
}
