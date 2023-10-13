#pragma once

#include "Group.h"
#include "PetDefines.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Totem.h"
#include "UnitAI.h"
#include <MoveSplineInit.h>

class spell_extension_system;
enum class MapDummy : uint8;


inline uint32 RandomInt(uint32 min, uint32 max)
{
    uint32 ret = rand32() % max;
    return std::min(ret, min);
}

enum OutsideSpells
{

};

 enum class ELKSpells : uint32
{
    ATTACK_HIT =                100002,
    ATTACK =                    100003,
    ATTACK_SLOW_DEBUFF =        100004,
    ANIMATION_RESET =           100005,
    SPELL_DEFLECT =             100006,
    COMBO_COUNT =               100007,
    SPELL_DEFLECT_SHORT =       100008,
    CRITICAL_ATTACK_HIT =       100009,
    CRITICAL_ATTACK =           100010,
    SPELL_DOUBLE_JUMP_BACK =    100011,
    SPELL_FEL_RUSH_BUFF =       100012,
    DISENGAGE_MINI_HOP =        100014,
    ATTACK1H =                  100025,
    ATTACK2HL =                 100026,
    ATTACK1HP =                 100027,
    ATTACKOFF =                 100028,
    ATTACKOFFP =                100029,
    ATTACKUNARMED =             100030,
    ATTACKSHIELD =              100031,
    CRITICAL_ATTACK1H =         100032,
    CRITICAL_ATTACKUNARMED =    100033,
    CRITICAL_ATTACKMUTILATE =   100034,
    CRITICAL_ATTACK2H =         100035,
    DEFLECT1H =                 100036,
    DEFLECTUNARMED =            100037,
    DEFLECTSHEILDBLOCK =        100038,
    DEFLECT2HL =                100039,
    SPIN_ATTACK_HIT =           100044,
    SPIN_ATTACK =               100045,

    CRUSADER_STRIKE =           1000001,
    FLASH_FOCUS =               1000012,
    HOLY_POWER =                1000014,
    CRUSADER_STRIKE_HIT =       1000021,
};
#define ELKS(x) uint32(ELKSpells::x)
enum SpellsC // Creature Spells.
{
    MIND_FLAY =                 150001,
    REJUVINATION =              150002,
    FIREBALL =                  150003,
    THROW =                     150004,
    FRENZY =                    150005,
    SHADOW_WORD_PAIN =          150006,
    ARCANE_BOLT =               150007,
    FEATHER_BURST =             150008,
    SUNDER_ARMOR =              150009,
    MIND_BLAST =                150010,
    WITHERED_TOUCH =            150011,
    EVASION =                   150012,
    HEAD_CRACK =                150013,
    POISON =                    150014,
    SLOW =                      150015,
    DECAYED_AGILITY =           150016,
    FAERIE_FIRE =               150017,
    MOONFIRE =                  150018,
    ARCANE_RESIDUE =            150019,
    CHAIN_LIGHTNING =           150020,
    FROST_SHOCK =               150021,
    HEALING_WAVE =              150022,
    WITHER_STRIKE =             150023,
    FLASH_HEAL =                150024,
    LIGHTNING_BOLT =            150025,
    WRATH =                     150026,
    FEAR =                      150027,
    ARCANE_MISSILES =           150028,
    ARCANE_TORRENT =            150029,
    SHALLOW_LEAP =              150030,
    TALL_LEAP =                 150031,
    SLIDE_LEAP =                150034,
    CRUSHING_WAVE_HIT =         150035,
    CRUSHING_WAVE =             150036,
    SHALLOW_LEAP_500 =          150037,
    GIGA_THUNDER_CLAP =         150038,
    GIGA_THUNDER_CLAP_HIT =     150039,
};

struct ExtensionObj
{
    ExtensionObj(void(*Function)(spell_extension_system*, Spell*), int32 Cost, bool Unique, UnitMods Mod, float ModMulti) : Function(Function), Cost(Cost), Unique(Unique), Mod(Mod), ModMulti(ModMulti) {};

    void(*Function)(spell_extension_system*, Spell*);
    int32 Cost;
    bool Unique;
    UnitMods Mod;
    float ModMulti;
};

#define ComboMap std::map<uint32, const ExtensionObj>
#define AddExtension(a, b, c, d, e, f) spell_extension_system::Extensions.emplace(uint32(a), ExtensionObj(b,c,d,e,f))

