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

struct ExtensionObj
{
    ExtensionObj(void(*Function)(spell_extension_system*, Spell*), int32 Cost, bool Unique) : Function(Function), Cost(Cost), Unique(Unique) {};
    void(*Function)(spell_extension_system*, Spell*);
    int32 Cost;
    bool Unique;
};

#define ComboMap std::map<uint32, const ExtensionObj>
#define AddExtension(x, y) spell_extension_system::Extensions.emplace(uint32(x), y)

enum class MapDummy : uint8;

inline uint32 RandomInt(uint32 min, uint32 max)
{
    uint32 ret = rand32() % max;
    return std::min(ret, min);
}
enum OutsideSpells
{

};
 enum Spells : uint32
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
};
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
enum SpellsStrike : uint32
{
    SPELL_PLAGUE_SCOURGE_STRIKE =       200001,
    SPELL_BLOOD_DEATH_STRIKE =          200002,
    SPELL_FROST_STRIKE_OBLITERATE =     200003,

    SPELL_CRUSADER_STRIKE =             210001,
    SPELL_CRUSADER_STRIKE_BASIC =       210017,
    SPELL_CRUSADER_STRIKE_EXPERT =      210030,
};

enum SpellsELKPaladin : uint32
{
    SPELL_RETRIBUTION_AURA = 210002,
    SPELL_RETRIBUTION_AURA_2 = 210003,
    SPELL_HOLY_POWER = 210004,

    SUBSPELL_LONG_RETRIBUTION = 219999,
};
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
        if(!aura)
            aura = GetCaster()->AddAura(1000000, GetCaster());
        aura->RefreshDuration();
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

class spell_elk_strike_aura : public AuraScript
{

    PrepareAuraScript(spell_elk_strike_aura);

