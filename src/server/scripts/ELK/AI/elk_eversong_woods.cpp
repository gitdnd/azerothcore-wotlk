#include "creature_base.h"

enum EversongDialogue : uint32
{ 
    FELENDREN_INTRO = 200192,
    FELENDREN_TALK_1 = 200193,
    FELENDREN_TALK_2 = 200194,
    FELENDREN_TALK_3 = 200195,
    FELENDREN_TALK_4 = 200196,
    FELENDREN_TALK_5 = 200197,
    FELENDREN_TALK_6 = 200198,
    FELENDREN_PUT_DOWN = 200199,
    FELENDREN_EMBRACE_1 = 200200,
    FELENDREN_EMBRACE_2 = 200201,
    FELENDREN_EMBRACE_3 = 200202,
    FELENDREN_EMBRACE_4 = 200202,
    FELENDREN_TALK_7 = 200204,
    FELENDREN_TALK_8 = 200205,
    FELENDREN_PUT_DOWN_RESET = 200206,
    FELENDREN_INTRO_MET = 200207,

    SOLANIANS_ORB = 200208,
    SCOURGE_SCROLL_INTRO = 200210,
    SCOURGE_SCROLL_READ_1 = 200211,
    SCOURGE_SCROLL_READ_2 = 200212,
    SCOURGE_SCROLL_READ_3 = 200213,
    SCOURGE_SCROLL_READ_4 = 200214,
    DOTDOTDOT = 200209,
};
enum EversongDialogueOptions : uint16
{
    FELENDREN_LEAVE_DIALOGUE,
    FELENDREN_INTRO_DIALOGUE,
    FELENDREN_DIALOGUE_ASSAULTED,
    FELENDREN_DIALOGUE_1,
    FELENDREN_DIALOGUE_2,
    FELENDREN_DIALOGUE_3,
    FELENDREN_DIALOGUE_4,
    FELENDREN_DIALOGUE_5,
    FELENDREN_DIALOGUE_6,
    FELENDREN_DIALOGUE_PUT_DOWN,
    FELENDREN_DIALOGUE_PUT_DOWN_ATTACK,
    FELENDREN_DIALOGUE_EMBRACE,
    FELENDREN_DIALOGUE_EMBRACE_1,
    FELENDREN_DIALOGUE_EMBRACE_2,
    FELENDREN_DIALOGUE_EMBRACE_3,
    FELENDREN_DIALOGUE_7,
    FELENDREN_DIALOGUE_8,

    SOLANIANS_ORB_TAKE,
    SOLANIANS_ORB_SHATTER,
    SOLANIANS_ORB_STARE,
    SCOURGE_SCROLL_TAKE,
    SCOURGE_SCROLL_BURN,
    SCOURGE_SCROLL_READ_DIALOGUE_1,
    SCOURGE_SCROLL_READ_DIALOGUE_2,
    SCOURGE_SCROLL_READ_DIALOGUE_3,
    SCOURGE_SCROLL_READ_DIALOGUE_4,
    SOLANIANS_JOURNAL_TAKE,
    SOLANIANS_JOURNAL_SHATTER,
    SOLANIANS_JOURNAL_STARE,
};
 