class ELKAuraScript : public AuraScript
{
protected:
};

class ELKSpellScript : public SpellScript
{
protected:
    void TriggerExtension()
    {
        Aura* aura = GetCaster()->GetAura(1000000);
        aura = GetCaster()->AddAura(1000000, GetCaster());
    }
    void AttackBegin()
    {
        GetSpell()->SetBonusRange(GetCaster()->GetCombatReach());
    }
    void AttackHit()
    {
        Unit* victim = GetHitUnit();

        CalcDamageInfo damageInfo;
        GetCaster()->CalculateMeleeDamage(victim, &damageInfo);
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION;
        Unit::DealDamageMods(victim, damageInfo.damages[0].damage, &damageInfo.damages[0].absorb);
        GetCaster()->PlayDistanceSound(129);

        GetCaster()->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        dmgInfo.SetSpellInfo(GetSpellInfo());
        GetCaster()->ProcDamageAndSpell(damageInfo.target, damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damages[0].damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);
        GetCaster()->DoOnAttackHitScripts(victim, dmgInfo);
    }
    void AfterAttack()
    {
        GetCaster()->DoAfterAttackScripts();
    }
    void QueSpell(Unit* caster)
    {
        Player* player = caster->ToPlayer();
        if (!player)
            return;
        player->SetQuedSpell(GetSpellInfo()->Id); 
    }
};


