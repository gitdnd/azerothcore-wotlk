#pragma once 


#include <sstream>
#include <utility>


#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Unit/Unit.h"
#include "ScriptedGossip.h"
#include "GameObjectAI.h"

#include "../Quest/QuestStageFlags.h"
#include "../Spells/spell_elk_include.h" 

#define AddScriptFlag(name, var) scriptFlagIds.emplace(name, & var)

#define ELKCUpdateAI() if (!UpdateVictim())     \
return;                                         \
if (ELKUpdateAI(diff))                          \
return;                                         \
if (me->HasUnitState(UNIT_STATE_CASTING))       \
{                                               \
    return;                                     \
}

#define ELKCActionType_NONE -1
#define ELKCActionType_MOVE -2

enum Events : uint16
{
    NONE,
    ATK_1,
    ATK_2,
    ATK_3,
    ATK_4,
    ATK_5,
    ATK_6,
    ATK_7,
    DEF_1,
    DEF_2,
    DEF_3,
    SPL_1,
    SPL_2,
    SPL_3,
    SPL_4,
    CHECK_HEALTH,
    REGULAR_CHECK,
    RETURN_CHECK,
    DYNAMIC_MOVEMENT_1,
    ACTION_PROGRESS,

};

enum class ELKCAType : uint8
{
    NONE,
    SPELL,
    JUMP
};
enum class  ELKCATarget : uint8
{
    NONE,
    VICTIM,
    VICTIMAWAY,
};
struct ELKCAction
{
    std::string name = "";
    uint8 min_dist = 0;
    uint8 max_dist = 0;
    uint16 delay = 0;
    float angle = 0.f;
    float speedXY = 0.f;
    float speedZ = 0.f;
    float distance = 0.f;
    uint32 id = 0;
    ELKCAType type = ELKCAType::NONE;
    ELKCATarget target = ELKCATarget::NONE;
    bool on_mutate = false;
    uint8 runeCost = false;
    uint32 cooldown = 0;
    int8 max_hp = 101;
    int8 min_hp = -1;
};
struct ELKCSequence
{
    std::string name = "";
    uint8 runeCost = 0;
    std::vector<uint8> actions = {};
    uint32 cooldown = 0;
};
struct ELKCCombo
{
    std::string name = "";
    std::vector< ELKCSequence> sequences = {};
    int16 probability = 0;
    std::vector<std::pair<int32, int8>> probabilityAura = {};
};

struct ELKAI;

enum class ELKActionMoveType : uint8
{
    NONE,
    DYNAMIC_MOVEMENT,
    LEAPING,
};
struct DynamicMovement
{
    uint16 moveTime = 0;
    float angleBase = 1.f;
    float angleRaw = 0.f;
    uint16 dist = 0;
    uint16 sprintPre = 0;
    uint16 sprintPost = 0;
};



class ELKCreatureScript : public CreatureScript
{
public:
#pragma region ELKCMain
    std::vector<ELKCCombo> Combos = {};
    std::map<int32, int8> AuraProbabilityTotal = {};
    int16 CombosTotal = 0;

    std::vector<ELKCAction> Actions = {};

    uint8 reinforcementCall = 0;
    uint8 chanceAtk = 1;
    uint8 chanceDef = 1;
    uint8 chanceSpell = 1;
    uint16 SpellPowerBonus = 0;

    uint32 regularCheck = 3500;
    uint32 regularCheckHP = 0;
    static inline std::map<std::string, ELKCreatureScript*> ELKCreatureScripts = {};

    DynamicMovement dynamicMovement;
    uint8 dynamicMovementOdds = 255;
protected:
    ELKCreatureScript(const char* name) : CreatureScript(name)
    {
        ELKCreatureScripts.emplace(name, this);
    }
#pragma endregion

#pragma region ELKCDialogue
public:
    struct CreatureResponseVariants
    {
        uint32 databaseId = 0;
        std::map<uint32, bool> flagTypes = {};
        uint32 choiceId = 0;

