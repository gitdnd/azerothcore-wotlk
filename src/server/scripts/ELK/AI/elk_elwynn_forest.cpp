#include "creature_base.h"


class young_wolf : public ELKCreatureScript
{
public:
    young_wolf() : ELKCreatureScript("young_wolf") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new young_wolfAI(creature, this);
    }
    struct young_wolfAI : public ELKAI
    {
        young_wolfAI(Creature* creature, const ELKCreatureScript* script) : ELKAI(creature, script)
        {
            
        };
        void EnterCombatCustom(Unit* /*who*/) override
        {
            regularCheck = 3500;
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
        bool DoEventsExtra() override
        {
            if (me->GetDistance(me->GetVictim()) < 4)
            {
                if (rand() % 13 == 0)
                {
                    StartDynamicMovements();
                    
                    return true;
                }
            }
            return false;
        }

        void StartDynamicMovements() override
        {
            dynamicMovement.moveTime = 3500;
            dynamicMovement.angleMulti = 2.f;
            dynamicMovement.angleRaw = 3.14f;
            dynamicMovement.dist = 5.f;
            ELKAI::StartDynamicMovements();
        }
    };
};




void AddSC_elk_elwynn_forest_mobs()
{

    new young_wolf();

}