class spell_elk_attack : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_attack);

    uint8 comboLength = 0;

    void SpellClick()
    {
        Spell* curAtk = GetCaster()->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == ELKS(ATTACK))
            {
                if (curAtk->GetSpellTimer() < 50)
                {
                    GetSpell()->skip = true;
                    GetCaster()->AddSpellCooldown(GetSpell()->m_spellInfo->Id, 0, 100, false);
                    QueSpell(GetCaster());
                    return;
                }
                else
                {
                    GetSpell()->skip = true;
                    GetCaster()->AddSpellCooldown(GetSpell()->m_spellInfo->Id, 0, 100, false);
                    return;
                }
            }
        }
        else
        {
            curAtk = GetCaster()->GetCurrentSpell(CURRENT_GENERIC_SPELL);
            if (curAtk)
            {
                if (curAtk->m_spellInfo->Id == ELKS(ATTACK_HIT) && curAtk->GetSpellTimer() > 0)
                {
                    GetSpell()->skip = true;
                    GetCaster()->AddSpellCooldown(GetSpell()->m_spellInfo->Id, 0, 1, false);
                    QueSpell(GetCaster());
                    return;
                }
            }
        }
        if (GetCaster()->IsPlayer())
        {
            Player* player = GetCaster()->ToPlayer();
            if (player)
            {
                int weps = 0;
                int anim = RandomInt(1, 4);
                int animSpell = ELKS(ATTACKUNARMED);
                auto wep1 = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND); 
                auto wep2 = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
                std::vector<const ItemTemplate*> weapons = {nullptr, nullptr};
                if (wep1)
                    weapons[0] = wep1->GetTemplate();
                if (wep2)
                    weapons[1] = wep2->GetTemplate(); 
                if (weapons[0])
                {
                    weps += 1;

                    switch (weapons[0]->GetSkill())
                    {
                    case SKILL_AXES:
                    case SKILL_SWORDS:
                    case SKILL_MACES:
                        animSpell = ELKS(ATTACK1H);
                        break;
                    case SKILL_FISHING:
                    case SKILL_2H_AXES:
                    case SKILL_2H_SWORDS:
                    case SKILL_2H_MACES:
                        animSpell = ELKS(ATTACK);
                        break;
                    case SKILL_POLEARMS:
                    case SKILL_STAVES:
                        animSpell = ELKS(ATTACK2HL);
                        break;
                    case SKILL_DAGGERS:
                        animSpell = ELKS(ATTACK1HP);
                        break;
                    case SKILL_SHIELD:
                        animSpell = ELKS(ATTACKSHIELD);
                        break;
                    case SKILL_FIST_WEAPONS:
                    default:
                        animSpell = ELKS(ATTACKUNARMED);
                        break;
                    }

                }

                if (weapons[1])
                {
                    weps += 2;

                    switch (weapons[1]->GetSkill())
                    {
                    case SKILL_AXES:
                    case SKILL_SWORDS:
                    case SKILL_MACES:
                        animSpell = ELKS(ATTACKOFF);
                        break;
                    case SKILL_FISHING:
                    case SKILL_2H_AXES:
                    case SKILL_2H_SWORDS:
                    case SKILL_2H_MACES:
                        animSpell = ELKS(ATTACKOFF);
                        break;
                    case SKILL_POLEARMS:
                    case SKILL_STAVES:
                        animSpell = ELKS(ATTACKOFFP);
                        break;
                    case SKILL_DAGGERS:
                        animSpell = ELKS(ATTACKOFFP);
                        break;
                    case SKILL_SHIELD:
                        animSpell = ELKS(ATTACKSHIELD);
                        break;
                    case SKILL_FIST_WEAPONS:
                    default:
                        animSpell = ELKS(ATTACKUNARMED);
                        break;
                    }
                } 
                if (weps == 0)
                    GetSpell()->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(ELKS(ATTACKUNARMED)));
                else
                    GetSpell()->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(animSpell));

            }
        }
        else
        {
            Creature* creature = GetCaster()->ToCreature();
            if (creature)
            {

                int weps = 0;
                int anim = RandomInt(1, 4);
                int animSpell = ELKS(ATTACKUNARMED);
                auto weapons = creature->GetWeaponEquippedEntry();
                 
                if (weapons[0])
                {
                    weps += 1;

                    switch (weapons[0]->GetSkill())
                    {
                    case SKILL_AXES:
                    case SKILL_SWORDS:
                    case SKILL_MACES:
                        animSpell = ELKS(ATTACK1H);
                    case SKILL_FISHING:
                    case SKILL_2H_AXES:
                    case SKILL_2H_SWORDS:
                    case SKILL_2H_MACES:
                        animSpell = ELKS(ATTACK);
                    case SKILL_POLEARMS:
                    case SKILL_STAVES:
                        animSpell = ELKS(ATTACK2HL);
                    case SKILL_DAGGERS:
                        animSpell = ELKS(ATTACK1HP);
                    case SKILL_SHIELD:
                        animSpell = ELKS(ATTACKSHIELD);
                    case SKILL_FIST_WEAPONS:
                    default:
                        animSpell = ELKS(ATTACKUNARMED);
                    }

                }

                if (weapons[1])
                {
                    weps += 2;

                    switch (weapons[1]->GetSkill())
                    {
                    case SKILL_AXES:
                    case SKILL_SWORDS:
                    case SKILL_MACES:
                        animSpell = ELKS(ATTACKOFF);
                    case SKILL_FISHING:
                    case SKILL_2H_AXES:
                    case SKILL_2H_SWORDS:
                    case SKILL_2H_MACES:
                        animSpell = ELKS(ATTACKOFF);
                    case SKILL_POLEARMS:
                    case SKILL_STAVES:
                        animSpell = ELKS(ATTACKOFFP);
                    case SKILL_DAGGERS:
                        animSpell = ELKS(ATTACKOFFP);
                    case SKILL_SHIELD:
                        animSpell = ELKS(ATTACKSHIELD);
                    case SKILL_FIST_WEAPONS:
                    default:
                        animSpell = ELKS(ATTACKUNARMED);
                    }
                } 
                GetSpell()->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(animSpell));

            }
        }
        uint16 cd = 0;
        if(GetCaster()->CanUseAttackType(BASE_ATTACK))
            cd += GetCaster()->GetAttackTime(BASE_ATTACK);
        if (GetCaster()->CanUseAttackType(OFF_ATTACK))
            cd += GetCaster()->GetAttackTime(OFF_ATTACK);
        GetSpell()->SetRuneCooldown(cd);
        GetSpell()->SetRuneCost(1);

        if (auto aura = GetCaster()->GetAura(ELKS(COMBO_COUNT)))
            comboLength = aura->GetStackAmount();
    }
    virtual void AttackUnique()
    {

    }
    void AttackHit()
    {
        GetCaster()->CastSpell(GetCaster(), ELKS(ATTACK_HIT), false);
        AttackUnique();
    }
    virtual void Register()
    {
        BeforeSpellLoad += SpellCastFn(spell_elk_attack::SpellClick);
        AfterFullChannel += SpellCastFn(spell_elk_attack::AttackHit);
    }

};