class mana_wyrm : public ELKCreatureScript
{
public:
    mana_wyrm() : ELKCreatureScript("mana_wyrm") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mana_wyrmAI(creature);
    } 
    struct mana_wyrmAI : public ELKAI
    {
        uint8 dynamicMovement = 0;
        uint8 movementAmount = 0;
        uint8 maxMovement = 0;
        mana_wyrmAI(Creature* creature) : ELKAI(creature)
        {
            reinforcementCall = 1;
            chanceAtk = 3;
            chanceDef = 1;
            chanceSpell = 1;

            optionAtk = 5;
            optionDef = 1;
            optionSpell = 2;
            baseCooldowns[SpellsC::ARCANE_TORRENT] = 10000;
        };
        void EnterCombatCustom(Unit* /*who*/) override
        {
            events.ScheduleEvent(REGULAR_CHECK, 1000);
        }
        void MoveBack()
        {
            Position pos = me->GetPosition();
            float angle = me->GetVictim()->GetRelativeAngle(me);

            angle += (float(movementAmount) * 0.7 + 1.67) * (maxMovement % 2 == 0 ? 1 : -1);

            me->MovePositionToFirstCollision(pos, 5, angle);

            Movement::MoveSplineInit init(me);
            init.MoveTo(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
            init.Launch();

            movementAmount += 1;
        }
        void MoveSide()
        {

            Position pos = me->GetVictim()->GetPosition();
            float angle = me->GetVictim()->GetAngle(me);

            angle += (float(movementAmount) * 0.37 + 3.14) * (maxMovement % 2 == 0 ? 1 : -1);

            me->MovePositionToFirstCollision(pos, 4, angle);

            Movement::MoveSplineInit init(me);
            init.MoveTo(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
            init.Launch();
            me->SetFacingToObject(me->GetVictim());

            movementAmount += 1;
        }
        void MoveFront()
        {

            Position pos = me->GetVictim()->GetPosition();
            float angle = me->GetOrientation();

            angle += (float(movementAmount) * 0.12) * (maxMovement % 2 == 0 ? 1 : -1);

            me->MovePositionToFirstCollision(pos, 4, angle);

            Movement::MoveSplineInit init(me);
            init.MoveTo(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
            init.Launch();
            me->SetFacingToObject(me->GetVictim());

            movementAmount += 1;
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
                if (dynamicMovement > 0)
                {
                    switch (dynamicMovement)
                    {
                    case 1:
                    {
                        MoveBack();
                        if (movementAmount > maxMovement)
                        {
                            maxMovement = rand() % 9;
                            dynamicMovement = 2;
                            movementAmount = 0;
                        }
                        break;
                    }
                    case 2:
                    {
                        MoveSide();
                        if (movementAmount > maxMovement)
                        {
                            dynamicMovement = 0;
                            movementAmount = 0;
                            events.ScheduleEvent(ATK_1, 0);
                        }
                        break;
                    }
                    case 3:
                    {
                        MoveFront();
                        if (movementAmount > maxMovement)
                        {
                            dynamicMovement = 0;
                            movementAmount = 0;
                        }
                        break;
                    }
                    }
                    return;
                }
                else
                {
                    if (me->GetDistance(target) > 1 && me->GetDistance(target) <= 5 && rand() % 5 == 0)
                        EasyCastTarget(SpellsC::ARCANE_TORRENT);
                    return;
                }
            }
            else
                if (dynamicMovement)
                {
                    dynamicMovement = 0;
                    movementAmount = 0;
                }

            switch (events.ExecuteEvent())
            {
            case DYNAMIC_MOVEMENT_1:
            {
                dynamicMovement = 1;
                maxMovement = rand() % 9;
                MoveBack();

                return;
                break;
            }
            case ATK_1:
            {
                switch (comboing)
                {
                case 1:
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_1, 700);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_1, 700);
                    comboing = 3;
                    break;
                case 3:
                    DelayAttack();
                    if (rand() % 2 == 0)
                        EasyCast(ATTACK);
                    else
                    {
                        dynamicMovement = 2;
                        maxMovement = rand() % 9;
                    }
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

                    dynamicMovement = 3;
                    maxMovement = rand() % 9;
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
                    RandomAction();
                }
                break;
            default: 
                return;
            }
        }
        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            if (spell->Id == 28730)
                if (Player* player = caster->ToPlayer())
                {
                    player->RewardPlayerAndGroupAtEvent(15468, player);
                    torrented = true;
                }
        }
        void JustDied(Unit* /*killer*/) override
        {
            if (torrented)
            {
                LootStoreItem storeItem(LootStoreItem(20482, 0, 100, false, LOOT_MODE_DEFAULT, 0, 1, 5));
                me->loot.AddItem(storeItem);
            }
        }
        void RandomAtk(uint8 atk, bool& exit) override
        {
            switch (atk)
            {
            case 0:
            {
                if (me->GetAvailableRunes() >= 3)
                {
                    exit = true;
                    EasyQueCombo(ATK_1); 
                }
                break;
            }
            case 1:
            {
                if (me->GetAvailableRunes() >= 3)
                {
                    exit = true;
                    EasyQueCombo(ATK_2); 
                }
                break;
            }
            case 2:
            {
                if (me->GetAvailableRunes() >= 5 && me->GetCritTempo() > 100 && me->GetDistance(me->GetVictim()) < me->GetCombatReach())
                {
                    exit = true;
                    EasyQueCombo(ATK_3);
                }
                break;
            }
            case 3:
            {
                if (me->GetAvailableRunes() >= 5 && me->GetCritTempo() > 100 && me->GetDistance(me->GetVictim()) < me->GetCombatReach())
                {
                    exit = true;
                    EasyQueCombo(ATK_4);
                }
                break;
            }
            case 4:
            {
                events.ScheduleEvent(DYNAMIC_MOVEMENT_1, 0);
                break;
            }
            }
        }
        void RandomDef(uint8 def, bool& exit) override
        {
            if (me->GetAvailableRunes() && EasyCastTarget(SPELL_DEFLECT))
            { 
                exit = true;
            }
        }
        void RandomSpell(uint8 spell, bool& exit) override
        {
            if (EasyCastTarget(SpellsC::ARCANE_TORRENT))
            { 
                exit = true;
            }
        }
        bool torrented = false;
    };
};
class tainted_arcane_wraith : public ELKCreatureScript
{
public:
    tainted_arcane_wraith() : ELKCreatureScript("tainted_arcane_wraith") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new tainted_arcane_wraithAI(creature);
    }
    struct tainted_arcane_wraithAI : public ELKAI
    {
        tainted_arcane_wraithAI(Creature* creature) : ELKAI(creature)
        {
            reinforcementCall = 1;
            chanceAtk = 3;
            chanceDef = 3;
            chanceSpell = 3;

            optionAtk = 1;
            optionDef = 1;
            optionSpell = 1;
            baseCooldowns[SpellsC::SLOW] = 10000;
        };
        void EnterCombatCustom(Unit* /*who*/) override
        {
            events.ScheduleEvent(REGULAR_CHECK, 1000);
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
                if (me->GetDistance(target) > 1 && me->GetDistance(target) <= 5 && rand() % 5 == 0) 
                    EasyCastTarget(SpellsC::SLOW);
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
                    events.ScheduleEvent(ATK_1, 700);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_1, 700);
                    comboing = 3;
                    break;
                case 3:
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_1, 700);
                    comboing = 4;
                    break;
                case 4:
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
                    RandomAction();
                }
                break;
            default:
                return;
            }
        }
        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            if (spell->Id == 28730)
                if (Player* player = caster->ToPlayer())
                {
                    player->RewardPlayerAndGroupAtEvent(15468, player);
                    torrented = true;
                }
        }
        void JustDied(Unit* /*killer*/) override
        {
            if (torrented)
            {
                LootStoreItem storeItem(LootStoreItem(20482, 0, 100, false, LOOT_MODE_DEFAULT, 0, 1, 5));
                me->loot.AddItem(storeItem);
            }
        }
        void RandomAtk(uint8 atk, bool& exit) override
        {
            if (me->GetAvailableRunes() >= 5)
            {
                exit = true;
                EasyQueCombo(ATK_1);
            }
        }
        void RandomDef(uint8 def, bool& exit) override
        {
            if (me->GetAvailableRunes() && EasyCastTarget(SPELL_DEFLECT))
            {
                exit = true;
            }
        }
        void RandomSpell(uint8 spell, bool& exit) override
        {
            if (EasyCastTarget(SpellsC::SLOW))
            {
                exit = true;
            }
        }
        bool torrented = false;
    };
};

