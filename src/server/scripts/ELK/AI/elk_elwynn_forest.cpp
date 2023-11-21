#include "creature_base.h"


class diseased_pup : public ELKCreatureScript
{
public:
    diseased_pup() : ELKCreatureScript("diseased_pup") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new diseased_pupAI(creature, this);
    }
    struct diseased_pupAI : public ELKAI
    {
        diseased_pupAI(Creature* creature, const ELKCreatureScript* script) : ELKAI(creature, script)
        {
            
        };
        void EnterCombatCustom(Unit* /*who*/) override
        {

            events.ScheduleEvent(REGULAR_CHECK, regularCheck);
        }
        void ResetExtra() override
        {
        }
        void UpdateAI(uint32 diff) override
        {
            if (ELKUpdateAI(diff))
                return;
            if (!UpdateVictim())
                return;
            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 0;
            DoEvents();
        }



    };
};

class mange : public ELKCreatureScript
{
public:
    mange() : ELKCreatureScript("mange") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mangeAI(creature, this);
    }
    struct mangeAI : public ELKAI
    {
        mangeAI(Creature* creature, const ELKCreatureScript* script) : ELKAI(creature, script)
        {

        };
        void EnterCombatCustom(Unit* /*who*/) override
        {

            events.ScheduleEvent(REGULAR_CHECK, regularCheck);
        }
        void ResetExtra() override
        {
        }
        void UpdateAI(uint32 diff) override
        {
            if (ELKUpdateAI(diff))
                return;
            if (!UpdateVictim())
                return;
            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 0;
            DoEvents();
        }



    };
};

class night_prowler : public ELKCreatureScript
{
public:
    night_prowler() : ELKCreatureScript("night_prowler") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new night_prowlerAI(creature, this);
    }
    struct night_prowlerAI : public ELKAI
    {
        night_prowlerAI(Creature* creature, const ELKCreatureScript* script) : ELKAI(creature, script)
        {

        };
        void EnterCombatCustom(Unit* /*who*/) override
        {

            events.ScheduleEvent(REGULAR_CHECK, regularCheck);
        }
        void ResetExtra() override
        {
        }
        void UpdateAI(uint32 diff) override
        {
            if (ELKUpdateAI(diff))
                return;
            if (!UpdateVictim())
                return;
            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 0;
            DoEvents();
        }


    };
};
class defias : public ELKCreatureScript
{
public:
    defias() : ELKCreatureScript("defias") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new defiasAI(creature, this);
    }
    struct defiasAI : public ELKAI
    {
        defiasAI(Creature* creature, const ELKCreatureScript* script) : ELKAI(creature, script)
        {

        };
        void EnterCombatCustom(Unit* /*who*/) override
        {

            events.ScheduleEvent(REGULAR_CHECK, regularCheck);
        }
        void ResetExtra() override
        {
        }
        void UpdateAI(uint32 diff) override
        {
            if (ELKUpdateAI(diff))
                return;
            if (!UpdateVictim())
                return;
            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 0;
            DoEvents();
        }

    };
};


class padfoot : public ELKCreatureScript
{
public:
    padfoot() : ELKCreatureScript("padfoot") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new padfootAI(creature, this);
    }
    struct padfootAI : public ELKAI
    {
        padfootAI(Creature* creature, const ELKCreatureScript* script) : ELKAI(creature, script)
        {

        };
        void EnterCombatCustom(Unit* /*who*/) override
        {

            events.ScheduleEvent(REGULAR_CHECK, regularCheck);
        }
        void ResetExtra() override
        {
        }
        void UpdateAI(uint32 diff) override
        {
            if (ELKUpdateAI(diff))
                return;
            if (!UpdateVictim())
                return;
            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 0;
            DoEvents();
        }

    };
};


class cutpurse : public ELKCreatureScript
{
public:
    cutpurse() : ELKCreatureScript("cutpurse") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new cutpurseAI(creature, this);
    }
    struct cutpurseAI : public ELKAI
    {
        cutpurseAI(Creature* creature, const ELKCreatureScript* script) : ELKAI(creature, script)
        {

        };
        void EnterCombatCustom(Unit* /*who*/) override
        {

            events.ScheduleEvent(REGULAR_CHECK, regularCheck);
        }
        void ResetExtra() override
        {
        }
        void UpdateAI(uint32 diff) override
        {
            if (ELKUpdateAI(diff))
                return;
            if (!UpdateVictim())
                return;
            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 0;
            DoEvents();
        }

    };
};




void AddSC_elk_elwynn_forest_mobs()
{

    new diseased_pup();
    new mange();
    new night_prowler();
    new defias();
    new padfoot();

    new cutpurse();
}





