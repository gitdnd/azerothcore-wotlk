#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"




class npc_landlord : public CreatureScript
{
public:
    npc_landlord() : CreatureScript("npc_landlord") { }

    struct npc_landlordAI : public ScriptedAI
    {
        npc_landlordAI(Creature* c) : ScriptedAI(c)
        {

        }

    };

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        sScriptMgr->LandlordGossip(player, creature);
        return false;
    }



    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_landlordAI(pCreature);
    }
};

void AddSC_eco_housing()
{
    new npc_landlord();

}