        static inline std::map<uint32, CreatureResponseVariants> variantId = {};
    };
    struct CreatureResponse
    {
        CreatureResponse() {}
        uint32 id = 0;
        std::map<uint32, bool> flagTypes = {};
        std::vector<uint32> flagsAdded = {};
        std::vector<CreatureResponseVariants*> variants = {};
        std::vector<uint32> _variants = {};
        std::vector<uint32> choices = {};

        bool CanUse(Player* player)
        {
            for (auto& [flag, yn] : flagTypes)
            {
                if (player->GetQuestStageFlag(flag) != yn)
                {
                    return false;
                }
            }
            return true;
        }
        static inline std::map<uint32, CreatureResponse*> AllResponses = {};
        static inline std::map<std::string, uint32> AllResponsesDictionary = {};
        static uint32 GetResponseId(std::string name)
        {
            auto it = AllResponsesDictionary.find(name);
            if (it != AllResponsesDictionary.end())
                return it->second;
            return 0;
        }
        static CreatureResponse* GetResponse(uint32 id)
        {
            auto it = AllResponses.find(id);
            if (it != AllResponses.end())
                return it->second;
            return nullptr;
        }
        uint32 GetText(Player* player, uint32 choice)
        {
            for(CreatureResponseVariants* var : variants)
                if (choice == var->choiceId)
                {
                    for (auto& [flag, yn] : var->flagTypes)
                    {
                        if (player->GetQuestStageFlag(flag) == yn)
                            return var->databaseId;
                    }
                }
            return 0;
        }
    };
    struct PlayerChoice
    {
        PlayerChoice() {}
        uint32 id = 0;
        std::map<uint32, bool> flagTypes = {};
        std::vector<uint32> flagsAdded = {};
        std::vector<uint32> responsesId = {};
        std::string text = "?/WHOOS/?";

        bool CanUse(Player* player)
        {
            for (auto& [flag, yn] : flagTypes)
            {
                if (player->GetQuestStageFlag(flag) != yn)
                {
                    return false;
                }
            }
            return true;
        }
        static inline std::map<uint32, PlayerChoice*> AllChoices = {};
        static inline std::map<std::string, uint32> AllChoicesDictionary = {};
        static uint32 GetChoiceId(std::string name)
        {
            auto it = AllChoicesDictionary.find(name);
            if (it != AllChoicesDictionary.end())
                return it->second;
            return 0;
        }
        static PlayerChoice* GetChoice(uint32 id)
        {
            auto it = AllChoices.find(id);
            if (it != AllChoices.end())
                return it->second;
            return nullptr;
        }
    };
    struct CreatureConversation
    {
        uint32 startResponse = 0;
        std::vector<uint32> batches = {};
    };

    std::vector<CreatureConversation> conversations = {};

