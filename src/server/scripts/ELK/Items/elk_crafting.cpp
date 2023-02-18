#include "../Spells/spell_elk_include.h"

#include <random>

#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Spell.h"

class elk_item_random_craft : public ItemScript
{
public:
    elk_item_random_craft() : ItemScript("elk_item_random_craft") { }

    virtual bool OnItemCreate(Player const* player, Item* item) override
    {
        Player* owner = item->GetOwner();
        auto chanceMap = sObjectMgr->GetELKCraftingChance("COPPER_BASIC");
        float chanceTotal = 0.f;
        for (auto [k, v] : chanceMap)
        {
            chanceTotal += v;
        }
        std::uniform_real_distribution<float> distribution(0, chanceTotal);
        float outcome = distribution(World::twisterEngine);
        uint32 result = 2851;
        for (auto [k, v] : chanceMap)
        {
            if (v >= outcome)
            {
                result = k;
                break;
            }
            else
                outcome -= v;
        }
        owner->AddItem(result, 1);
        return false;
    }
};


void AddSC_elk_crafting()
{
    new elk_item_random_craft();
}
