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
            ELKCUpdateAI();


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 1;
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
            ELKCUpdateAI();


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 1;
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
            ELKCUpdateAI();

            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 1;
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
            ELKCUpdateAI();


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 1;
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
            ELKCUpdateAI();


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 1;
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
            ELKCUpdateAI();


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 1;
            DoEvents();
        }

    };
};

class milley : public ELKCreatureScript
{
public:
    milley() : ELKCreatureScript("milley")
    {
        AddScriptFlag("MILLEY_GIVES_GRAPES", givesGrapes);
        AddScriptFlag("MILLEY_WAS_RUDE", wasRude);
    }
    uint32 givesGrapes;
    uint32 wasRude;
    static const uint32 MilleysHarvestId = 3904;
    bool CreatureResponseScript(Player* player, PlayerChoice* choice, CreatureResponse* response) const override
    {
        if (binary_search(response->flagsAdded.begin(), response->flagsAdded.end(), givesGrapes)) {
            if (uint32 harvest = player->GetItemCount(MilleysHarvestId))
            {
                player->DestroyItemCount(MilleysHarvestId, harvest, true);
                player->ModifyMoney(player->GetQuestStageFlag(wasRude) ? harvest * 2 : harvest * 3);
                return true;
            }
            return false;
        }
        return true;
    }
};

class rogue_wizard : public ELKCreatureScript
{
public:
    rogue_wizard() : ELKCreatureScript("rogue_wizard") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new rogue_wizardAI(creature, this);
    }
    struct rogue_wizardAI : public ELKAI
    {
        rogue_wizardAI(Creature* creature, const ELKCreatureScript* script) : ELKAI(creature, script)
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
            ELKCUpdateAI();
            DoEvents();
        }



    };
};
class hogger : public ELKCreatureScript
{
public:
    hogger() : ELKCreatureScript("hogger") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new hoggerAI(creature, this);
    }
    struct hoggerAI : public ELKAI
    {
        hoggerAI(Creature* creature, const ELKCreatureScript* script) : ELKAI(creature, script)
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
            ELKCUpdateAI();


            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 1;
            DoEvents();
        }
        virtual void OnMutate() override
        {
            me->RemoveAura(2000004);
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
    new hogger();
    new rogue_wizard();
    new cutpurse();
}