    void SpellApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        Unit* target= GetTarget();
        if (!target)
            return;
        target->SetStrikeAura(GetSpellInfo()->Id);
        target->RemoveAura(COMBO_COUNT);
    }
    void SpellRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        Unit* target = GetTarget();
        if (!target)
            return;
        target->SetStrikeAura(0);
        target->RemoveAura(COMBO_COUNT);
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_elk_strike_aura::SpellApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_elk_strike_aura::SpellRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_elk_attack : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_attack);
    Unit* caster;
    Spell* spell;

    uint8 comboLength = 0;


    void SpellClick()
    {
        spell = GetSpell();
        caster = GetCaster();
        Spell* curAtk = caster->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == ATTACK)
            {
                if (curAtk->GetSpellTimer() < 50)
                {
                    spell->skip = true;
                    GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 100, false);
                    QueSpell(caster);
                    return;
                }
                else
                {
                    spell->skip = true;
                    GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 100, false);
                    return;
                }
            }
        }
        else
        {
            curAtk = caster->GetCurrentSpell(CURRENT_GENERIC_SPELL);
            if (curAtk)
            {
                if (curAtk->m_spellInfo->Id == ATTACK_HIT && curAtk->GetSpellTimer() > 0)
                {
                    spell->skip = true;
                    GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 1, false);
                    QueSpell(caster);
                    return;
                }
            }
        }
        if (caster->IsPlayer())
        {
            Player* player = caster->ToPlayer();
            if (player)
            {
                int weps = 0;
                int anim = RandomInt(1, 4);
                int animSpell = ATTACKUNARMED;
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
                        animSpell = ATTACK1H;
                        break;
                    case SKILL_FISHING:
                    case SKILL_2H_AXES:
                    case SKILL_2H_SWORDS:
                    case SKILL_2H_MACES:
                        animSpell = ATTACK;
                        break;
                    case SKILL_POLEARMS:
                    case SKILL_STAVES:
                        animSpell = ATTACK2HL;
                        break;
                    case SKILL_DAGGERS:
                        animSpell = ATTACK1HP;
                        break;
                    case SKILL_SHIELD:
                        animSpell = ATTACKSHIELD;
                        break;
                    case SKILL_FIST_WEAPONS:
                    default:
                        animSpell = ATTACKUNARMED;
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
                        animSpell = ATTACKOFF;
                        break;
                    case SKILL_FISHING:
                    case SKILL_2H_AXES:
                    case SKILL_2H_SWORDS:
                    case SKILL_2H_MACES:
                        animSpell = ATTACKOFF;
                        break;
                    case SKILL_POLEARMS:
                    case SKILL_STAVES:
                        animSpell = ATTACKOFFP;
                        break;
                    case SKILL_DAGGERS:
                        animSpell = ATTACKOFFP;
                        break;
                    case SKILL_SHIELD:
                        animSpell = ATTACKSHIELD;
                        break;
                    case SKILL_FIST_WEAPONS:
                    default:
                        animSpell = ATTACKUNARMED;
                        break;
                    }
                } 
                if (weps == 0)
                    spell->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(ATTACKUNARMED));
                else
                    spell->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(animSpell));

            }
        }
        else
        {
            Creature* creature = caster->ToCreature();
            if (creature)
            {

                int weps = 0;
                int anim = RandomInt(1, 4);
                int animSpell = ATTACKUNARMED;
                auto weapons = creature->GetWeaponEquippedEntry();
                 
                if (weapons[0])
                {
                    weps += 1;

                    switch (weapons[0]->GetSkill())
                    {
                    case SKILL_AXES:
                    case SKILL_SWORDS:
                    case SKILL_MACES:
                        animSpell = ATTACK1H;
                    case SKILL_FISHING:
                    case SKILL_2H_AXES:
                    case SKILL_2H_SWORDS:
                    case SKILL_2H_MACES:
                        animSpell = ATTACK;
                    case SKILL_POLEARMS:
                    case SKILL_STAVES:
                        animSpell = ATTACK2HL;
                    case SKILL_DAGGERS:
                        animSpell = ATTACK1HP;
                    case SKILL_SHIELD:
                        animSpell = ATTACKSHIELD;
                    case SKILL_FIST_WEAPONS:
                    default:
                        animSpell = ATTACKUNARMED;
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
                        animSpell = ATTACKOFF;
                    case SKILL_FISHING:
                    case SKILL_2H_AXES:
                    case SKILL_2H_SWORDS:
                    case SKILL_2H_MACES:
                        animSpell = ATTACKOFF;
                    case SKILL_POLEARMS:
                    case SKILL_STAVES:
                        animSpell = ATTACKOFFP;
                    case SKILL_DAGGERS:
                        animSpell = ATTACKOFFP;
                    case SKILL_SHIELD:
                        animSpell = ATTACKSHIELD;
                    case SKILL_FIST_WEAPONS:
                    default:
                        animSpell = ATTACKUNARMED;
                    }
                } 
                spell->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(animSpell));

            }
        }
        uint16 cd = 0;
        if(caster->CanUseAttackType(BASE_ATTACK))
            cd += caster->GetAttackTime(BASE_ATTACK);
        if (caster->CanUseAttackType(OFF_ATTACK))
            cd += caster->GetAttackTime(OFF_ATTACK);
        spell->SetRuneCooldown(cd);
        spell->SetRuneCost(1);

        if (auto aura = caster->GetAura(COMBO_COUNT))
            comboLength = aura->GetStackAmount();
    }
    void SpellBegin()
    {
    }
    virtual void AttackUnique()
    {

    }
    void AttackHit()
    {
        uint32 hitSpell = ATTACK_HIT;
        caster->CastSpell(caster, hitSpell, false);
        AttackUnique();

        auto curAtk = caster->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == hitSpell)
            {
                curAtk->GetTriggerDummy()[MapDummy::ComboLength] = comboLength;
                return;
            }
        }
    }
    virtual void Register()
    {
        BeforeSpellLoad += SpellCastFn(spell_elk_attack::SpellClick);
        BeforeCast += SpellCastFn(spell_elk_attack::SpellBegin);
        AfterFullChannel += SpellCastFn(spell_elk_attack::AttackHit);
    }

};

