#include "creature_base.h"

class secret_chest : public GameObjectScript
{
public:
    secret_chest() : GameObjectScript("secret_chest") { }

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new secret_chestAI(go);
    }

    struct secret_chestAI : public GameObjectAI
    {
        secret_chestAI(GameObject* gameObject) : GameObjectAI(gameObject)
        {
        }
        bool GossipHello(Player* player, bool  /*reportUse*/) override
        {
            return false;
        }
        bool GossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            ClearGossipMenuFor(player); 
            return false;
        }
    };

};
void AddSC_elk_generic_AI()
{ 

    new secret_chest();
}
