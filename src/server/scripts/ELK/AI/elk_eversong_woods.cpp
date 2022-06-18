#include "creature_base.h"



class mana_wyrm : public CreatureScript
{
public:
    mana_wyrm() : CreatureScript("mana_wyrm") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mana_wyrmAI(creature);
    }
    struct mana_wyrmAI : public ELKAI
    {
        mana_wyrmAI(Creature* creature) : ELKAI(creature)
        {
            reinforcementCall = 1;
            chanceAtk = 3;
            chanceDef = 1;
            chanceSpell = 1;
        };
        void EnterCombatCustom(Unit* /*who*/) override
        {
            events.ScheduleEvent(REGULAR_CHECK, 1000);
        }
        void UpdateAI(uint32 diff) override
        {
            //GetCollisionWidth // if close enough, do kick/jab/w/e

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
                if (me->GetDistance(target) > 3 and me->GetDistance(target) <= 8 and rand() % 5 == 0)
                    me->CastSpell(target, SpellsC::ARCANE_TORRENT, false);
                return;
            }
            switch (events.ExecuteEvent())
            {
            case ATK_1:
            {
                switch (comboing)
                {
                case 1:
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_1, 750);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_1, 750);
                    comboing = 3;
                    break;
                case 3:
                    DelayAttack();
                    if(rand() % 2 == 0)
                        EasyCast(ATTACK);
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
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_2, 1000);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_2, 1000);
                    comboing = 3;
                    break;
                case 3:
                    DelayAttack();
                    EasyCast(ATTACK);
                    comboing = 0;
                    break;
                }
                break;
            }
            case REGULAR_CHECK:
                events.ScheduleEvent(REGULAR_CHECK, 1000);
                if (comboing)
                    break;
                if (me->GetDistance(target) < 3)
                {
                    int action = RandomOrder();
                    switch (action)
                    {
                        // ATTACK
                    case 1:
                    {
                        switch (RAND(1, 4))
                        {
                        case 1:
                        case 2:
                        {
                            EasyQueCombo(ATK_1);
                            break;
                        }
                        case 3:
                        case 4:
                        {
                            EasyQueCombo(ATK_2);
                            break;
                        }
                        }
                    }
                    // ATTACK END
                    case 2:
                    {
                        EasyCastTarget(SPELL_DEFLECT);
                        break;
                    }
                    case 3:
                        EasyCastTarget(SpellsC::ARCANE_TORRENT);
                        break;
                    default:
                        break;
                    }
                }
                break;
            default:
                DoMeleeAttackIfReady();
                return;
            }
        }
    };
};

class wretched : public CreatureScript
{
public:
    wretched() : CreatureScript("wretched") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new wretchedAI(creature);
    }
    struct wretchedAI : public ELKAI
    {
        wretchedAI(Creature* creature) : ELKAI(creature)
        {
            reinforcementCall = RAND(1, 2);
            entries.push_back(15949);
            chanceAtk = 5;
            chanceDef = 2;
            chanceSpell = 1;
        }
        void EnterCombatCustom(Unit* /*who*/) override
        {
            mode = RAND(0, 1);
            events.ScheduleEvent(REGULAR_CHECK, 1000);
            if (rand() % 4 == 0)
            {
                events.ScheduleEvent(CHECK_HEALTH, 1000);
            }
        }
        void UpdateAI(uint32 diff) override
        {
            //GetCollisionWidth // if close enough, do kick/jab/w/e

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
                if (me->GetDistance(target) > 3 and me->GetDistance(target) <= 8 and rand() % 10 == 0)
                    me->CastSpell(target, SpellsC::ARCANE_TORRENT, false);
                else if (me->GetDistance(target) > 9)
                    me->CastSpell(target, SpellsC::ARCANE_BOLT, false);
                return;
            }
            switch (events.ExecuteEvent())
            {
            case CHECK_HEALTH:
                if (!comboing and me->HealthBelowPct(25))
                {
                    me->CastSpell(me, SpellsC::FRENZY, false);
                    mode = 0;
                }
                else
                    events.ScheduleEvent(CHECK_HEALTH, 1000);
                break;
            case ATK_1:
            {
                switch (comboing)
                {
                case 1:
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_1, 750);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_1, 750);
                    comboing = 3;
                    break;
                case 3:
                    DelayAttack();
                    EasyCast(ATTACK);
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
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_2, 700);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_2, 250);
                    comboing = 3;
                    break;
                case 3:
                    DelayAttack();
                    EasyCast(ATTACK);
                    comboing = 0;
                    break;
                }
                break;
            }
            case ATK_3:
            {
                if (rand() % 9 == 0)
                {
                }
                else
                {
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_1, 750);
                }
                break;
            }
            case REGULAR_CHECK:
                events.ScheduleEvent(REGULAR_CHECK, 1000);
                if (comboing)
                    break;
                if (me->GetDistance(target) < 3)
                {
                    int action = RandomOrder();
                    switch (action)
                    {
                    // ATTACK
                    case 1:
                    {
                        if(mode == 0)
                        {
                            switch (RAND(0, 2))
                            {
                            case 0:
                                EasyCastTarget(SpellsC::THROW);
                                break;
                            case 1:
                                EasyCastTarget(SpellsC::MIND_FLAY);
                                break;
                            case 2:
                                EasyCastTarget(SpellsC::ARCANE_BOLT);
                                break;
                            }
                        }
                        else
                        {
                            switch (RAND(1, 8))
                            {
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
                            case 8:
                            {
                                me->CastSpell(me, SpellsC::FRENZY, false);
                                mode = 0;
                                EasyQueCombo(ATK_3);
                                break;
                            }
                            }
                        }
                    }
                    // ATTACK END
                    case 2:
                    {
                        EasyCastTarget(SPELL_DEFLECT);
                        break;
                    }
                    case 3:
                        EasyCastTarget(SpellsC::ARCANE_BOLT);
                        break;
                    default:
                        break;
                    }
                }
                break;
            default:
                DoMeleeAttackIfReady();
                return;
            }
        }
    };
};

void AddSC_elk_eversong_woods_mobs()
{
    new mana_wyrm();
    new wretched();
}
