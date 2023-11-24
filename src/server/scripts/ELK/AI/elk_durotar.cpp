#include "creature_base.h"

class lazy_peon : public ELKCreatureScript
{
public:
    lazy_peon() : ELKCreatureScript("lazy_peon") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new lazy_peonAI(creature, this);
    }
    struct lazy_peonAI : public ELKAI
    {
        lazy_peonAI(Creature* creature, const ELKCreatureScript* script) : ELKAI(creature, script)
        {
            switch (creature->GetSpawnId())
            {
            case 3345:
                scriptedPos[0] = Position(-228.318, -4277.61, 65.3247, 1.02993);
                scriptedPos[1] = Position(-226.786, -4285.76, 64.9699, 5.15956);
                scriptedPos[2] = Position(-228.477, -4283.62, 65.1967, 2.89447);
                break;
            case 3346:
                scriptedPos[0] = Position(-331.7, -4428.64, 53.782, 2.2342);
                scriptedPos[1] = Position(-334.504, -4440.13, 54.717, 4.46866);
                scriptedPos[2] = Position(-329.996, -4433.72, 54.7811, 5.10483);
                break;
            case 3347:
                scriptedPos[0] = Position(-231.603, -4452.81, 63.9317, 4.11774);
                scriptedPos[1] = Position(-234.1, -4450.19, 63.2157, 2.95221);
                scriptedPos[2] = Position(-228.19, -4448.53, 63.7461, 0.026598);
                break;
            case 3348:
                scriptedPos[0] = Position(-516.797, -4375.86, 46.1781, 3.60939);
                scriptedPos[1] = Position(-509.903, -4372.29, 45.6313, 0.489787);
                scriptedPos[2] = Position(-510.443, -4378.93, 45.732, 5.15506);
                break;
            case 6523:
                scriptedPos[0] = Position(-318.613, -4133.32, 52.7755, 0.337633);
                scriptedPos[1] = Position(-317.23, -4124.11, 51.8386, 1.36258);
                scriptedPos[2] = Position(-320.966, -4127.11, 51.7729, 3.5617);
                break;
            case 6524:
                scriptedPos[0] = Position(-629.117, -4468.45, 46.6277, 1.30535);
                scriptedPos[1] = Position(-634.474, -4482.16, 46.1243, 4.78333);
                scriptedPos[2] = Position(-635.911, -4477.31, 46.4219, 2.25568);
                break;
            case 6525:
                scriptedPos[0] = Position(-755.114, -4323.08, 43.8142, 0.698278);
                scriptedPos[1] = Position(-757.436, -4326, 45.4192, 4.05193);
                scriptedPos[2] = Position(-759.838, -4318.94, 44.0533, 5.04232);
                break;
            case 6526:
                scriptedPos[0] = Position(-369.242, -4017.57, 50.6491, 5.22157);
                scriptedPos[1] = Position(-374.909, -4015.79, 50.5308, 2.83788);
                scriptedPos[2] = Position(-371.201, -4012.19, 50.8522, 1.33777);
                break;
            case 6527:
                scriptedPos[0] = Position(-755.82, -4147.39, 37.6741, 2.23197);
                scriptedPos[1] = Position(-752.944, -4140.56, 39.4023, 1.43557);
                scriptedPos[2] = Position(-752.253, -4144.26, 38.0483, 2.00027);
                break;
            case 7372:
                scriptedPos[0] = Position(-210.64, -4231.78, 63.4257, 3.37757);
                scriptedPos[1] = Position(-212.516, -4219.29, 62.1373, 1.92066);
                scriptedPos[2] = Position(-209.51, -4227.29, 63.727, 1.11955);
                break;
            case 7373:
                scriptedPos[0] = Position(-769.523, -4203.34, 43.9795, 3.57738);
                scriptedPos[1] = Position(-775.303, -4194.23, 43.4298, 2.23435);
                scriptedPos[2] = Position(-769.858, -4197.43, 42.2445, 4.90077);
                break;
            case 7374:
                scriptedPos[0] = Position(-621.197, -4336.11, 41.1525, 0.0108);
                scriptedPos[1] = Position(-623.236, -4349.7, 41.0866, 4.7389);
                scriptedPos[2] = Position(-626.904, -4343.6, 41.5611, 1.888);
                break;
            case 7375:
                scriptedPos[0] = Position(-269.136, -4137.83, 56.1272, 1.37121);
                scriptedPos[1] = Position(-263.703, -4144.72, 56.1565, 5.55737);
                scriptedPos[2] = Position(-269.336, -4144.42, 55.1774, 4.24184);
                break;
            case 7376:
                scriptedPos[0] = Position(-494.018, -4459.84, 51.5334, 4.78675);
                scriptedPos[1] = Position(-498.779, -4457.59, 51.1254, 2.98033);
                scriptedPos[2] = Position(-492.12, -4452.65, 50.877, 0.643769);
                break;
            default:
                scriptedPos[0] = Position(0,0,0,0);
                scriptedPos[1] = Position(0, 0, 0, 0);
                scriptedPos[2] = Position(0, 0, 0, 0);
                break;
            }
        };
        void Reset() override
        {
            eventsOOC.ScheduleEvent(RETURN_CHECK, 1000);
            me->GetMotionMaster()->MovePoint(0, scriptedPos[2]);

            repeats = 0;
            peonStage = 0;
        }
        void UpdateAI(uint32 diff) override
        {

            if (!UpdateVictim())
            {
                eventsOOC.Update(diff);
                if (uint8 eventId = eventsOOC.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case RETURN_CHECK: 
                        if (me->GetDistance(scriptedPos[2]) < 3)
                        {
                            eventsOOC.CancelEvent(RETURN_CHECK);
                            me->CastSpell(me, 17743, true);
                        }
                        else
                        {
                            me->GetMotionMaster()->MovePoint(0, scriptedPos[2]);
                            eventsOOC.ScheduleEvent(RETURN_CHECK, 1000);
                        }
                        break;
                    case REGULAR_CHECK: 
                        eventsOOC.ScheduleEvent(REGULAR_CHECK, 1000);
                        switch (peonStage)
                        {
                        case 0:
                            me->SetWalk(false);
                            me->GetMotionMaster()->MovePoint(0, scriptedPos[1]);
                            break;
                        case 3:
                            me->SetSheath(SHEATH_STATE_MELEE);
                            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 173);
                            me->PlayDistanceSound(6197, nullptr);
                            me->SetWalk(true);
                            break;
                        case 6:
                            me->SetSheath(SHEATH_STATE_UNARMED);
                            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                            me->GetMotionMaster()->MovePoint(0, scriptedPos[0]);
                            break;
                        case 12:
                            me->AddAura(68442, me);
                            break;
                        case 14:
                        {
                            std::list<GameObject*> woodList;
                            me->GetGameObjectListWithEntryInGrid(woodList, 500001, 20);
                            if (woodList.size() > 0)
                            {
                                auto woodPile = woodList.front();
                                LootStoreItem storeItem(LootStoreItem(4470, 0, 100, false, LOOT_MODE_DEFAULT, 0, 1, 1));
                                woodPile->loot.AddItem(storeItem);
                                woodPile->Respawn(); 
                            }
                            me->RemoveAura(68442);
                            break;
                        }
                        case 15:
                            repeats--;
                            peonStage = -1;
                            if (repeats <= 0)
                            {
                                eventsOOC.CancelEvent(REGULAR_CHECK);
                                eventsOOC.ScheduleEvent(RETURN_CHECK, 1000);
                            } 
                            break;
                        default: 
                            break;
                        }
                        peonStage++;
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                // events.Update(diff);
                DoMeleeAttackIfReady();
            }
        }
        void EnterCombat(Unit* /*victim*/)
        {
            eventsOOC.Reset();
            peonStage = 0;
            repeats = 0;
        }
        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            if (spell->Id == 19938)
            {
                peonStage = 0;
                me->RemoveAura(17743);
                eventsOOC.ScheduleEvent(REGULAR_CHECK, 1000);
                eventsOOC.CancelEvent(RETURN_CHECK);
                me->PlayDistanceSound(RAND(0, 1) ? 6292 : 6294, nullptr);
                me->Say(5774, caster);
                if(Player* player = caster->ToPlayer())
                    player->RewardPlayerAndGroupAtEvent(me->GetCreatureTemplate()->Entry, player);
                me->SetWalk(false);
                repeats = RAND(10, 60);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
            }
        }
        Position scriptedPos[3];
        EventMap eventsOOC;
        uint8 peonStage = 0;
        uint8 repeats = 0;
    };
};


void AddSC_elk_durotar_mobs()
{
    new lazy_peon();
}