class wretched : public ELKCreatureScript
{
public:
    wretched() : ELKCreatureScript("wretched") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new wretchedAI(creature);
    }
    struct wretchedAI : public ELKAI
    {
        wretchedAI(Creature* creature) : ELKAI(creature)
        {
            reinforcementCall = RAND(1, 2);
            reinforcementEntries.push_back(15949);
            chanceAtk = 5;
            chanceDef = 2;
            chanceSpell = 1;
             
            optionAtk = 8;
            optionDef = 1;
            optionSpell = 1;

            baseCooldowns[SpellsC::MIND_FLAY] = 20000;
            baseCooldowns[SpellsC::ARCANE_BOLT] = 20000;
            baseCooldowns[SpellsC::ARCANE_TORRENT] = 20000;
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
                if (me->GetDistance(target) > 2 && me->GetDistance(target) <= 5 && rand() % 10 == 0)
                    me->CastSpell(target, SpellsC::ARCANE_TORRENT, false);
                else if (me->GetDistance(target) > 5)
                    me->CastSpell(target, SpellsC::ARCANE_BOLT, false);
                return;
            }
            switch (events.ExecuteEvent())
            {
            case CHECK_HEALTH:
                if (!comboing && me->HealthBelowPct(25))
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
                    events.ScheduleEvent(ATK_1, 700);
                    comboing = 2;
                    break;
                case 2:
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_1, 700);
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
                    events.ScheduleEvent(ATK_2, 700);
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
                if (rand() % 9 == 0 || me->GetAvailableRunes() == 0)
                {
                    break;
                }
                else
                {
                    DelayAttack();
                    EasyCast(ATTACK);
                    events.ScheduleEvent(ATK_1, 700);
                }
                break;
            }
            case REGULAR_CHECK:
                events.ScheduleEvent(REGULAR_CHECK, 1000);
                if (comboing)
                    break;
                if (me->GetDistance(target) < 3)
                {
                    RandomAction();
                }
                break;
            default: 
                return;
            }
        }
        void RandomAtk(uint8 atk, bool& exit) override
        { 
            if (mode == 0)
            {
                switch (atk)
                {
                case 0:
                case 1:
                case 2:
                    if(EasyCastTarget(SpellsC::THROW))
                    {
                        exit = true;
                    }
                    break;
                case 3:
                case 4:
                case 5:
                    if (EasyCastTarget(SpellsC::MIND_FLAY))
                    {
                        exit = true;
                    }
                    break;
                case 6:
                case 7:
                    if (EasyCastTarget(SpellsC::ARCANE_BOLT))
                    {
                        exit = true;
                    }
                    break;
                }
            }
            else
            {
                switch (atk)
                {
                case 0:
                case 1:
                case 2:
                {
                    if (me->GetAvailableRunes() >= 3)
                    {
                        exit = true;
                        EasyQueCombo(ATK_1);
                        break;
                    }
                    break;
                }
                case 3:
                case 4:
                case 5:
                {
                    if (me->GetAvailableRunes() >= 3)
                    {
                        exit = true;
                        EasyQueCombo(ATK_2);
                    }
                    break;
                }
                case 6:
                case 7:
                {
                    if (EasyCast(SpellsC::FRENZY))
                    {
                        exit = true;
                        EasyQueCombo(ATK_3);
                        mode = 0;
                    }
                    break;
                }
                }
            }
        }
        void RandomDef(uint8 def, bool& exit) override
        {
            if (me->GetAvailableRunes() && EasyCastTarget(SPELL_DEFLECT))
            { 
                exit = true;
            }
        }
        void RandomSpell(uint8 spell, bool& exit) override
        {
            if (EasyCastTarget(SpellsC::ARCANE_BOLT))
            { 
                exit = true;
            }
        }
    };
};

class felendren : public ELKCreatureScript
{
public:
    enum FelendrenResponses {
        INITIAL,
        a1,
        b1,
    };
    static inline std::map<FelendrenResponses, DialogueResponse> Responses = {};