class spell_elk_critical_attack : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_critical_attack);
    Unit* caster;
    Spell* spell;

    uint8 comboLength = 0; 



    void SpellClick()
    {
        spell = GetSpell();
        caster = GetCaster();
        if (caster->GetCritTempo() > 100)
            caster->ModCritTempo(-100);
        else
        {
            spell->skip = true;
            GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 100, false);
            return;
        }

        Spell* curAtk = caster->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == ATTACK)
            {
                if (curAtk->GetSpellTimer() < 50)
                {
                    spell->skip = true;
                    GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 100, false);
                    QueSpell(caster);
                    return;
                }
                else
                {
                    spell->skip = true;
                    GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 100, false);
                    return;
                }
            }
        }
        else
        {
            curAtk = caster->GetCurrentSpell(CURRENT_GENERIC_SPELL);
            if (curAtk)
            {
                if (curAtk->m_spellInfo->Id == ATTACK_HIT && curAtk->GetSpellTimer() > 0)
                {
                    spell->skip = true;
                    GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 1, false);
                    QueSpell(caster);
                    return;
                }
            }
        }
        if (caster->IsPlayer())
        {
            Player* player = caster->ToPlayer();
            if (player)
            {
                int weps = 0;
                int anim = RandomInt(1, 4);
                int animSpell = ATTACKUNARMED;
                auto wep1 = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                auto wep2 = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
                std::vector<const ItemTemplate*> weapons = { nullptr, nullptr };
                if (wep1)
                    weapons[0] = wep1->GetTemplate();
                if (wep2)
                    weapons[1] = wep2->GetTemplate();

                animSpell = CRITICAL_ATTACKUNARMED;
                if (weapons[0])
                {
                    weps += 1;

                    switch (weapons[0]->GetSkill())
                    {
                    case SKILL_AXES:
                    case SKILL_SWORDS:
                    case SKILL_MACES:
                        animSpell = CRITICAL_ATTACK1H;
                        break;
                    case SKILL_FISHING:
                    case SKILL_2H_AXES:
                    case SKILL_2H_SWORDS:
                    case SKILL_2H_MACES:
                        animSpell = CRITICAL_ATTACK2H;
                        break;
                    case SKILL_POLEARMS:
                    case SKILL_STAVES:
                        animSpell = CRITICAL_ATTACK2H;
                        break;
                    case SKILL_DAGGERS:
                        animSpell = CRITICAL_ATTACKMUTILATE;
                        break;
                    case SKILL_SHIELD:
                        animSpell = CRITICAL_ATTACK1H;
                        break;
                    case SKILL_FIST_WEAPONS:
                    default:
                        animSpell = CRITICAL_ATTACKUNARMED;
                        break;
                    }

                }

                if (weps == 0)
                    spell->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(ATTACKUNARMED));
                else
                    spell->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(animSpell));

            }
        }
        else
        {
            Creature* creature = caster->ToCreature();
            if (creature)
            {

                int weps = 0;
                int anim = RandomInt(1, 4);
                int animSpell = ATTACKUNARMED;
                auto weapons = creature->GetWeaponEquippedEntry();
                 
                animSpell = CRITICAL_ATTACKUNARMED;
                if (weapons[0])
                {
                    weps += 1;

                    switch (weapons[0]->GetSkill())
                    {
                    case SKILL_AXES:
                    case SKILL_SWORDS:
                    case SKILL_MACES:
                        animSpell = CRITICAL_ATTACK1H;
                        break;
                    case SKILL_FISHING:
                    case SKILL_2H_AXES:
                    case SKILL_2H_SWORDS:
                    case SKILL_2H_MACES:
                        animSpell = CRITICAL_ATTACK2H;
                        break;
                    case SKILL_POLEARMS:
                    case SKILL_STAVES:
                        animSpell = CRITICAL_ATTACK2H;
                        break;
                    case SKILL_DAGGERS:
                        animSpell = CRITICAL_ATTACKMUTILATE;
                        break;
                    case SKILL_SHIELD:
                        animSpell = CRITICAL_ATTACK1H;
                        break;
                    case SKILL_FIST_WEAPONS:
                    default:
                        animSpell = CRITICAL_ATTACKUNARMED;
                        break;
                    }

                }
                
                spell->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(animSpell));

            }
        }
        uint16 cd = 0;
        if (caster->CanUseAttackType(BASE_ATTACK))
            cd += caster->GetAttackTime(BASE_ATTACK);
        if (caster->CanUseAttackType(OFF_ATTACK))
            cd += caster->GetAttackTime(OFF_ATTACK);
        spell->SetRuneCooldown(cd);
        spell->SetRuneCost(1);

        if (auto aura = caster->GetAura(COMBO_COUNT))
            comboLength = aura->GetStackAmount();
    }
    void SpellBegin()
    {
    }
    virtual void AttackUnique()
    {

    }
    void AttackHit()
    {
        uint32 hitSpell = CRITICAL_ATTACK_HIT;
        caster->CastSpell(caster, hitSpell, false);
        AttackUnique();

        auto curAtk = caster->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == hitSpell)
            {
                curAtk->GetTriggerDummy()[MapDummy::ComboLength] = comboLength;
                return;
            }
        }
    }
    virtual void Register()
    {
        BeforeSpellLoad += SpellCastFn(spell_elk_critical_attack::SpellClick);
        BeforeCast += SpellCastFn(spell_elk_critical_attack::SpellBegin);
        AfterFullChannel += SpellCastFn(spell_elk_critical_attack::AttackHit);
    }

};


