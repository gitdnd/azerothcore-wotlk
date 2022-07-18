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
    SCOURGE_SCROLL_READ_1,
    SCOURGE_SCROLL_READ_2,
    SCOURGE_SCROLL_READ_3,
    SCOURGE_SCROLL_READ_4,
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
        mana_wyrmAI(Creature* creature) : ELKAI(creature)
        {
            reinforcementCall = 1;
            chanceAtk = 3;
            chanceDef = 1;
            chanceSpell = 1;

            optionAtk = 2;
            optionDef = 1;
            optionSpell = 1;
            baseCooldowns[SpellsC::ARCANE_TORRENT] = 10000;
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
                if (me->GetDistance(target) > 1 and me->GetDistance(target) <= 5 and rand() % 5 == 0)
                    EasyCastTarget(SpellsC::ARCANE_TORRENT);
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
                    if (rand() % 2 == 0)
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
                if (me->GetDistance(target) > 1 and me->GetDistance(target) <= 5 and rand() % 5 == 0) 
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
                    events.ScheduleEvent(ATK_1, 750);
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
                if (me->GetDistance(target) > 2 and me->GetDistance(target) <= 5 and rand() % 10 == 0)
                    me->CastSpell(target, SpellsC::ARCANE_TORRENT, false);
                else if (me->GetDistance(target) > 5)
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
                if (rand() % 9 == 0 || me->GetAvailableRunes() == 0)
                {
                    break;
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
    felendren() : ELKCreatureScript("felendren") {}
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
            AddGossipItemFor(player, 0, "<Talk, Put Down> I've come to terms with it, I should kill you.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN);
            AddGossipItemFor(player, 0, "<Talk, Embrace> Right, are you sure you're not missing something? Feel anything?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE);
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
            player->AddStageQuestFlag(QuestStageFlags::FELENDREN_PUT_DOWN);
        }
        virtual void sGossipHello(Player* player)  override
        {
            if (player->GetStageQuestFlag(QuestStageFlags::FELENDREN_PUT_DOWN))
            {
                AddGossipItemFor(player, 0, "<Attack> You're dead.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN_ATTACK);
                AddGossipItemFor(player, 0, "<Talk, Return> Never mind, I'm being hasty. What else can I do?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_PUT_DOWN_RESET, me); 
            }
            else if (!player->GetStageQuestFlag(QuestStageFlags::FELENDREN_MET))
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<Talk> What are you doing here you pale skinned, wretched rat?", GOSSIP_SENDER_MAIN, FELENDREN_INTRO_DIALOGUE);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<Attack> I'm going to shatter your ribs and throw you off this peak you disgrace.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_ASSAULTED);
                SendGossipMenuFor(player, FELENDREN_INTRO, me);
                player->AddStageQuestFlag(QuestStageFlags::FELENDREN_MET);
            }
            else if (!player->GetStageQuestFlag(QuestStageFlags::FELENDREN_TALKED_6))
            {
                BuildGossip_TALK_7(player);
            }
            else
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<Talk> Why were you here again, bastard?", GOSSIP_SENDER_MAIN, FELENDREN_INTRO_DIALOGUE);
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
                AddGossipItemFor(player, 0, "<Talk> What happens to be your problem?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_1);
                AddGossipItemFor(player, 0, "<Talk> Speak quickly or you'll be a skinned rat soon.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_1);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<Attack> Never mind, you're just another pathetic waste of Mana.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_ASSAULTED);
                SendGossipMenuFor(player, FELENDREN_TALK_1, me);
                break;
            }
            case FELENDREN_DIALOGUE_1:
            {
                AddGossipItemFor(player, 0, "<Talk> Do you realize you've been here causing trouble for weeks?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_2);
                AddGossipItemFor(player, 0, "<Talk> You spent a month trying to relieve yourself with magic like some sort of Fel-tainted Pig?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_2);
                SendGossipMenuFor(player, FELENDREN_TALK_2, me);
                break;
            }
            case FELENDREN_DIALOGUE_2:
            {
                AddGossipItemFor(player, 0, "<Talk> Why and how did you release all of these Arcane Wraiths? They have cost us a few lives and a lot of time.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_3);
                AddGossipItemFor(player, 0, "<Talk> As all wretched, you fail to face the consequences. Your elementals are wreaking havoc in this once tranquil school.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_3);
                SendGossipMenuFor(player, FELENDREN_TALK_3, me);
                break;
            }
            case FELENDREN_DIALOGUE_3:
            {
                AddGossipItemFor(player, 0, "<Talk> It was you. This entire beautiful school has been abandoned since you started whatever it is you're doing.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_4);
                AddGossipItemFor(player, 0, "<Talk> You've intervolved every inch of this place with your disgusting sorcerry.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_4);
                SendGossipMenuFor(player, FELENDREN_TALK_4, me);
                break;
            }
            case FELENDREN_DIALOGUE_4:
            {
                AddGossipItemFor(player, 0, "<Talk> Lanthan sent me here to kill you to end the corruption of this honorable place.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_5);
                AddGossipItemFor(player, 0, "<Talk> Lanthan was right, I should have beheaded you as soon as I got here.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_5);
                SendGossipMenuFor(player, FELENDREN_TALK_5, me);
                break;
            }
            case FELENDREN_DIALOGUE_5:
            {
                AddGossipItemFor(player, 0, "<Talk, Put Down> I believe it's best for us all if I ended your life right now.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN);
                AddGossipItemFor(player, 0, "<Talk, Embrace> Try touching the darkness, feel the burning and sharp energy.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE);
                AddGossipItemFor(player, 0, "<Talk> What do you suggest I do then? I'm at a complete loss as to how to help you.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                AddGossipItemFor(player, 0, "<Talk> Now what?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_TALK_6, me);
                break;
            }
            case FELENDREN_DIALOGUE_PUT_DOWN:
            {
                AddGossipItemFor(player, 0, "<Attack> I'm sorry Felendren.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN_ATTACK);
                AddGossipItemFor(player, 0, "<Attack> Your life should end, Felendren, you're a menace and a dangerous idiot.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN_ATTACK);
                AddGossipItemFor(player, 0, "<Attack> Burn in the Nether where your soul belongs, bastard.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN_ATTACK);
                AddGossipItemFor(player, 0, "<Talk, Return> Never mind, I ought to reconsider this. Sunwell damn you, what should I do?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
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
                AddGossipItemFor(player, 0, "<Talk, Embrace> Some sort of corrosive, burning pulsation. Don't tell me you don't see it.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_1);
                AddGossipItemFor(player, 0, "<Talk, Embrace> You fool, even I can feel it.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_1); 
                AddGossipItemFor(player, 0, "<Talk, Return> Never mind this rubbish, what was I supposed to do again?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_EMBRACE_1, me);
                break;
            }
            case FELENDREN_DIALOGUE_EMBRACE_1:
            {
                AddGossipItemFor(player, 0, "<Talk, Embrace> If you absorb it, you might be able to resolve this ugly mess.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_2);
                AddGossipItemFor(player, 0, "<Talk, Embrace> Immedeatly drink this energy or I'll burn your remaining hairs off, runt.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_2);
                AddGossipItemFor(player, 0, "<Talk, Return> Screw this, what else can I do?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_EMBRACE_2, me);
                break;
            }
            case FELENDREN_DIALOGUE_EMBRACE_2:
            {
                AddGossipItemFor(player, 0, "<Choice, Embrace> It is worth it, Felendren. Whatever power or punishment waits for you beyond here is yours to embelish your honor with.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_3);
                AddGossipItemFor(player, 0, "<Choice, Embrace> Be done with it already.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_EMBRACE_3);
                AddGossipItemFor(player, 0, "<Talk, Return> You're right, it's not worth the risk. What else?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
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

class solanians_orb : public GameObjectScript
{
public:
    solanians_orb() : GameObjectScript("solanians_orb") { }

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new solanians_orbAI(go);
    }

    struct solanians_orbAI : public GameObjectAI
    {
        solanians_orbAI(GameObject* gameObject) : GameObjectAI(gameObject)
        { 
        }
        bool GossipHello(Player* player, bool  /*reportUse*/) override
        {
            AddGossipItemFor(player, 0, "<Take Orb> I'm returning this.", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_TAKE);
            AddGossipItemFor(player, 0, "<Shatter Orb> I do need more power.", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_SHATTER);
            AddGossipItemFor(player, 0, "<Stare at your reflection in the orb>", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_STARE);
            SendGossipMenuFor(player, SOLANIANS_ORB, me->GetGUID());
            return false;
        }
        bool GossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            ClearGossipMenuFor(player);
            switch (action)
            {
            case SOLANIANS_ORB_TAKE:
                player->AddStageQuestFlag(QuestStageFlags::SOLANIANS_SCRYING_ORB_FOUND);
                player->GossipEnd(me, 0);
                break;
            case SOLANIANS_ORB_SHATTER:
                player->AddStageQuestFlag(QuestStageFlags::SOLANIANS_SCRYING_ORB_CONSUMED);
                player->GiveXP(5000, player);
                player->GossipEnd(me, 0);
                break;
            case SOLANIANS_ORB_STARE:
                if (RAND(1, 5) < 5)
                {
                    AddGossipItemFor(player, 0, "<Take Orb> I'm returning this.", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_TAKE);
                    AddGossipItemFor(player, 0, "<Shatter Orb> I do need more power.", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_SHATTER);
                    AddGossipItemFor(player, 0, "<Stare at your reflection in the orb>", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_STARE);
                    SendGossipMenuFor(player, DOTDOTDOT, me->GetGUID());
                }
                else
                    GossipSelect(player, sender, SOLANIANS_ORB_SHATTER);
                break;
            }
            return false;
        }
    };

};

class scourge_scroll : public GameObjectScript
{
public:
    scourge_scroll() : GameObjectScript("scourge_scroll") { }

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new scourge_scrollAI(go);
    }

    struct scourge_scrollAI : public GameObjectAI
    {
        scourge_scrollAI(GameObject* gameObject) : GameObjectAI(gameObject)
        {
        }
        void BuildGossipHello(Player*& player)
        {
            AddGossipItemFor(player, 0, "<Take Scroll of Scourge Magic> Know thy enemy.", GOSSIP_SENDER_MAIN, SCOURGE_SCROLL_TAKE);
            AddGossipItemFor(player, 0, "<Burn Scroll> Curse the Scourge.", GOSSIP_SENDER_MAIN, SCOURGE_SCROLL_BURN);
        }
        bool GossipHello(Player* player, bool  /*reportUse*/) override
        {
            BuildGossipHello(player);
            AddGossipItemFor(player, 0, "<Read the Scroll>", GOSSIP_SENDER_MAIN, SCOURGE_SCROLL_READ_1);
            SendGossipMenuFor(player, SCOURGE_SCROLL_INTRO, me->GetGUID());
            return false;
        }
        bool GossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            ClearGossipMenuFor(player);
            switch (action)
            {
            case SOLANIANS_ORB_TAKE:
                player->AddStageQuestFlag(QuestStageFlags::SCOLL_OF_SCOURGE_MAGIC_FOUND,
                    );
                player->GossipEnd(me, 0);
                break;
            case SOLANIANS_ORB_SHATTER:
                player->AddStageQuestFlag(QuestStageFlags::SCOLL_OF_SCOURGE_MAGIC_CONSUMED);
                player->GiveXP(5000, player);
                player->GossipEnd(me, 0);
                break;
            case SOLANIANS_ORB_STARE:
                if (RAND(1, 5) < 5)
                {
                    AddGossipItemFor(player, 0, "<Take Orb> I'm returning this.", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_TAKE);
                    AddGossipItemFor(player, 0, "<Shatter Orb> I do need more power.", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_SHATTER);
                    AddGossipItemFor(player, 0, "<Stare at your reflection in the orb>", GOSSIP_SENDER_MAIN, SOLANIANS_ORB_STARE);
                    SendGossipMenuFor(player, DOTDOTDOT, me->GetGUID());
                }
                else
                    GossipSelect(player, sender, SOLANIANS_ORB_SHATTER);
                break;
            }
            return false;
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
}