class spell_elk_spin_attack : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_spin_attack);

    uint8 comboLength = 0;



    void SpellClick()
    {
        Spell* curAtk = GetCaster()->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == ELKS(ATTACK))
            {
                if (curAtk->GetSpellTimer() < 50)
                {
                    GetSpell()->skip = true;
                    GetCaster()->AddSpellCooldown(GetSpell()->m_spellInfo->Id, 0, 100, false);
                    QueSpell(GetCaster());
                    return;
                }
                else
                {
                    GetSpell()->skip = true;
                    GetCaster()->AddSpellCooldown(GetSpell()->m_spellInfo->Id, 0, 100, false);
                    return;
                }
            }
        }
        else
        {
            curAtk = GetCaster()->GetCurrentSpell(CURRENT_GENERIC_SPELL);
            if (curAtk)
            {
                if (curAtk->m_spellInfo->Id == ELKS(ATTACK_HIT) && curAtk->GetSpellTimer() > 0)
                {
                    GetSpell()->skip = true;
                    GetCaster()->AddSpellCooldown(GetSpell()->m_spellInfo->Id, 0, 1, false);
                    QueSpell(GetCaster());
                    return;
                }
            }
        }

        uint16 cd = 0;
        if (GetCaster()->CanUseAttackType(BASE_ATTACK))
            cd += GetCaster()->GetAttackTime(BASE_ATTACK);
        if (GetCaster()->CanUseAttackType(OFF_ATTACK))
            cd += GetCaster()->GetAttackTime(OFF_ATTACK);
        GetSpell()->SetRuneCooldown(cd);
        GetSpell()->SetRuneCost(1);

        if (auto aura = GetCaster()->GetAura(ELKS(COMBO_COUNT)))
            comboLength = aura->GetStackAmount();
    }

    virtual void AttackUnique()
    {

    }
    void AttackHit()
    {
        GetCaster()->CastSpell(GetCaster(), ELKS(SPIN_ATTACK_HIT), false);
        AttackUnique();
    }
    virtual void Register()
    {
        BeforeSpellLoad += SpellCastFn(spell_elk_spin_attack::SpellClick);
        AfterFullChannel += SpellCastFn(spell_elk_spin_attack::AttackHit);
    }

};

class spell_extension_system : public AuraScript
{
    PrepareAuraScript(spell_extension_system);


    std::vector<std::pair<uint32, const ExtensionObj*>> CurrentExtensions = {};
    std::map<ObjectGuid, bool> LastLaugh = {};