class spell_elk_spin_attack : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_spin_attack);
    Unit* caster;
    Spell* spell;

    uint8 comboLength = 0;



    void SpellClick()
    {
        spell = GetSpell();
        caster = GetCaster();

        Spell* curAtk = caster->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == ATTACK)
            {
                if (curAtk->GetSpellTimer() < 50)
                {
                    spell->skip = true;
                    GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 100, false);
                    QueSpell(caster);
                    return;
                }
                else
                {
                    spell->skip = true;
                    GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 100, false);
                    return;
                }
            }
        }
        else
        {
            curAtk = caster->GetCurrentSpell(CURRENT_GENERIC_SPELL);
            if (curAtk)
            {
                if (curAtk->m_spellInfo->Id == ATTACK_HIT && curAtk->GetSpellTimer() > 0)
                {
                    spell->skip = true;
                    GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 1, false);
                    QueSpell(caster);
                    return;
                }
            }
        }

        uint16 cd = 0;
        if (caster->CanUseAttackType(BASE_ATTACK))
            cd += caster->GetAttackTime(BASE_ATTACK);
        if (caster->CanUseAttackType(OFF_ATTACK))
            cd += caster->GetAttackTime(OFF_ATTACK);
        spell->SetRuneCooldown(cd);
        spell->SetRuneCost(1);

        if (auto aura = caster->GetAura(COMBO_COUNT))
            comboLength = aura->GetStackAmount();
    }
    void SpellBegin()
    {
    }
    virtual void AttackUnique()
    {

    }
    void AttackHit()
    {
        uint32 hitSpell = SPIN_ATTACK_HIT;
        caster->CastSpell(caster, hitSpell, false);
        AttackUnique();

        auto curAtk = caster->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == hitSpell)
            {
                curAtk->GetTriggerDummy()[MapDummy::ComboLength] = comboLength;
                return;
            }
        }
    }
    virtual void Register()
    {
        BeforeSpellLoad += SpellCastFn(spell_elk_spin_attack::SpellClick);
        BeforeCast += SpellCastFn(spell_elk_spin_attack::SpellBegin);
        AfterFullChannel += SpellCastFn(spell_elk_spin_attack::AttackHit);
    }

};

class spell_extension_system : public AuraScript
{
    PrepareAuraScript(spell_extension_system);


    std::vector<std::pair<uint32, const ExtensionObj*>> CurrentExtensions = {};

    void SpellCast(Spell* spell)
    {
        const uint32 id = spell->GetSpellInfo()->Id;
        ComboMap::const_iterator it = Extensions.find(id);
        if (it == Extensions.end())
            return;

        if (it->second.Unique == true)
        {
            for (std::pair<uint32, const ExtensionObj*> obj : CurrentExtensions)
            {
                if (obj.first == it->first)
                    return;
            }
        }
        std::pair<uint32, const ExtensionObj*> extension = std::make_pair(it->first, &it->second);
        int32 mana;
        CurrentExtensions.emplace_back(extension);
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
        if (GetUnitOwner()->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            return;
        GetAura()->SetDuration(GetAura()->GetDuration() + 100);
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {

    }
    std::vector<std::pair<GUID, bool>> LastLaugh = {};
    void OnDamage(AuraEffect const* aurEff, ProcEventInfo& procInfo)
    {
        uint32 victim = procInfo.GetDamageInfo()->GetVictim()->GetGUID();
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