    struct PlayerQuestFlag
    {
        static inline std::map<std::string, uint32> AllQuestFlags = {};
        static uint32 GetQuestFlag(std::string name)
        {
            auto it = AllQuestFlags.find(name);
            if (it != AllQuestFlags.end())
                return it->second;
            return 0;
        }
        uint32 id = 0;
    };
    std::map<std::string, uint32*> scriptChoiceIds = {};
    std::map<std::string, uint32*> scriptFlagIds = {};
    void FillScriptVars()
    {
        for (auto scr : scriptChoiceIds)
            *(scr.second) = PlayerChoice::GetChoiceId(scr.first);
        for (auto scr : scriptFlagIds)
            *(scr.second) = PlayerQuestFlag::GetQuestFlag(scr.first);
        
    }
    virtual bool PlayerChoiceScript(Player* player, PlayerChoice* choice, CreatureResponse* response) const { return true; }
    virtual bool CreatureResponseScript(Player* player, PlayerChoice* choice, CreatureResponse* response) const { return true; }
















#pragma endregion
#pragma region ELKCAI
    struct ELKAI : public ScriptedAI
    {
        virtual void sGossipHello(Player* player)  override
        {
            for (auto& convo : script->conversations)
            {
                CreatureResponse* response = CreatureResponse::GetResponse(convo.startResponse);
                if (!response)
                    continue;
                if (!response->CanUse(player))
                    continue;
                for (auto ch : response->choices)
                {
                    PlayerChoice* newChoice = PlayerChoice::GetChoice(ch);
                    if (!newChoice)
                        continue;
                    if (!newChoice->CanUse(player))
                        continue;
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, newChoice->text, GOSSIP_SENDER_MAIN, newChoice->id);
                }
                for (auto& batch : convo.batches)
                {
                    PlayerChoice* newChoice = PlayerChoice::GetChoice(batch);
                    if (!newChoice)
                        continue;
                    if (!newChoice->CanUse(player))
                        continue;
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, newChoice->text, GOSSIP_SENDER_MAIN, newChoice->id);
                }
                SendGossipMenuFor(player, response->GetText(player, 0), me);
                return;
            }
        }

        void sGossipSelect(Player* player, uint32 sender, uint32 uiAction) override
        {
            ClearGossipMenuFor(player);
            PlayerChoice* choice = PlayerChoice::GetChoice(uiAction);
            if (!choice->CanUse(player))
                return;
            for (uint8 i = 0; i < choice->responsesId.size(); i++)
            {
                CreatureResponse* response = CreatureResponse::GetResponse(choice->responsesId[i]);
                if (!response->CanUse(player))
                    break;

                if (script->PlayerChoiceScript(player, choice, response))
                    if (script->CreatureResponseScript(player, choice, response))
                    {

                        for (auto ch : response->choices)
                        {
                            PlayerChoice* newChoice = PlayerChoice::GetChoice(ch);
                            if (!newChoice)
                                continue;
                            if (!newChoice->CanUse(player))
                                continue;
                            AddGossipItemFor(player, GOSSIP_ICON_CHAT, newChoice->text, GOSSIP_SENDER_MAIN, newChoice->id);
                        }

                        SendGossipMenuFor(player, response->GetText(player, choice->responsesId[i]), me);

                        return;
                    }
            }
        }


        ELKAI(Creature* creature, const ELKCreatureScript* s) : ScriptedAI(creature)
        {
            reinforcementEntries.push_back(me->GetEntry());
            script = s;
            ResetExtra();
            for (auto& seq : script->Combos)
                sequenceCooldown.push_back(std::map<uint8, uint16>());
        }
        const ELKCreatureScript* script;
        EventMap events;

        std::vector<WorldLocation> positions{ me->GetWorldLocation() };

        uint8 reinforcementCall = RandomInt(1, 2);

        uint8 chanceAtk = RandomInt(2, 3);
        uint8 chanceDef = RandomInt(0, 1);
        uint8 chanceSpell = 0;

        uint8 mode = 0;

        uint8 lastCategory = 0;
        uint8 lastChoice = 0;

        uint32 regularCheck = 2500;

        DynamicMovement dynamicMovement;

        std::map<uint32, uint16> baseCost;

        std::vector<uint32> reinforcementEntries;

        void Reset() override
        {
            ResetExtra();
            events.Reset();
            ResetCombo();
            ResetCC();
            chanceAtk = script->chanceAtk;
            chanceDef = script->chanceDef;
            chanceSpell = script->chanceSpell;
            regularCheck = script->regularCheck + me->GetHealthPct() * script->regularCheckHP;

            lastCategory = 0;
            lastChoice = 0;
        }

        virtual void ResetExtra() {};

        void DelayAttack()
        {
            me->setAttackTimer(BASE_ATTACK, me->GetAttackTime(BASE_ATTACK));
        }
        bool EasyCast(uint32 spell)
        {

            SpellCastResult ret = me->CastSpell(me, spell, false);
            if (ret == SPELL_CAST_OK)
            {
               
                return true;
            }
            return false;
        }
        bool EasyCastLocation(uint32 spell, Position point)
        {
            auto target = me->GetVictim();
            if (!target || me->GetSpellCooldown(spell))
                return false;
            SpellCastResult ret = me->CastSpell(point.GetPositionX(), point.GetPositionY(), point.GetPositionZ(), spell, false);
            if (ret == SPELL_CAST_OK)
            {
               
                return true;
            }
            return false;
        }
        bool EasyCastVictim(uint32 spell)
        {
            auto target = me->GetVictim();
            if (!target || me->GetSpellCooldown(spell))
                return false;
            SpellCastResult ret = me->CastSpell(target, spell, false);
            if (ret == SPELL_CAST_OK)
            {
               
                return true;
            }
            return false;
        }
        void ReinforcementCall()
        {
            auto target = me->GetVictim();
            if (!target)
                return;
            uint16 fighting = target->GetThreatMgr().GetThreatListSize();
            if (fighting < reinforcementCall)
            {
                std::list<Creature*> cList;

                for (auto i : reinforcementEntries)
                {
                    me->GetCreaturesWithEntryInRange(cList, 40.0f, i);
                }
                for (auto cre : cList)
                {
                    if (cre->IsInCombat() == false and cre->IsAlive() == true)
                    {
                        fighting++;
                        cre->CombatStart(target);
                        cre->AddThreat(target, 1.0f);
                        target->AddThreat(cre, 1.0f);
                    }
                    if (fighting >= reinforcementCall)
                        break;
                }
            }
        }
        virtual void EnterCombatCustom(Unit* /*who*/)
        {

        }
        void JustEngagedWith(Unit* who) override
        {
            EnterCombatCustom(who);
            ReinforcementCall();
        }

        virtual void MovementInform(uint32 type, uint32 point)
        {
            if (type != POINT_MOTION_TYPE)
                return;
        }

        void RandomAction()
        {
            std::vector<uint8> indexesPassed = {};
            int16 countBase = script->CombosTotal;
            int16 random = rand();
            for (auto aura : script->AuraProbabilityTotal)
            {
                if (me->HasAura(aura.first))
                    countBase += aura.second;
            }
            int16 count = random % countBase;

            int16 countPassed = 0;
            while (true)
            {
                int8 category = -1;
                while (count >= 0)
                {
                    category++;
                    if (category >= script->Combos.size())
                        category -= script->Combos.size();

                    for (uint8 passed : indexesPassed)
                    {
                        if (category == passed)
                            goto ContinueCombos;
                    }
                    count -= script->Combos[category].probability;
                    for (auto aura : script->Combos[category].probabilityAura)
                    {
                        if (me->HasAura(aura.first))
                            count -= aura.second;
                    }
                ContinueCombos:
                    continue;
                }
                uint8 option = script->Combos[category].sequences.size();
                countPassed += script->Combos[category].probability;

                if (!option)
                    goto NextAction;
                {
                    uint8 rnd = random % option;
                    for (uint8 i = 0; i < option; i++)
                    {
                        uint8 choice = (i + rnd) % (option);
                        if (category == lastCategory && lastChoice == choice)
                            goto NextAction;
                        bool exit = false;
                        RandomCombo({ category, choice }, exit);
                        if (exit)
                        {
                            lastCategory = category;
                            lastChoice = choice;
                            return;
                        }
                    }
                }
            NextAction:
                indexesPassed.push_back(category);
                if (indexesPassed.size() < script->Combos.size())
                    count = random % (countBase - countPassed);
                else
                    return;
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
        void RandomCombo(std::pair<int16, uint8> combo, bool& exit)
        {
            if (sequenceCooldown[combo.first][combo.second] > 0)
                return;
            const ELKCSequence& seq = script->Combos[combo.first].sequences[combo.second];
            const ELKCAction& act = script->Actions[seq.actions[0]];
            int8 healthPct = me->GetHealthPct();
            if (seq.runeCost <= me->GetAvailableRunes() && act.min_hp < healthPct && act.max_hp >= healthPct)
            {
                exit = DoELKAIAction(act);
                if (exit)
                {
                    sequenceCooldown[combo.first][combo.second] = seq.cooldown;
                    currentCombo.type = combo.first;
                    currentCombo.combo = combo.second;
                    currentCombo.action = 0;
                    events.ScheduleEvent(ACTION_PROGRESS, act.delay);
                }
            }
        }
        bool DoELKAIAction(const ELKCAction& act)
        {
            currentCombo.typeMove = ELKActionMoveType::NONE;
            if(act.type == ELKCAType::SPELL)
            {
                if (act.target == ELKCATarget::NONE)
                {
                    if (act.runeCost <= me->GetAvailableRunes() && me->GetDistance(me->GetVictim()) >= act.min_dist && (me->GetDistance(me->GetVictim()) < act.max_dist || act.max_dist == 0) && EasyCast(act.id))
                        return true;
                }
                else if (act.target == ELKCATarget::VICTIM)
                {
                    if (act.runeCost <= me->GetAvailableRunes() && me->GetDistance(me->GetVictim()) >= act.min_dist && (me->GetDistance(me->GetVictim()) < act.max_dist || act.max_dist == 0) && EasyCastVictim(act.id))
                        return true;
                }
            }
            else if (act.type == ELKCAType::JUMP)
            {
                if (act.target == ELKCATarget::VICTIMAWAY)
                {
                    if (me->GetDistance(me->GetVictim()) >= act.min_dist && (me->GetDistance(me->GetVictim()) < act.max_dist || act.max_dist == 0))
                    {
                        Position pos = me->GetFirstCollisionPosition(act.distance, me->GetVictim()->GetRelativeAngle(me) + act.angle);
                        if(act.on_mutate)
                            currentCombo.typeMove = ELKActionMoveType::LEAPING;
                        JumpTowards(pos, act.speedXY, act.speedZ, me->GetVictim());
                        return true;
                    }
                }
                else if (act.target == ELKCATarget::VICTIM)
                {
                    if (me->GetDistance(me->GetVictim()) >= act.min_dist && (me->GetDistance(me->GetVictim()) < act.max_dist || act.max_dist == 0))
                    {
                        Position pos = me->GetFirstCollisionPosition(me->GetDistance(me->GetVictim()) + act.distance, me->GetRelativeAngle(me->GetVictim()) + act.angle);
                        if (act.on_mutate)
                            currentCombo.typeMove = ELKActionMoveType::LEAPING;
                        JumpTowards(pos, act.speedXY, act.speedZ, me->GetVictim());
                        return true;
                    }
                }
            }
            return false;
        }
        bool ContinueCombo()
        {
            const ELKCSequence& seq = script->Combos[currentCombo.type].sequences[currentCombo.combo];

            if (seq.actions.size() > currentCombo.action + 1)
            {
                const ELKCAction& act = script->Actions[seq.actions[currentCombo.action + 1]];
                if (DoELKAIAction(act))
                {
                    currentCombo.action++;
                    if(currentCombo.typeMove == ELKActionMoveType::NONE)
                        events.ScheduleEvent(ACTION_PROGRESS, act.delay);
                    return true;
                }
            }
            ResetCombo();
            return false;
        }
        struct CurrentCombo
        {
            int8 type = ELKCActionType_NONE;
            uint8 combo = 0;
            uint8 action = 0;
            ELKActionMoveType typeMove = ELKActionMoveType::NONE;
            void Reset()
            {
                type = ELKCActionType_NONE;
                combo = 0;
                action = 0;
                typeMove = ELKActionMoveType::NONE;
            }
        };
        CurrentCombo currentCombo;
        std::vector<std::map<uint8, uint16>> sequenceCooldown = {

        };

        bool ResetCombo()
        {
            currentCombo.Reset();
            events.CancelEvent(ACTION_PROGRESS);
            events.RescheduleEvent(REGULAR_CHECK, regularCheck);
            return true;
        }; // FIRST IS COMBO, SECOND IS INDEX
        virtual void DamageTakenExtra(Unit* doneby, uint32& damage, DamageEffectType type, SpellSchoolMask school)
        {

        }
        void DamageTaken(Unit* doneby, uint32& damage, DamageEffectType type, SpellSchoolMask school) override
        {
            DamageTakenExtra(doneby, damage, type, school);
            regularCheck = script->regularCheck + me->GetHealthPct() * script->regularCheckHP;
        }
        virtual void OnMutate() {};
        void sOnMutate() override
        {
            OnMutate();
            if (!me->GetVictim() || !me->CanFreeMove())
            {
                if (currentCombo.typeMove == ELKActionMoveType::DYNAMIC_MOVEMENT)
                {
                    ResetDynamicMovement();
                }
                if (currentCombo.typeMove == ELKActionMoveType::LEAPING)
                {
                    ResetCombo();
                }
                return;
            }
            else if (currentCombo.typeMove == ELKActionMoveType::LEAPING)
            {
                ContinueCombo();
            }
        }

        virtual void ResetCC()
        {
        }
        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            if (me->HasUnitState(UNIT_STATE_STUNNED))
            {
                ResetDynamicMovement();
                events.Reset();
                ResetCC();
                events.ScheduleEvent(REGULAR_CHECK, regularCheck);
                ResetCombo();
            }
            SpellHitExtra(caster, spell);
        }
        virtual void SpellHitExtra(Unit* caster, SpellInfo const* spell)
        {

        }
        bool ELKUpdateAI(uint32 diff)
        {
            events.Update(diff);
            for (auto& cdCat : sequenceCooldown)
            {
                for (auto cC = cdCat.begin(); cC != cdCat.end(); )
                {
                    if (cC->second > diff)
                    {
                        cC->second -= diff;
                        cC++;
                    }
                    else
                        cdCat.erase(cC++);
                }
            }
            if (me->isMoving())
            {
                if (currentCombo.typeMove == ELKActionMoveType::LEAPING)
                    return true;
                if (currentCombo.typeMove == ELKActionMoveType::DYNAMIC_MOVEMENT)
                {
                    if (dynamicMovement.dist < diff)
                    {
                        dynamicMovement.dist = script->dynamicMovement.dist;
                        DynamicMove();
                    }
                    else
                        dynamicMovement.dist -= diff;

                    if (dynamicMovement.moveTime < diff)
                    {
                        ResetDynamicMovement();
                        RandomAction();
                    }
                    else
                        dynamicMovement.moveTime -= diff;


                    return true;
                }
            }
            else
            {
                if (currentCombo.typeMove == ELKActionMoveType::DYNAMIC_MOVEMENT)
                {
                    ResetDynamicMovement();
                }
            }
            return false;
        }
        void ResetDynamicMovement()
        {
            ResetCombo();
            me->RemoveAura(2983);
            dynamicMovement.moveTime = 0;
            if (me->GetVictim())
                me->SetFacingToObject(me->GetVictim());
            if (me->GetMotionMaster()->IsDynamicMovement())
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->SetDynamicMovement(false);
                if(me->GetVictim())
                    me->ResumeChasingVictim();
                else
                    me->GetMotionMaster()->MoveIdle();
                if (script->dynamicMovement.sprintPost)
                {
                    Aura* aura = me->AddAura(2983, me);
                    aura->SetDuration(script->dynamicMovement.sprintPost);
                }
            }
        }
        virtual void StartDynamicMovements()
        {
            if (me->GetMotionMaster()->IsDynamicMovement())
            {
                ResetDynamicMovement();
            }
            me->GetMotionMaster()->SetDynamicMovement(true);

            currentCombo.typeMove = ELKActionMoveType::DYNAMIC_MOVEMENT;
            currentCombo.type = ELKCActionType_MOVE;                                                 // UNIQUE
            dynamicMovement.moveTime = script->dynamicMovement.moveTime;
            int8 posneg = (rand() % 2);
            if (posneg == 0)
                posneg = -1;
            dynamicMovement.angleBase = posneg * script->dynamicMovement.angleBase;
            dynamicMovement.angleRaw = posneg * script->dynamicMovement.angleRaw;
            dynamicMovement.dist = script->dynamicMovement.dist;

            dynamicMovement.angleBase += me->GetOrientation();
            DynamicMove();
            if (script->dynamicMovement.sprintPre)
            {
                Aura* aura = me->AddAura(2983, me);
                aura->SetDuration(script->dynamicMovement.sprintPre);
            }
        }
        virtual bool DoEventsExtra()
        {
            return false;
        }
        void DoEvents()
        {
            switch (events.ExecuteEvent())
            {
            case ACTION_PROGRESS:
                ContinueCombo();
                break;
            case REGULAR_CHECK:
                events.ScheduleEvent(REGULAR_CHECK, regularCheck);
                if (currentCombo.type != -1)
                {
                    DoEvents();
                    break;
                }
                DoEvents();

                if (me->GetDistance(me->GetVictim()) < 4)
                {
                    if (rand() % script->dynamicMovementOdds == 0)
                    {
                        StartDynamicMovements();

                        break;
                    }
                }

                if (DoEventsExtra())
                    break;
                RandomAction();
                break;
            default:
                return;
            }
        }
        void DynamicMove()
        {
            if (!me->GetMotionMaster()->IsDynamicMovement())
            {
                ResetDynamicMovement();
                return;
            }
            
            me->GetMotionMaster()->MoveIdle();
            dynamicMovement.angleBase += dynamicMovement.angleRaw;
            me->SetOrientation(dynamicMovement.angleBase);
            Position pos = me->GetPosition();


            me->MovePositionToFirstCollision(pos, 5.f, 0);

            me->GetMotionMaster()->MovePoint(0, pos.m_positionX, pos.m_positionY, pos.m_positionZ, false, false);
            me->GetMotionMaster()->SetDynamicMovement(true);
        }


    };