    felendren() : ELKCreatureScript("felendren")
    {
    }
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new felendrenAI(creature);
    }
    struct felendrenAI : public ELKAI
    {
        felendrenAI(Creature* creature) : ELKAI(creature)
        {
            reinforcementCall = RAND(1, 2); 
            chanceAtk = 5;
            chanceDef = 3;
            chanceSpell = 3;

        }
        void EnterCombatCustom(Unit* /*who*/) override
        {
            std::cout << Responses[FelendrenResponses::a1].textIndex;
        }
        void UpdateAI(uint32 diff) override
        { 
            if (!UpdateVictim())
            {
                if (aggrod)
                {
                    aggrod = false;
                    surrendered = false;
                    me->SetFaction(1604);
                }
                return;
            }
            events.Update(diff);
        }
        void BuildGossip_TALK_7(Player*& player)
        {
            player->RemoveStageQuestFlag(QuestStageFlags::FELENDREN_PUT_DOWN);
            AddGossipItemFor(player, 0, "<Put Down> I've come to terms with it, I should kill you.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN);
            AddGossipItemFor(player, 0, "<Embrace> Right, are you sure you're not missing something? Feel anything?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE);
            AddGossipItemFor(player, 0, "<Leave> See you later.", GOSSIP_SENDER_MAIN, FELENDREN_LEAVE_DIALOGUE);
            SendGossipMenuFor(player, FELENDREN_TALK_7, me);
        }
        void BuildGossip_PUT_DOWN(Player*& player)
        {
            aggrod = true;
            surrendered = true;
            me->SetFaction(7);
            player->GossipEnd(me, 0);

            me->CombatStart(player);
            player->CombatStart(me);
            me->AddThreat(player, 1.0f);
            player->AddThreat(me, 1.0f);
            player->AddQuestStageFlag(QuestStageFlags::FELENDREN_PUT_DOWN);
        }
        virtual void sGossipHello(Player* player)  override
        {
            if (player->GetQuestStageFlag(QuestStageFlags::FELENDREN_PUT_DOWN))
            {
                AddGossipItemFor(player, 0, "<Attack> You're dead.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN_ATTACK);
                AddGossipItemFor(player, 0, "<Return> Never mind, I'm being hasty. What else can I do?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_PUT_DOWN_RESET, me); 
            }
            else if (!player->GetQuestStageFlag(QuestStageFlags::FELENDREN_MET))
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "What are you doing here you pale skinned, wretched rat?", GOSSIP_SENDER_MAIN, FELENDREN_INTRO_DIALOGUE);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<Attack> I'm going to shatter your ribs and throw you off this peak you disgrace.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_ASSAULTED);
                SendGossipMenuFor(player, FELENDREN_INTRO, me);
                player->AddQuestStageFlag(QuestStageFlags::FELENDREN_MET);
            }
            else if (!player->GetQuestStageFlag(QuestStageFlags::FELENDREN_TALKED_6))
            {
                BuildGossip_TALK_7(player);
            }
            else
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Why were you here again, bastard?", GOSSIP_SENDER_MAIN, FELENDREN_INTRO_DIALOGUE);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<Attack> I finished up my preparation, time to die..", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_ASSAULTED);
                SendGossipMenuFor(player, FELENDREN_INTRO_MET, me);
            }
        }
        void sGossipSelect(Player* player, uint32 sender, uint32 uiAction) override
        {
            ClearGossipMenuFor(player);
            switch (uiAction)
            {
            case FELENDREN_LEAVE_DIALOGUE:
            {
                player->GossipEnd(me, uiAction);
                break;
            }
            case FELENDREN_INTRO_DIALOGUE:
            {
                AddGossipItemFor(player, 0, "What happens to be your problem?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_1);
                AddGossipItemFor(player, 0, "Speak quickly or you'll be a skinned rat soon.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_1);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<Attack> Never mind, you're just another pathetic waste of Mana.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_ASSAULTED);
                SendGossipMenuFor(player, FELENDREN_TALK_1, me);
                break;
            }
            case FELENDREN_DIALOGUE_1:
            {
                AddGossipItemFor(player, 0, "Do you realize you've been here causing trouble for weeks?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_2);
                AddGossipItemFor(player, 0, "You spent a month trying to relieve yourself with magic like some sort of Fel-tainted Pig?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_2);
                SendGossipMenuFor(player, FELENDREN_TALK_2, me);
                break;
            }
            case FELENDREN_DIALOGUE_2:
            {
                AddGossipItemFor(player, 0, "Why and how did you release all of these Arcane Wraiths? They have cost us a few lives and a lot of time.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_3);
                AddGossipItemFor(player, 0, "As all wretched, you fail to face the consequences. Your elementals are wreaking havoc in this once tranquil school.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_3);
                SendGossipMenuFor(player, FELENDREN_TALK_3, me);
                break;
            }
            case FELENDREN_DIALOGUE_3:
            {
                AddGossipItemFor(player, 0, "It was you. This entire beautiful school has been abandoned since you started whatever it is you're doing.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_4);
                AddGossipItemFor(player, 0, "You've intervolved every inch of this place with your disgusting sorcerry.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_4);
                SendGossipMenuFor(player, FELENDREN_TALK_4, me);
                break;
            }
            case FELENDREN_DIALOGUE_4:
            {
                AddGossipItemFor(player, 0, "Lanthan sent me here to kill you to end the corruption of this honorable place.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_5);
                AddGossipItemFor(player, 0, "Lanthan was right, I should have beheaded you as soon as I got here.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_5);
                SendGossipMenuFor(player, FELENDREN_TALK_5, me);
                break;
            }
            case FELENDREN_DIALOGUE_5:
            {
                AddGossipItemFor(player, 0, "<Put Down> I believe it's best for us all if I ended your life right now.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN);
                AddGossipItemFor(player, 0, "<Embrace> Try touching the darkness, feel the burning and sharp energy.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE);
                AddGossipItemFor(player, 0, "What do you suggest I do then? I'm at a complete loss as to how to help you.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                AddGossipItemFor(player, 0, "Now what?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_TALK_6, me);
                break;
            }
            case FELENDREN_DIALOGUE_PUT_DOWN:
            {
                AddGossipItemFor(player, 0, "<Attack> I'm sorry Felendren.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN_ATTACK);
                AddGossipItemFor(player, 0, "<Attack> Your life should end, Felendren, you're a menace and a dangerous idiot.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN_ATTACK);
                AddGossipItemFor(player, 0, "<Attack> Burn in the Nether where your soul belongs, bastard.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN_ATTACK);
                AddGossipItemFor(player, 0, "<Return> Never mind, I ought to reconsider this. Sunwell damn you, what should I do?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_PUT_DOWN, me);
                break;
            }
            case FELENDREN_DIALOGUE_PUT_DOWN_ATTACK:
            {
                me->Say("If it has to be this way...", LANG_THALASSIAN);
                BuildGossip_PUT_DOWN(player);
                break;
            }
            case FELENDREN_DIALOGUE_ASSAULTED:
            {
                aggrod = true;
                me->Say("Maniac, and in the middle of my meditation too!", LANG_THALASSIAN);
                player->GossipEnd(me, uiAction);
                surrendered = false;
                me->SetFaction(7);

                me->CombatStart(player);
                player->CombatStart(me);
                me->AddThreat(player, 1.0f);
                player->AddThreat(me, 1.0f);
                break;
            }
            case FELENDREN_DIALOGUE_EMBRACE:
            {
                AddGossipItemFor(player, 0, "<Embrace> Some sort of corrosive, burning pulsation. Don't tell me you don't see it.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_1);
                AddGossipItemFor(player, 0, "<Embrace> You fool, even I can feel it.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_1); 
                AddGossipItemFor(player, 0, "<Return> Never mind this rubbish, what was I supposed to do again?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_EMBRACE_1, me);
                break;
            }
            case FELENDREN_DIALOGUE_EMBRACE_1:
            {
                AddGossipItemFor(player, 0, "<Embrace> If you absorb it, you might be able to resolve this ugly mess.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_2);
                AddGossipItemFor(player, 0, "<Embrace> Immedeatly drink this energy or I'll burn your remaining hairs off, runt.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_2);
                AddGossipItemFor(player, 0, "<Return> Screw this, what else can I do?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_EMBRACE_2, me);
                break;
            }
            case FELENDREN_DIALOGUE_EMBRACE_2:
            {
                AddGossipItemFor(player, 0, "<Choice, Embrace> It is worth it, Felendren. Whatever power or punishment waits for you beyond here is yours to embelish your honor with.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_3);
                AddGossipItemFor(player, 0, "<Choice, Embrace> Be done with it already.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_3);
                AddGossipItemFor(player, 0, "<Return> You're right, it's not worth the risk. What else?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_EMBRACE_3, me);
                break;
            }
            case FELENDREN_DIALOGUE_6:
            {
                BuildGossip_TALK_7(player);
                break;
            }
            } /*




              ADD DIALOGUE FOR IF YOU DON"T HAVE THE QUEST WHOOPS


              */
        }
        bool aggrod = false;
        bool surrendered = false;
    };

};

class solanians_orb : public ELKCreatureScript
{
public:
    solanians_orb() : ELKCreatureScript("solanians_orb")
    {
    }

    ELKAI* GetAI(Creature* creature) const override
    {
        return new solanians_orbAI(creature);
    }

    struct solanians_orbAI : public ELKAI
    {
        solanians_orbAI(Creature* creature) : ELKAI(creature)
        {
        }
        bool CanBeSeen(Player const* seer) override {
            return !(seer->GetQuestStageFlag(QuestStageFlags::SOLANIANS_SCRYING_ORB_FOUND));
        }
        virtual void sGossipHello(Player* player)  override
        {
            AddGossipItemFor(player, 0, "<Take Orb> I'm returning this.", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_TAKE);
            AddGossipItemFor(player, 0, "<Shatter Orb> I do need more power.", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_SHATTER);
            AddGossipItemFor(player, 0, "<Stare at your reflection in the orb>", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_STARE);
            SendGossipMenuFor(player, SOLANIANS_ORB, me);

        }
        void sGossipSelect(Player* player, uint32 sender, uint32 uiAction) override
        {
            ClearGossipMenuFor(player);
            switch (uiAction)
            {
            case SOLANIANS_ORB_TAKE:
                player->AddQuestStageFlag(QuestStageFlags::SOLANIANS_SCRYING_ORB_FOUND);
                player->GossipEnd(me, 0);
                player->UpdateVisibilityOf(me);
                break;
            case SOLANIANS_ORB_SHATTER:
                player->AddQuestStageFlag(QuestStageFlags::SOLANIANS_SCRYING_ORB_FOUND);
                player->AddQuestStageFlag(QuestStageFlags::SOLANIANS_SCRYING_ORB_CONSUMED);
                player->GiveXP(5000, player);
                player->GossipEnd(me, 0);
                player->UpdateVisibilityOf(me);
                break;
            case SOLANIANS_ORB_STARE:
                if (RAND(1, 5) < 5)
                {
                    AddGossipItemFor(player, 0, "<Take Orb> I'm returning this.", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_TAKE);
                    AddGossipItemFor(player, 0, "<Shatter Orb> I do need more power.", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_SHATTER);
                    AddGossipItemFor(player, 0, "<Stare at your reflection in the orb>", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_STARE);
                    SendGossipMenuFor(player, DOTDOTDOT, me);
                }
                else
                    sGossipSelect(player, sender, SOLANIANS_ORB_SHATTER);
                break;
            } 
        }
    };

};
 
class scourge_scroll : public ELKCreatureScript
{
public:
    scourge_scroll() : ELKCreatureScript("scourge_scroll") { }

    ELKAI* GetAI(Creature* creature) const override
    {
        return new scourge_scrollAI(creature);
    }

    struct scourge_scrollAI : public ELKAI
    {
        scourge_scrollAI(Creature* creature) : ELKAI(creature)
        {
        }
        void BuildGossipHello(Player*& player)
        {
            AddGossipItemFor(player, 0, "<Take Scroll of Scourge Magic> Know thy enemy.", GOSSIP_SENDER_MAIN, SCOURGE_SCROLL_TAKE);
            AddGossipItemFor(player, 0, "<Burn Scroll> Curse the Scourge.", GOSSIP_SENDER_MAIN, SCOURGE_SCROLL_BURN);
        }
        bool CanBeSeen(Player const* seer) override {
            return seer->GetQuestStageFlag(QuestStageFlags::SCOLL_OF_SCOURGE_MAGIC_FOUND);
        }
        void sGossipHello(Player* player) override
        {
            BuildGossipHello(player);
            AddGossipItemFor(player, 0, "<Read the Scroll>", GOSSIP_SENDER_MAIN, SCOURGE_SCROLL_READ_1);
            SendGossipMenuFor(player, SCOURGE_SCROLL_INTRO, me); 
        }
        void sGossipSelect(Player* player, uint32 sender, uint32 uiAction) override
        {
            ClearGossipMenuFor(player);
            switch (uiAction)
            {
            case SCOURGE_SCROLL_TAKE:
                player->AddQuestStageFlag(QuestStageFlags::SCOLL_OF_SCOURGE_MAGIC_FOUND);
                player->GossipEnd(me, 0);
                break;
            case SCOURGE_SCROLL_BURN:
                player->AddQuestStageFlag(QuestStageFlags::SCOLL_OF_SCOURGE_MAGIC_FOUND);
                player->AddQuestStageFlag(QuestStageFlags::SCOLL_OF_SCOURGE_MAGIC_CONSUMED);
                player->AddReputation(1602, 1000);
                player->GossipEnd(me, 0);
                break;
            case SCOURGE_SCROLL_READ_1:
                BuildGossipHello(player);
                AddGossipItemFor(player, 0, "<Read the Scroll>", GOSSIP_SENDER_MAIN, SCOURGE_SCROLL_READ_2);
                SendGossipMenuFor(player, SCOURGE_SCROLL_READ_1, me);
                break;
            case SCOURGE_SCROLL_READ_2:
                BuildGossipHello(player);
                AddGossipItemFor(player, 0, "<Read the Scroll>", GOSSIP_SENDER_MAIN, SCOURGE_SCROLL_READ_3);
                SendGossipMenuFor(player, SCOURGE_SCROLL_READ_2, me);
                break;
            case SCOURGE_SCROLL_READ_3:
                BuildGossipHello(player);
                AddGossipItemFor(player, 0, "<Read the Scroll>", GOSSIP_SENDER_MAIN, SCOURGE_SCROLL_READ_4);
                SendGossipMenuFor(player, SCOURGE_SCROLL_READ_3, me);
                break;
            case SCOURGE_SCROLL_READ_4:
                BuildGossipHello(player); 
                SendGossipMenuFor(player, SCOURGE_SCROLL_READ_4, me);
                break;
            } 
        }
    };

};

class amani_berserker : public ELKCreatureScript
{
public:
    amani_berserker() : ELKCreatureScript("amani_berserker") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new amani_berserkerAI(creature);
    }
    struct amani_berserkerAI : public ELKAI
    {
        uint8 dynamicMovement = 0;
        uint8 movementAmount = 0;
        bool threw = false;
        amani_berserkerAI(Creature* creature) : ELKAI(creature)
        {
            reinforcementCall = 0;
            chanceAtk = 5;
            chanceDef = 3;
            chanceSpell = 3;

            optionAtk = 3;
            optionDef = 2;
            optionSpell = 4;

            me->SetFloatValue(PLAYER_CRIT_PERCENTAGE, 100);
            me->ApplySpellPowerBonus(300, true);
        };
        void EnterCombatCustom(Unit* /*who*/) override
        {
            events.ScheduleEvent(REGULAR_CHECK, 300);
        }
        void MoveBack()
        {
            Position pos = me->GetPosition();
            float angle = me->GetVictim()->GetRelativeAngle(me);

            me->MovePositionToFirstCollision(pos, 3, angle + 3.14);

            Movement::MoveSplineInit init(me);


            init.MoveTo(pos.m_positionX, pos.m_positionY, pos.m_positionZ);

            init.Launch();


            movementAmount += 1;
        } 
        void ResetExtra() override
        {
            chanceDef = 1;

            lives = 10;
            spellHits = 0;
            damagedAmount = 0;
            ResetCC();
        }
        void ResetCC()
        {
            isLeaping = 0;
            threw = false;
            dynamicMovement = 0;
            movementAmount = 0;
        }
        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;
            events.Update(diff);
            if (!me->CanFreeMove())
            {
                events.Reset();
                ResetCC();
                events.ScheduleEvent(REGULAR_CHECK, 300);
                comboing = 0;
                return;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }
            if (me->isMoving() && !comboing)
            {
                if (isLeaping)
                    return;
                if (dynamicMovement > 0)
                {
                    switch (dynamicMovement)
                    {
                    case 1:
                    {
                        MoveBack();
                        if (movementAmount > 5)
                        {
                            me->RemoveAura(56354);
                            dynamicMovement = 0;
                            movementAmount = 0;
                        }
                        return;
                        break;
                    }
                    default:
                        me->RemoveAura(56354);
                        dynamicMovement = 0;
                        movementAmount = 0;
                        return;
                        break;
                    }
                }
                else
                {
                    if (threw)
                    {
                        threw = false;

                        Position pos = me->GetFirstCollisionPosition(me->GetDistance(me->GetVictim()), me->GetRelativeAngle(me->GetVictim()));

                        JumpTowards(pos, 1.f, 5.f, me->GetVictim());
                    }
                    else if (me->GetDistance(me->GetVictim()) > 3 && me->GetDistance(me->GetVictim()) <= 7 && rand() % 5 == 0 && EasyCastTarget(SpellsC::THROW))
                    {
                        threw = true;
                        
                        me->AddSpellCooldown(SpellsC::THROW, 0, 15000);
                    }
                    return;
                }
            }
            else
                if (dynamicMovement)
                {
                    me->RemoveAura(56354);
                    dynamicMovement = 0;
                    movementAmount = 0;
                }
            if (isLeaping)
                isLeaping = 0;
            if(threw)
                threw = false;

            if (!comboing)
                if (Aura* aura = me->GetAura(COMBO_COUNT); aura && aura->GetStackAmount() > 4)
                    me->CastSpell(me, CRUSHING_WAVE, false);

            DoEvents();
        }
        void DoEvents()
        {
            switch (events.ExecuteEvent())
            {
            case ATK_1:
            {
                switch (comboing)
                {
                case 1:
                    EasyAttack(ATTACK, ATK_1, 700);
                    break;
                case 2:
                    EasyAttack(SPIN_ATTACK, ATK_1, 300);
                    break;
                case 3:
                    EasyAttack(ATTACK, ATK_1, 1000);
                    break;
                case 4:
                    EasyAttack(ATTACK, ATK_1, 700);
                    break;
                case 5:
                    EasyAttack(SPIN_ATTACK, ATK_1, 300);
                    break;
                case 6:
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
                    EasyAttack(CRITICAL_ATTACK, ATK_2, 700);
                    break;
                case 2:
                    EasyAttack(ATTACK, ATK_2, 1000);
                    break;
                case 3:
                    EasyAttack(CRITICAL_ATTACK, ATK_2, 700);
                    break;
                case 4:
                    EasyAttack(ATTACK, ATK_2, 1000);
                    break;
                case 5:
                    EasyCast(CRITICAL_ATTACK);
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
                    EasyAttack(CRITICAL_ATTACK, ATK_3, 700);
                    break;
                case 2:
                    EasyAttack(SPIN_ATTACK, ATK_3, 300);
                    break;
                case 3:
                    EasyAttack(SPIN_ATTACK, ATK_3, 300);
                    break;
                case 4:
                    EasyAttack(SPIN_ATTACK, ATK_3, 300);
                    break;
                case 5:
                    EasyCast(SPIN_ATTACK);
                    comboing = 0;
                    break;
                }
                break;
            }
            case ATK_4:
            {
                switch (comboing)
                {
                case 1:
                    EasyAttack(CRITICAL_ATTACK, ATK_4, 700);
                    break;
                case 2:
                    EasyAttack(SPIN_ATTACK, ATK_4, 300);
                    break;
                case 3:
                    EasyAttack(SPIN_ATTACK, ATK_4, 700);
                    break;
                case 4:
                    EasyAttack(SPIN_ATTACK, ATK_4, 300);
                    break;
                case 5:
                    EasyCast(SPIN_ATTACK);
                    comboing = 0;
                    break;
                }
                break;
            }
            case SPL_1:
            {
                switch (comboing)
                {
                case 1:
                    if (EasyCastTarget(SpellsC::THROW))
                    {
                        events.ScheduleEvent(SPL_1, 200);
                        comboing++;
                    }
                    else
                        comboing = 0;
                    break;
                case 2:
                    EasyCastTarget(SpellsC::THROW);
                    comboing = 0;
                    break;
                }
            }
            case REGULAR_CHECK:
                events.ScheduleEvent(REGULAR_CHECK, 300);
                if (comboing)
                {
                    DoEvents();
                    break;
                }
                DoEvents();
                if (me->GetDistance(me->GetVictim()) < 3)
                {
                    RandomAction();
                }
                break;
            default:
                return;
            }
        }
        uint32 spellHits = 0;
        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            if (caster == me)
                return;
            spellHits++;
            if (rand() % spellHits > 10)
            {
                me->CastSpell(me, 150032, true);
                spellHits = 0;
            }
        }
        uint8 lives = 10;
        uint16 damagedAmount = 0;
        void DamageTaken(Unit* doneby, uint32& damage, DamageEffectType, SpellSchoolMask) override
        {
            if (me->IsAlive() == false)
                return;
            if (damage >= me->GetHealth() && lives > rand() % 10)
            {
                isLeaping = 0;
                damage = 0;
                me->SetHealth(300);
                lives--;
                chanceDef = 0;
            }
            if (me->HasAura(150033))
                return;
            damagedAmount += damage;
            if (damagedAmount > 30)
                damagedAmount -= 30;
            else
                damagedAmount = 0;
            if (damagedAmount > 300)
            {
                damagedAmount = 0;
                doneby->CastSpell(me, 150033, true);
            }

        }
        void RandomAtk(uint8 atk, bool& exit) override
        {
            switch (atk)
            {
            case 0:
            {
                if (me->GetAvailableRunes() >= 6 && me->GetDistance(me->GetVictim()) < me->GetCombatReach())
                {
                    exit = true;
                    EasyQueCombo(ATK_1);
                }
                break;
            }
            case 1:
            {
                if (me->GetAvailableRunes() >= 5 && me->GetCritTempo() > 300 && me->GetDistance(me->GetVictim()) < me->GetCombatReach())
                {
                    exit = true;
                    EasyQueCombo(ATK_2);
                }
                break;
            }
            case 2:
            {
                if (me->GetAvailableRunes() >= 5 && me->GetCritTempo() > 100 && me->GetDistance(me->GetVictim()) < me->GetCombatReach())
                {
                    exit = true;
                    EasyQueCombo(ATK_3);
                }
                break;
            }
            case 3:
            {
                if (me->GetAvailableRunes() >= 5 && me->GetCritTempo() > 100 && me->GetDistance(me->GetVictim()) < me->GetCombatReach())
                {
                    exit = true;
                    EasyQueCombo(ATK_4);
                }
                break;
            }
            case 4:
            {
                exit = true;
                me->AddAura(56354, me);
                dynamicMovement = 1;
                MoveBack();
                break;
            }
            }
        }
        void RandomDef(uint8 def, bool& exit) override
        {
            if (me->GetAvailableRunes() && EasyCastTarget(SPELL_DEFLECT))
            {
                exit = true;
            }
        }
        void RandomSpell(uint8 spell, bool& exit) override
        {
            switch (spell)
            {
            case 0:
            {
                if (EasyCastTarget(SpellsC::THROW))
                {
                    events.ScheduleEvent(SPL_1, 200);
                    comboing = 1;
                    exit = true; 
                    break;
                }
            }
            case 1:
            {
                isLeaping = 1;

                Position pos = me->GetFirstCollisionPosition(5, 3.14);

                JumpTowards(pos, 1.f, 5.f, me->GetVictim());

                exit = true;
                break;
            }
            case 2:
            {
                if (me->GetDistance(me->GetVictim()) < 3)
                    EasyCast(SpellsC::GIGA_THUNDER_CLAP);
            }
            }
        }
        uint8 isLeaping = 0;
        void sOnMutate() override
        {
            if (!me->GetVictim() || !me->CanFreeMove())
            {
                if (isLeaping)
                    isLeaping = 0;
                return;
            }
            switch (isLeaping)
            {
            case 1:
                if (rand() % 2 == 0)
                {
                    Position pos = me->GetFirstCollisionPosition(2, me->GetVictim()->GetRelativeAngle(me) + 1.67);

                    JumpTowards(pos, 1.f, 5.f, me->GetVictim());

                    isLeaping++;

                }
                else
                    isLeaping = 0;
                break;
            case 2:
                isLeaping = 0;

                Position pos = me->GetFirstCollisionPosition(me->GetDistance(me->GetVictim()), me->GetRelativeAngle(me->GetVictim()));

                JumpTowards(pos, 1.f, 8.f, me->GetVictim());

                break;
            }
        }
        void JumpTowards(const Position& pos, float speedXYmult, float speedZ, Unit* unit = nullptr)
        {
            if (!Acore::IsValidMapCoord(pos.m_positionX, pos.m_positionY, pos.m_positionZ) || pos.m_positionZ <= INVALID_HEIGHT)
                return;

            float dist = me->GetExactDist2d(pos.m_positionX, pos.m_positionY);

            float speedXY = (dist * 10.0f / speedZ) * speedXYmult;

            if (speedXY < 1.0f)
                speedXY = 1.0f;

            me->GetMotionMaster()->MoveJumpOriented(pos.m_positionX, pos.m_positionY, pos.m_positionZ, speedXY, speedZ, me->GetVictim());
        }
    };
};


class amani_shadowpriest : public ELKCreatureScript
{
public:
    amani_shadowpriest() : ELKCreatureScript("amani_shadowpriest") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new amani_shadowpriestAI(creature);
    }
    struct amani_shadowpriestAI : public ELKAI
    {
        uint8 dynamicMovement = 0;
        uint8 movementAmount = 0;
        bool threw = false;
        amani_shadowpriestAI(Creature* creature) : ELKAI(creature)
        {
            reinforcementCall = 0;
            chanceAtk = 5;
            chanceDef = 3;
            chanceSpell = 3;

            optionAtk = 3;
            optionDef = 2;
            optionSpell = 4;

            me->SetFloatValue(PLAYER_CRIT_PERCENTAGE, 100);
            me->ApplySpellPowerBonus(300, true);
        };
        void EnterCombatCustom(Unit* /*who*/) override
        {
            events.ScheduleEvent(REGULAR_CHECK, 300);
        }
        void MoveBack()
        {
            Position pos = me->GetPosition();
            float angle = me->GetVictim()->GetRelativeAngle(me);

            me->MovePositionToFirstCollision(pos, 3, angle + 3.14);

            Movement::MoveSplineInit init(me);


            init.MoveTo(pos.m_positionX, pos.m_positionY, pos.m_positionZ);

            init.Launch();


            movementAmount += 1;
        }
        void ResetExtra() override
        {
            chanceDef = 1;

            lives = 10;
            spellHits = 0;
            damagedAmount = 0;
            ResetCC();
        }
        void ResetCC()
        {
            isLeaping = 0;
            threw = false;
            dynamicMovement = 0;
            movementAmount = 0;
        }
        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;
            events.Update(diff);
            if (!me->CanFreeMove())
            {
                events.Reset();
                ResetCC();
                events.ScheduleEvent(REGULAR_CHECK, 300);
                comboing = 0;
                return;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                return;
            }
            if (me->isMoving() && !comboing)
            {
                if (isLeaping)
                    return;
                if (dynamicMovement > 0)
                {
                    switch (dynamicMovement)
                    {
                    case 1:
                    {
                        MoveBack();
                        if (movementAmount > 5)
                        {
                            me->RemoveAura(56354);
                            dynamicMovement = 0;
                            movementAmount = 0;
                        }
                        return;
                        break;
                    }
                    default:
                        me->RemoveAura(56354);
                        dynamicMovement = 0;
                        movementAmount = 0;
                        return;
                        break;
                    }
                }
                else
                {
                    if (threw)
                    {
                        threw = false;

                        Position pos = me->GetFirstCollisionPosition(me->GetDistance(me->GetVictim()), me->GetRelativeAngle(me->GetVictim()));

                        JumpTowards(pos, 1.f, 5.f, me->GetVictim());
                    }
                    else if (me->GetDistance(me->GetVictim()) > 3 && me->GetDistance(me->GetVictim()) <= 7 && rand() % 5 == 0 && EasyCastTarget(SpellsC::THROW))
                    {
                        threw = true;

                        me->AddSpellCooldown(SpellsC::THROW, 0, 15000);
                    }
                    return;
                }
            }
            else
                if (dynamicMovement)
                {
                    me->RemoveAura(56354);
                    dynamicMovement = 0;
                    movementAmount = 0;
                }
            if (isLeaping)
                isLeaping = 0;
            if (threw)
                threw = false;

            if (!comboing)
                if (Aura* aura = me->GetAura(COMBO_COUNT); aura && aura->GetStackAmount() > 4)
                    me->CastSpell(me, CRUSHING_WAVE, false);

            DoEvents();
        }
        void DoEvents()
        {
            switch (events.ExecuteEvent())
            {
            case ATK_1:
            {
                switch (comboing)
                {
                case 1:
                    EasyAttack(ATTACK, ATK_1, 700);
                    break;
                case 2:
                    EasyAttack(SPIN_ATTACK, ATK_1, 300);
                    break;
                case 3:
                    EasyAttack(ATTACK, ATK_1, 1000);
                    break;
                case 4:
                    EasyAttack(ATTACK, ATK_1, 700);
                    break;
                case 5:
                    EasyAttack(SPIN_ATTACK, ATK_1, 300);
                    break;
                case 6:
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
                    EasyAttack(CRITICAL_ATTACK, ATK_2, 700);
                    break;
                case 2:
                    EasyAttack(ATTACK, ATK_2, 1000);
                    break;
                case 3:
                    EasyAttack(CRITICAL_ATTACK, ATK_2, 700);
                    break;
                case 4:
                    EasyAttack(ATTACK, ATK_2, 1000);
                    break;
                case 5:
                    EasyCast(CRITICAL_ATTACK);
                    comboing = 0;
                    break;
                }
                break;
            }
            case SPL_1:
            {
                switch (comboing)
                {
                case 1:
                    if (EasyCastTarget(SpellsC::THROW))
                    {
                        events.ScheduleEvent(SPL_1, 200);
                        comboing++;
                    }
                    else
                        comboing = 0;
                    break;
                case 2:
                    EasyCastTarget(SpellsC::THROW);
                    comboing = 0;
                    break;
                }
            }
            case REGULAR_CHECK:
                events.ScheduleEvent(REGULAR_CHECK, 300);
                if (comboing)
                {
                    DoEvents();
                    break;
                }
                DoEvents();
                if (me->GetDistance(me->GetVictim()) < 3)
                {
                    RandomAction();
                }
                break;
            default:
                return;
            }
        }
        uint32 spellHits = 0;
        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            if (caster == me)
                return;
            spellHits++;
            if (rand() % spellHits > 10)
            {
                me->CastSpell(me, 150032, true);
                spellHits = 0;
            }
        }
        uint8 lives = 10;
        uint16 damagedAmount = 0;
        void DamageTaken(Unit* doneby, uint32& damage, DamageEffectType, SpellSchoolMask) override
        {
            if (damage >= me->GetHealth() && lives > rand() % 10)
            {
                isLeaping = 0;
                damage = 0;
                me->SetHealth(300);
                lives--;
                chanceDef = 0;
            }

            if (me->HasAura(150033))
                return;
            damagedAmount += damage;
            if (damagedAmount > 30)
                damagedAmount -= 30;
            else
                damagedAmount = 0;
            if (damagedAmount > 300)
            {
                damagedAmount = 0;
                doneby->CastSpell(me, 150033, true);
            }

        }
        void RandomAtk(uint8 atk, bool& exit) override
        {
            switch (atk)
            {
            case 0:
            {
                if (me->GetAvailableRunes() >= 6 && me->GetDistance(me->GetVictim()) < me->GetCombatReach())
                {
                    exit = true;
                    EasyQueCombo(ATK_1);
                }
                break;
            }
            case 1:
            {
                if (me->GetAvailableRunes() >= 5 && me->GetCritTempo() > 300 && me->GetDistance(me->GetVictim()) < me->GetCombatReach())
                {
                    exit = true;
                    EasyQueCombo(ATK_2);
                }
                break;
            }
            case 2:
            {
                if (me->GetAvailableRunes() >= 5 && me->GetCritTempo() > 100 && me->GetDistance(me->GetVictim()) < me->GetCombatReach())
                {
                    exit = true;
                    EasyQueCombo(ATK_3);
                }
                break;
            }
            case 3:
            {
                if (me->GetAvailableRunes() >= 5 && me->GetCritTempo() > 100 && me->GetDistance(me->GetVictim()) < me->GetCombatReach())
                {
                    exit = true;
                    EasyQueCombo(ATK_4);
                }
                break;
            }
            case 4:
            {
                exit = true;
                me->AddAura(56354, me);
                dynamicMovement = 1;
                MoveBack();
                break;
            }
            }
        }
        void RandomDef(uint8 def, bool& exit) override
        {
            if (me->GetAvailableRunes() && EasyCastTarget(SPELL_DEFLECT))
            {
                exit = true;
            }
        }
        void RandomSpell(uint8 spell, bool& exit) override
        {
            switch (spell)
            {
            case 0:
            {
                if (EasyCastTarget(SpellsC::THROW))
                {
                    events.ScheduleEvent(SPL_1, 200);
                    comboing = 1;
                    exit = true;
                    break;
                }
            }
            case 1:
            {
                isLeaping = 1;

                Position pos = me->GetFirstCollisionPosition(5, 3.14);

                JumpTowards(pos, 1.f, 5.f, me->GetVictim());

                exit = true;
                break;
            }
            case 2:
            {
                if (me->GetDistance(me->GetVictim()) < 3)
                    EasyCast(SpellsC::GIGA_THUNDER_CLAP);
            }
            }
        }
        uint8 isLeaping = 0;
        void sOnMutate() override
        {
            if (!me->GetVictim() || !me->CanFreeMove())
            {
                if (isLeaping)
                    isLeaping = 0;
                return;
            }
            switch (isLeaping)
            {
            case 1:
                if (rand() % 2 == 0)
                {
                    Position pos = me->GetFirstCollisionPosition(2, me->GetVictim()->GetRelativeAngle(me) + 1.67);

                    JumpTowards(pos, 1.f, 5.f, me->GetVictim());

                    isLeaping++;

                }
                else
                    isLeaping = 0;
                break;
            case 2:
                isLeaping = 0;

                Position pos = me->GetFirstCollisionPosition(me->GetDistance(me->GetVictim()), me->GetRelativeAngle(me->GetVictim()));

                JumpTowards(pos, 1.f, 8.f, me->GetVictim());

                break;
            }
        }
        void JumpTowards(const Position& pos, float speedXYmult, float speedZ, Unit* unit = nullptr)
        {
            if (!Acore::IsValidMapCoord(pos.m_positionX, pos.m_positionY, pos.m_positionZ) || pos.m_positionZ <= INVALID_HEIGHT)
                return;

            float dist = me->GetExactDist2d(pos.m_positionX, pos.m_positionY);

            float speedXY = (dist * 10.0f / speedZ) * speedXYmult;

            if (speedXY < 1.0f)
                speedXY = 1.0f;

            me->GetMotionMaster()->MoveJumpOriented(pos.m_positionX, pos.m_positionY, pos.m_positionZ, speedXY, speedZ, me->GetVictim());
        }
    };
};

void AddSC_elk_eversong_woods_mobs()
{
    new mana_wyrm();
    new wretched();
    new tainted_arcane_wraith();
    new felendren();

    new solanians_orb();
    new scourge_scroll();

    new amani_shadowpriest();
    new amani_berserker();
    /*
    new amani_axe_thrower(); // jumps and throws axes in 3s. hes an imp basically. doesnt even deflect, dies fast
    new spearcrafter_otembe(); // gets +10 deflect chance after being hit every few attacks, lots of dmg, throws spears and bladestorms in place, after using lives system starts summoning serpent wards

    new shadowpine_ripper(); // small fodder guy that just attacks then equips a weapon, either a sword, or axes and becomes like berserker. if he kills you he casts cannibalize on you.
    new shadowpine_witch(); // summons spiders, zombie mummies, revives allies: KEY, drains mana, uses lightning that reduces armor, shrinks you. if you get to low HP he just casts finger of death for 100% his curr mana and laughs.
    new shadowpine_oracle(); // spirit links allies splitting damage, restores their mana to full after a 10s cast, gives shields that blow up after 5s and thunderclap, stuns them but makes them immune while channeling if and only if theres multiple enemies (which there always is). if completed, heals to full.
    new shadowpine_headhunter(); // replacement for axe thrower, throws spears at a location you are constantly forcing you to move in 5s.
    new shadowpine_shadowcaster(); // better shadow priest, the same but more spells.
    new shadowpine_catlord(); // big; leaves flamestrikes for 30s, jumps a lot, summons cat when hit a lot while deflecting. deflects when not doing anything. will get away to cast a 5s cast time regen spell that if interrupted disarms and silences them for 10s.
    new shadowpine_hexxer(); // berserk AI mixing in all of the spells previously used besides catlord. also shrinks you
    new kelgash(); //

    new amanshi_berserker() // amani berserkers have a 3% chance to turn into one of these on death.
    new amanshi_warbringer() // 2 of them outside the instance somewhere
    // replace savages, lookouts and scouts with all the custom guys I've made. make elder lynxes wander outside
    */
}