    void SpellCast(Spell* spell)
    {
        const uint32 id = spell->GetSpellInfo()->Id;
        ComboMap::const_iterator it = Extensions.find(id);
        if (it == Extensions.end())
            return;

        bool uniqueFound = false;
        if (it->second.Unique == true)
        {
            for (std::pair<uint32, const ExtensionObj*> obj : CurrentExtensions)
            {
                if (obj.first == it->first)
                {
                    uniqueFound = true;
                    break;
                }
            }
        }
        if (!uniqueFound)
        {
            std::pair<uint32, const ExtensionObj*> extension = std::make_pair(it->first, &it->second);
            CurrentExtensions.emplace_back(extension);
            sort(CurrentExtensions.begin(), CurrentExtensions.end(),
                [](const std::pair<uint32, const ExtensionObj*>& first, const std::pair<uint32, const ExtensionObj*>& second)
                {
                    return first.second->Cost < second.second->Cost;
                }
            );
        }
        int32 mana;
        GetAura()->RefreshDuration();
        for (auto ext : CurrentExtensions)
        {
            mana = GetUnitOwner()->GetCreateMana();

            mana = int32(CalculatePct(mana, ext.second->Cost));
            int32 currMana = GetUnitOwner()->GetPower(POWER_MANA);
            if (currMana < mana)
                return;
            GetUnitOwner()->SetPower(POWER_MANA, currMana - mana);
            (*ext.second->Function)(this, spell);
        }
    }
    void IsCasting(AuraEffect const* aurEff)
    {
        if (!GetUnitOwner()->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            return;
        GetAura()->SetDuration(GetAura()->GetDuration() + 100);
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (!GetUnitOwner()->IsInCombat())
            return;
        uint16 totalCost = 0;
        for (auto ext : CurrentExtensions)
        {
            totalCost += ext.second->Cost;
        }
        if (totalCost == 0)
            return;
        int16 dividedCost = float(totalCost) / LastLaugh.size();
        for (auto ll : LastLaugh)
        {
            if (ll.second)
            {
                GetUnitOwner()->HandleStatModifier(CurrentExtensions.back().second->Mod, TOTAL_VALUE, GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second, false);

                GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second += dividedCost;
                if (GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second > GetUnitOwner()->GetLevel() * 100)
                    GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second = GetUnitOwner()->GetLevel() * 100;
                if (GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second < GetUnitOwner()->GetLevel() * -100)
                    GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second = GetUnitOwner()->GetLevel() * -100;

                GetUnitOwner()->HandleStatModifier(CurrentExtensions.back().second->Mod, TOTAL_VALUE, GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second, true);
            }
            else
            {
                GetUnitOwner()->HandleStatModifier(CurrentExtensions.back().second->Mod, TOTAL_VALUE, GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second, false);

                GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second -= dividedCost;
                if (GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second > GetUnitOwner()->GetLevel() * 100)
                    GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second = GetUnitOwner()->GetLevel() * 100;
                if (GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second < GetUnitOwner()->GetLevel() * -100)
                    GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second = GetUnitOwner()->GetLevel() * -100;

                GetUnitOwner()->HandleStatModifier(CurrentExtensions.back().second->Mod, TOTAL_VALUE, GetUnitOwner()->ExtensionSpellPower[CurrentExtensions.back().first].second, true);
            }
        }
    }
    void OnDamage(AuraEffect const* aurEff, ProcEventInfo& procInfo)
    {
        ObjectGuid victim = procInfo.GetDamageInfo()->GetVictim()->GetGUID();
        if (victim != GetUnitOwner()->GetGUID())
        {
            LastLaugh[victim] = true;
        }
        else
        {
            victim = procInfo.GetDamageInfo()->GetAttacker()->GetGUID();
            if (victim != GetUnitOwner()->GetGUID())
                LastLaugh[victim] = false;
        }
    }
    void Register() override
    {
        OnSpellCast += OnSpellCastFn(spell_extension_system::SpellCast);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_extension_system::IsCasting, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectRemove += AuraEffectRemoveFn(spell_extension_system::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_DEFAULT);
        OnEffectProc += AuraEffectProcFn(spell_extension_system::OnDamage, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
public:

    inline static ComboMap Extensions = {};
};


class spell_combo_counter_aura : public AuraScript
{
    PrepareAuraScript(spell_combo_counter_aura);

    uint32 dur = 0;
    uint8 combo = 0;
    void PreSpellCast(Spell* spell)
    {
        if (GetCaster()->HasUnitState(UNIT_STATE_CASTING) && dur == 0)
        {
            dur = GetAura()->GetDuration();
            combo = GetAura()->GetStackAmount();
            GetAura()->SetOverrideDuration(100000);
        }
    }
    void PostSpellCast(Spell* spell)
    {
        if (!GetCaster()->HasUnitState(UNIT_STATE_CASTING) && dur > 0)
        {
            if (GetAura()->GetStackAmount() > combo)
                GetAura()->SetDuration(GetAura()->GetSpellInfo()->GetMaxDuration());
            else
                GetAura()->SetDuration(dur);
            dur = 0;
        }
    }
    void Register() override
    {
        OnSpellCast += OnSpellCastFn(spell_combo_counter_aura::PreSpellCast);
        OnSpellCast += OnSpellCastFn(spell_combo_counter_aura::PostSpellCast);
    }
public:

    inline static ComboMap Extensions = {};
};
#define CRITICAL_ANIMATION_SEARCH_BLOCK { if (GetCaster()->IsPlayer())\
{ \
Player * player = GetCaster()->ToPlayer(); \
if (player) \
{ \
int weps = 0; \
int anim = RandomInt(1, 4); \
int animSpell = ELKS(ATTACKUNARMED); \
auto wep1 = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND); \
auto wep2 = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND); \
std::vector<const ItemTemplate*> weapons = { nullptr, nullptr }; \
if (wep1) \
weapons[0] = wep1->GetTemplate(); \
if (wep2) \
weapons[1] = wep2->GetTemplate(); \
\
animSpell = ELKS(CRITICAL_ATTACKUNARMED); \
if (weapons[0]) \
{ \
weps += 1; \
\
switch (weapons[0]->GetSkill()) \
{ \
case SKILL_AXES: \
case SKILL_SWORDS: \
case SKILL_MACES: \
    animSpell = ELKS(CRITICAL_ATTACK1H); \
    break; \
case SKILL_FISHING: \
case SKILL_2H_AXES: \
case SKILL_2H_SWORDS: \
case SKILL_2H_MACES: \
    animSpell = ELKS(CRITICAL_ATTACK2H); \
    break; \
case SKILL_POLEARMS: \
case SKILL_STAVES: \
    animSpell = ELKS(CRITICAL_ATTACK2H); \
    break; \
case SKILL_DAGGERS: \
    animSpell = ELKS(CRITICAL_ATTACKMUTILATE); \
    break; \
case SKILL_SHIELD: \
    animSpell = ELKS(CRITICAL_ATTACK1H); \
    break; \
case SKILL_FIST_WEAPONS: \
default: \
    animSpell = ELKS(CRITICAL_ATTACKUNARMED); \
    break; \
}\
\
}\
\
if (weps == 0) \
GetSpell()->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(ELKS(ATTACKUNARMED))); \
else \
GetSpell()->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(animSpell)); \
\
} \
} \
else \
{ \
Creature * creature = GetCaster()->ToCreature(); \
if (creature) \
{\
\
int weps = 0; \
int anim = RandomInt(1, 4); \
int animSpell = ELKS(ATTACKUNARMED); \
auto weapons = creature->GetWeaponEquippedEntry(); \
\
animSpell = ELKS(CRITICAL_ATTACKUNARMED); \
if (weapons[0]) \
{ \
weps += 1; \
\
switch (weapons[0]->GetSkill()) \
{ \
case SKILL_AXES: \
case SKILL_SWORDS: \
case SKILL_MACES: \
    animSpell = ELKS(CRITICAL_ATTACK1H); \
    break; \
case SKILL_FISHING: \
case SKILL_2H_AXES: \
case SKILL_2H_SWORDS: \
case SKILL_2H_MACES: \
    animSpell = ELKS(CRITICAL_ATTACK2H); \
    break; \
case SKILL_POLEARMS: \
case SKILL_STAVES: \
    animSpell = ELKS(CRITICAL_ATTACK2H); \
    break; \
case SKILL_DAGGERS: \
    animSpell = ELKS(CRITICAL_ATTACKMUTILATE); \
    break; \
case SKILL_SHIELD: \
    animSpell = ELKS(CRITICAL_ATTACK1H); \
    break; \
case SKILL_FIST_WEAPONS: \
default: \
    animSpell = ELKS(CRITICAL_ATTACKUNARMED); \
    break; \
}\
\
}\
\
GetSpell()->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(animSpell)); \
\
} \
} \
}