#pragma endregion 
};
/*
class magistrix_erona : public ELKCreatureScript
{
public:

    static inline ResponseHolder responses = {};

    magistrix_erona() : ELKCreatureScript("magistrix_erona")
    {
        responses.Insert(DialogueResponse(
            "ERONA_INTRO_1",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_1A", {}, {}, "ERONA_TALK_2", nullptr),
                DialogueLine("ERONA_DIALOGUE_1B", {}, {}, "ERONA_TALK_2", nullptr),
                DialogueLine("ERONA_DIALOGUE_1C", {}, {}, "ERONA_TALK_2", nullptr)
            },
            nullptr
        ));

        responses.Insert(DialogueResponse(
            "ERONA_TALK_2",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2B", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2C", {}, {}, "ERONA_TALK_2C", nullptr)
            },
            nullptr
        ));

        responses.Insert(DialogueResponse(
            "ERONA_TALK_2C",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2B", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2C", {}, {}, "ERONA_TALK_2C", nullptr)
            },
            nullptr
        ));

        Responses.emplace(EronaResponses::TALK_2C, DialogueResponse(
            "ERONA_TALK_2C",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2B", {}, {}, "ERONA_TALK_3", nullptr),
                DialogueLine("ERONA_DIALOGUE_2CA", {}, {}, "ERONA_TALK_2CA", nullptr),
                DialogueLine("ERONA_DIALOGUE_2CB", {}, {}, "ERONA_TALK_2CB", nullptr)
            },
            nullptr
        ));

        responses.Refine();







        Responses.emplace(EronaResponses::TALK_2CA, DialogueResponse(
            "ERONA_TALK_2CA",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, EronaResponses::TALK_3, nullptr),
                DialogueLine("ERONA_DIALOGUE_2B", {}, {}, EronaResponses::TALK_3, nullptr)
            },
            nullptr
        ));
        Responses.emplace(EronaResponses::TALK_2CB, DialogueResponse(
            "ERONA_TALK_2CB",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, EronaResponses::TALK_3, nullptr),
                DialogueLine("ERONA_DIALOGUE_2B", {}, {}, EronaResponses::TALK_3, nullptr)
            },
            nullptr
        ));
        Responses.emplace(EronaResponses::TALK_3, DialogueResponse(
            "ERONA_TALK_2CB",
            {},
            {
                DialogueLine("ERONA_DIALOGUE_2A", {}, {}, EronaResponses::TALK_4, nullptr)
            },
            nullptr
        ));
    }
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new magistrix_eronaAI(creature);
    }
    struct magistrix_eronaAI : public ELKAI
    {
        magistrix_eronaAI(Creature* creature) : ELKAI(creature) {}

        virtual void sGossipHello(Player* player)  override
        {
            if (player->GetQuestStageFlag(QuestStageFlags::FELENDREN_PUT_DOWN))
            {
                AddGossipItemFor(player, 0, "<Attack> You're dead.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_PUT_DOWN_ATTACK);
                AddGossipItemFor(player, 0, "<Return> Never mind, I'm being hasty. What else can I do?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_6);
                SendGossipMenuFor(player, FELENDREN_PUT_DOWN_RESET, me);
            }
        }
        void sGossipSelect(Player* player, uint32 sender, uint32 uiAction) override
        {
            ClearGossipMenuFor(player);
            switch (uiAction)
            {
            case FELENDREN_LEAVE_DIALOGUE:
            {
                AddGossipItemFor(player, 0, "What happens to be your problem?", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_1);
                AddGossipItemFor(player, 0, "Speak quickly or you'll be a skinned rat soon.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_1);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<Attack> Never mind, you're just another pathetic waste of Mana.", GOSSIP_SENDER_MAIN, FELENDREN_DIALOGUE_ASSAULTED);
                SendGossipMenuFor(player, FELENDREN_TALK_1, me);
                break;
            }
        }
    };

    struct DialogueLine
    {
        DialogueLine() {};
        DialogueLine(std::string textIndex, std::vector<std::string> flagNeededName, std::vector<std::string> flagAddedName, std::string nextResponse, void (ELKAI::* pressedScript)())
            : nextResponse(nextResponse),
            pressedScript(pressedScript)
        {
            ELKDialogue* dial = sObjectMgr->GetELKDialogue(textIndex);
            if (dial)
                textLine = dial->dialogue;
            else
                textLine = "";
            for (std::string& flag : flagNeededName)
            {
                flagNeeded.push_back(sObjectMgr->GetELKFlag(flag));
            }
            for (std::string& flag : flagAddedName)
            {
                flagAdded.push_back(sObjectMgr->GetELKFlag(flag));
            }
        }
        std::string textLine = "";
        std::vector<uint32> flagNeeded = {};
        std::vector<uint32> flagAdded = {};
        std::string nextResponse = "";
        uint8 nextIndex = 0;
        void (ELKAI::* pressedScript)() = nullptr; // ai.appearScript = &ELKAI::ScriptFunction;
    };

    struct DialogueResponse
    {
        DialogueResponse() {};
        DialogueResponse(std::string textName, std::vector<std::string> flagNeededName, std::vector<DialogueLine> lines, void (ELKAI::* scriptMethod)())
            : textName(textName),
            lines(lines),
            appearScript(scriptMethod)
        {
            ELKDialogue* dial = sObjectMgr->GetELKDialogue(textName);
            if (dial)
                textIndex = dial->id + 1000000;
            else
                textIndex = 1000001;
            for (std::string& flag : flagNeededName)
            {
                flagNeeded.push_back(sObjectMgr->GetELKFlag(flag));
            }






            // get the shit in the deebee
            //std::string nextLines = DB[string];
            //std::vector<std::string> v = SplitLines(nextLines);
        }
        std::vector<std::string> SplitLines(std::string const& str) {
            std::vector<std::string> ret;
            std::stringstream ss(str);
            std::string item;

            while (getline(ss, item, ' ')) {
                ret.push_back(item);
            }

            return ret;
        }

        std::string textName = "";
        uint32 textIndex = 0;
        std::vector<uint32> flagNeeded = {};
        std::vector<DialogueLine> lines = {};
        void (ELKAI::* appearScript)() = nullptr; // ai.appearScript = &ELKAI::ScriptFunction;
    };

    struct ResponseHolder
    {
    private:
        std::vector<DialogueResponse> responsesArray = {};
        std::map<std::string, uint16> responseIndexes = {};
    public:
        void AddResponseIndex(std::string str)
        {
            if (responseIndexes[str] == 0)
            {
                responseIndexes[str] = responseIndexes.size() + 1;
            }
        }

        void Insert(DialogueResponse const& response)
        {
            AddResponseIndex(response.textName);
            for (auto& line : response.lines)
            {
                if (line.nextResponse != "")
                    AddResponseIndex(line.nextResponse);
            }
            responsesArray.push_back(response);
        }
        void Refine()
        {
            for (auto& response : responsesArray)
            {
                for (auto& line : response.lines)
                {
                    line.nextIndex = responseIndexes[line.nextResponse];
                }
            }
        }
    };
    /*
        dialogue loops from last to first, chekcing conditions.
    */