#define CRITICAL_ATTACK_HIT_BLOCK(x_class) bool WasInAir = false; \
uint32 strike = 0; \
uint8 targetsHit = 0; \
void SpellBegin() \
{ \
    AttackBegin(); \
} \
void SpellHit() \
{ \
    GetCaster()->ModifyPower(POWER_MANA, GetCaster()->GetStat(STAT_SPIRIT)); \
    if (WasInAir) \
    { \
        CustomSpellValues values; \
        values.AddSpellMod(SPELLVALUE_AURA_DURATION, 2000); \
        GetCaster()->CastCustomSpell(ELKS(ATTACK_SLOW_DEBUFF), values, GetCaster(), TRIGGERED_FULL_MASK); \
    } \
    else \
    { \
        GetCaster()->CastSpell(GetHitUnit(), ELKS(ATTACK_SLOW_DEBUFF), true); \
    } \
     \
    targetsHit++; \
    DamageComponent(); \
} \
void SpellFinish() \
{ \
    if (!(GetCaster())) \
        return; \
     \
    AfterAttack(); \
     \
     \
    if (targetsHit) \
    { \
        Aura* aura = GetCaster()->GetAura(ELKS(COMBO_COUNT)); \
        if (!aura) \
            GetCaster()->AddAura(ELKS(COMBO_COUNT), GetCaster()); \
        else \
            aura->SetStackAmount(aura->GetStackAmount() + 1); \
    } \
} \
void Register() override \
{ \
    BeforeCast += SpellCastFn(x_class::SpellBegin); \
    AfterCast += SpellCastFn(x_class::SpellFinish); \
    AfterHit += SpellHitFn(x_class::SpellHit); \
} 

