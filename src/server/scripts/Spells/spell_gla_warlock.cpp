/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

 /*
  * Scripts for spells with SPELLFAMILY_ROGUE and SPELLFAMILY_GENERIC spells used by rogue players.
  * Ordered alphabetically using scriptname.
  * Scriptnames of files in this file should be prefixed with "spell_rog_".
  */

#include "CellImpl.h"
#include "GridNotifiers.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"

enum GlaWarlockSpells
{
    SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON     = 48018,

    SPELL_WARLOCK_FINGER_OF_DEATH           = 110001,
    SPELL_WARLOCK_FINGER_OF_DEATH_AURA      = 110002,
    SPELL_WARLOCK_BANISH_AURA               = 110005,
    SPELL_WARLOCK_BANISH_AURA_DUMMY         = 110006,
    SPELL_WARLOCK_BANE_OF_HAVOC             = 110007,
    SPELL_WARLOCK_BANE_OF_DESPAIR           = 110018,
    SPELL_WARLOCK_AGONY                     = 110014,
    SPELL_WARLOCK_DRAIN_LIFE                = 110013,
    SPELL_WARLOCK_DRAIN_LIFE_ENEMY          = 110015,
    SPELL_WARLOCK_DRAIN_LIFE_ALLY           = 110016,
    SPELL_WARLOCK_RITUAL_STRIKE             = 110019,
    SPELL_WARLOCK_CURSE_OF_IMPRUDENCE       = 110020,
    SPELL_WARLOCK_WITHER                    = 110022,
    SPELL_WARLOCK_RITUAL_OF_TOWER           = 110027,
    SPELL_WARLOCK_DRAIN_SOUL_SLOW           = 110029,
    SPELL_WARLOCK_SOUL_FRAGMENTS_HIT        = 110031,
    SPELL_WARLOCK_DEVOUR_MAGIC              = 110033,
    SPELL_WARLOCK_CRIPPLE                   = 110034,
    SPELL_WARLOCK_BANE_OF_PORTALS           = 110058,
    SPELL_WARLOCK_BANE_OF_PORTALS_DUMMY     = 110059,
    SPELL_WARLOCK_DOOM                      = 110060,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT       = 110062,
    SPELL_WARLOCK_DEMONIC_RETREAT           = 110063,
    SPELL_WARLOCK_DEMONIC_FOCUS             = 110064,
    SPELL_WARLOCK_DEMONIC_SACRIFICE         = 110070,
    SPELL_WARLOCK_IMPS_TRICKERY             = 110071,
    SPELL_WARLOCK_VOIDWALKERS_REGENERATION  = 110072,
    SPELL_WARLOCK_SUCCUBUS_ALLURE           = 110073,
    SPELL_WARLOCK_FELHUNTERS_HUNGER         = 110074,
    SPELL_WARLOCK_FELGUARDS_PERSISTENCE     = 110075,
    SPELL_WARLOCK_DEMONIC_CIRCLE_IMPRISON   = 110076,
    SPELL_WARLOCK_DEMONIC_CIRCLE_DEMONMPORT = 110077,
    SPELL_WARLOCK_BLOOD_BOIL                = 110078,
    SPELL_WARLOCK_BLOOD_BOIL_DUMMY          = 110079,
    SPELL_WARLOCK_HARDENED_SKIN             = 110087,
    SPELL_WARLOCK_ENRAGE                    = 110090,
    SPELL_WARLOCK_KILL_ME                   = 110091,
    SPELL_WARLOCK_SHIVARRAS_WRATH           = 110092,
    SPELL_WARLOCK_BEHOLDERS_CRUELTY         = 110093,
    SPELL_WARLOCK_TYRANTS_VITALITY          = 110094,
    SPELL_WARLOCK_VILEFIENDS_ALACRITY       = 110095,
    SPELL_WARLOCK_BANE_OF_DESPAIR_WARRIOR   = 110096,
    SPELL_WARLOCK_BANE_OF_DESPAIR_PALADIN   = 110097,
    SPELL_WARLOCK_BANE_OF_DESPAIR_HUNTER    = 110098,
    SPELL_WARLOCK_BANE_OF_DESPAIR_ROGUE     = 110099,
    SPELL_WARLOCK_BANE_OF_DESPAIR_PRIEST    = 110100,
    SPELL_WARLOCK_BANE_OF_DESPAIR_DEATH_KNIGHT  = 110101,
    SPELL_WARLOCK_BANE_OF_DESPAIR_SHAMAN    = 110102,
    SPELL_WARLOCK_BANE_OF_DESPAIR_MAGE      = 110103,
    SPELL_WARLOCK_BANE_OF_DESPAIR_WARLOCK   = 110104,
    SPELL_WARLOCK_BANE_OF_DESPAIR_DRUID     = 110105,

};
enum GlaWarlockUnits
{
    IMP                                     = 416,
    VOIDWALKER                              = 1860,
    SUCCUBUS                                = 1863,
    FELHUNTER                               = 417,
    FELGUARD                                = 17252,
    WARLOCK_TOWER                           = 1000001,
    SHIVARRA                                = 1000002,
    BEHOLDER                                = 1000003,
    TYRANT                                  = 1000004,
    VILEFIEND                               = 1000005,
};

class spell_gla_feral_demon : public AuraScript
{
    PrepareAuraScript(spell_gla_feral_demon);
    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_KILL_ME, SPELL_WARLOCK_SHIVARRAS_WRATH,
            SPELL_WARLOCK_BEHOLDERS_CRUELTY, SPELL_WARLOCK_TYRANTS_VITALITY, SPELL_WARLOCK_VILEFIENDS_ALACRITY
            });
    }
    void Proc(ProcEventInfo& eventInfo)
    {
        switch (eventInfo.GetActor()->GetEntry())
        {
        case SHIVARRA:
            eventInfo.GetActor()->AddAura(SPELL_WARLOCK_SHIVARRAS_WRATH, eventInfo.GetActor());
            break;
        case BEHOLDER:
            eventInfo.GetActor()->AddAura(SPELL_WARLOCK_BEHOLDERS_CRUELTY, eventInfo.GetActor());
            break;
        case TYRANT:
            eventInfo.GetActor()->AddAura(SPELL_WARLOCK_TYRANTS_VITALITY, eventInfo.GetActor());
            break;
        case VILEFIEND:
            eventInfo.GetActor()->AddAura(SPELL_WARLOCK_VILEFIENDS_ALACRITY, eventInfo.GetActor());
            break;
        default:
            break;
        }
    }
    void Register() override
    {
        OnProc += AuraProcFn(spell_gla_feral_demon::Proc);
    }
};
class spell_gla_enrage : public AuraScript
{
    PrepareAuraScript(spell_gla_enrage);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_ENRAGE });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = -1;
    }

    void Absorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        int32 remainingHp = int32(GetTarget()->GetHealth() - dmgInfo.GetDamage());
        int32 minHp = int32(GetTarget()->CountPctFromMaxHealth(GetAura()->GetEffect(EFFECT_1)->CalculateAmount(GetCaster())));

        if (remainingHp < minHp)
            GetAura()->GetEffect(EFFECT_2)->SetAmount(GetAura()->GetEffect(EFFECT_1)->CalculateAmount(GetCaster()));
        else
            GetAura()->GetEffect(EFFECT_2)->SetAmount(0);
        absorbAmount = 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gla_enrage::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_gla_enrage::Absorb, EFFECT_0);
    }
};

class spell_gla_hardened_skin : public AuraScript
{
    PrepareAuraScript(spell_gla_hardened_skin);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_HARDENED_SKIN });
    }

    uint32 absorbPct;

    bool Load() override
    {
        absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
        return true;
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = -1;
    }

    void Absorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        int32 remainingHp = int32(GetTarget()->GetHealth() - dmgInfo.GetDamage());
        int32 maxHp = int32(GetTarget()->CountPctFromMaxHealth(GetAura()->GetEffect(EFFECT_1)->CalculateAmount(GetCaster())));

        if (remainingHp > maxHp)
            absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gla_hardened_skin::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_gla_hardened_skin::Absorb, EFFECT_0);
    }
};

class spell_gla_blood_boil : public SpellScript
{
    PrepareSpellScript(spell_gla_blood_boil);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_BLOOD_BOIL, SPELL_WARLOCK_BLOOD_BOIL_DUMMY });
    }

    void Cast()
    {
        if (Unit* pet = GetCaster()->GetGuardianPet())
        {
            GetCaster()->DealDamage(GetCaster(), pet, (float)pet->GetMaxHealth() * float(GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster(), nullptr, pet)) / 100.f);
            pet->CastSpell(pet->GetPositionX(), pet->GetPositionY(), pet->GetPositionZ(), SPELL_WARLOCK_BLOOD_BOIL_DUMMY, true);
        }
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_gla_blood_boil::Cast);
    }
};

class spell_gla_demonic_circle_demonport : public AuraScript
{
    PrepareAuraScript(spell_gla_demonic_circle_demonport);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_DEMONIC_CIRCLE_DEMONMPORT, SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON });
    }

    void HandleTeleport(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (GameObject* circle = player->GetGameObject(SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON))
            {
                if (Unit* pet = player->GetGuardianPet())
                {
                    pet->NearTeleportTo(circle->GetPositionX(), circle->GetPositionY(), circle->GetPositionZ(), circle->GetOrientation(), false, false, false, true);
                    pet->RemoveAurasWithMechanic(1 << MECHANIC_SNARE);
                    HealInfo heal(player, pet, float(pet->GetMaxHealth()) * float(GetSpellInfo()->Effects[EFFECT_1].CalcValue(player, nullptr, pet)) / 100.f, GetSpellInfo(), GetSpellInfo()->GetSchoolMask());
                    pet->HealBySpell(heal, false);
                }
            }
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_gla_demonic_circle_demonport::HandleTeleport, EFFECT_0, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
    }
};
class spell_gla_demonic_circle_imprison : public AuraScript
{
    PrepareAuraScript(spell_gla_demonic_circle_imprison);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_DEMONIC_CIRCLE_IMPRISON, SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON });
    }

    void HandleTeleport(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (GameObject* circle = player->GetGameObject(SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON))
            {
                if (Unit* unit = GetTarget(); unit && unit->GetExactDist(circle->GetPosition()) <= (float)GetSpellInfo()->GetEffect(EFFECT_2).BasePoints)
                {
                    unit->NearTeleportTo(circle->GetPositionX(), circle->GetPositionY(), circle->GetPositionZ(), circle->GetOrientation(), false, false, false, true);
                    for (auto aura : unit->GetAppliedAuras())
                    {
                        for (uint8 i = EFFECT_0; i <= EFFECT_2; i++)
                        {
                            if (!aura.second->GetBase()->GetEffect(i))
                                continue;
                            if (aura.second->GetBase()->GetEffect(i)->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE
                                || aura.second->GetBase()->GetEffect(i)->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE_PERCENT
                                || aura.second->GetBase()->GetEffect(i)->GetAuraType() == SPELL_AURA_PERIODIC_LEECH)
                            {
                                aura.second->GetBase()->Remove();
                            }
                        }
                    }
                }
            }
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_gla_demonic_circle_imprison::HandleTeleport, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_gla_felgaurds_persistence : public AuraScript
{
    PrepareAuraScript(spell_gla_felgaurds_persistence);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_FELGUARDS_PERSISTENCE });
    }

    void Proc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetProcSpell())
        {
            for (auto effect : eventInfo.GetProcSpell()->m_spellInfo->Effects)
            {
                if (effect.ApplyAuraName == SPELL_AURA_MOD_SILENCE || effect.Mechanic != Mechanics::MECHANIC_NONE)
                {
                    Aura* aura = eventInfo.GetProcTarget()->GetAura(eventInfo.GetProcSpell()->m_spellInfo->Id);
                    if (aura)
                        aura->SetDuration(float(aura->GetDuration()) * float(GetAura()->GetEffect(EFFECT_0)->GetAmount()) / 100.f);
                    return;
                }
            }
        }
    }

    void Register() override
    {
        AfterProc += AuraProcFn(spell_gla_felgaurds_persistence::Proc);
    }
};

class spell_gla_demonic_sacrifice : public SpellScript
{
    PrepareSpellScript(spell_gla_demonic_sacrifice);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_DEMONIC_SACRIFICE, SPELL_WARLOCK_IMPS_TRICKERY, SPELL_WARLOCK_VOIDWALKERS_REGENERATION,
            SPELL_WARLOCK_SUCCUBUS_ALLURE, SPELL_WARLOCK_FELHUNTERS_HUNGER, SPELL_WARLOCK_FELGUARDS_PERSISTENCE, });
    }

    void Cast()
    {
        uint32 spell = 0;
        Unit* minion = GetCaster()->GetGuardianPet();
        if (!minion)
            return;
        switch (minion->GetEntry())
        {
        case IMP:
            spell = SPELL_WARLOCK_IMPS_TRICKERY;
            break;
        case VOIDWALKER:
            spell = SPELL_WARLOCK_VOIDWALKERS_REGENERATION;
            break;
        case SUCCUBUS:
            spell = SPELL_WARLOCK_SUCCUBUS_ALLURE;
            break;
            case FELHUNTER:
            spell = SPELL_WARLOCK_FELHUNTERS_HUNGER;
            break;
        case FELGUARD:
            spell = SPELL_WARLOCK_FELGUARDS_PERSISTENCE;
            break;
        default:
            break;
        }
        if (spell)
            GetCaster()->CastSpell(GetCaster(), spell, true);
        minion->Kill(GetCaster(), minion);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_gla_demonic_sacrifice::Cast);
    }
};

class spell_gla_demonic_enhancements : public SpellScript
{
    PrepareSpellScript(spell_gla_demonic_enhancements);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_DOOM });
    }

    SpellCastResult CheckCast()
    {
        if (GetCaster()->GetCurrentSpell(CURRENT_GENERIC_SPELL))
        {
            TriggerCastFlags flags = TRIGGERED_IGNORE_CAST_IN_PROGRESS;
            GetCaster()->CastSpell(GetCaster(), GetSpellInfo()->Id, flags);
        }
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_gla_demonic_enhancements::CheckCast);
    }
};

class spell_gla_soul_fragments_hit : public SpellScript
{
    PrepareSpellScript(spell_gla_soul_fragments_hit);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SOUL_FRAGMENTS_HIT, SPELL_WARLOCK_DRAIN_SOUL_SLOW });
    }
    void Hit(SpellMissInfo missInfo)
    {
        Aura* aura = GetHitUnit()->GetAura(SPELL_WARLOCK_DRAIN_SOUL_SLOW);
        if (!aura)
            return;
        SetHitDamage(float(GetHitDamage()) * (1.f + float(aura->GetStackAmount()) * float(aura->GetEffect(EFFECT_1)->GetAmount())));
    }


    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_gla_soul_fragments_hit::Hit);
    }
};
class spell_gla_bane_of_portals : public AuraScript
{
    PrepareAuraScript(spell_gla_bane_of_portals);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_BANE_OF_PORTALS, SPELL_WARLOCK_BANE_OF_PORTALS_DUMMY });
    }

    void Proc(ProcEventInfo& eventInfo)
    {
        if (Player* player = eventInfo.GetActor()->ToPlayer(); player)
        {
            if (eventInfo.GetProcSpell())
            {   
                uint32 id = eventInfo.GetProcSpell()->GetSpellInfo()->Id;
                player->ModifySpellCooldown(id, GetAura()->GetEffect(EFFECT_1)->GetAmplitude());
            }
        }
    }
    void Remove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetTarget() && GetUnitOwner())
            GetUnitOwner()->CastSpell(GetTarget(), SPELL_WARLOCK_BANE_OF_PORTALS_DUMMY, true);
    }


    void Register() override
    {
        AfterProc += AuraProcFn(spell_gla_bane_of_portals::Proc);
        AfterEffectRemove += AuraEffectRemoveFn(spell_gla_bane_of_portals::Remove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_gla_cripple : public AuraScript
{
    PrepareAuraScript(spell_gla_cripple);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_CRIPPLE });
    }

    bool tick = false;
    void Apply(AuraEffect const* aurEff)
    {
        if (tick)
        {
            int32 amount = GetAura()->GetEffect(EFFECT_1)->GetAmount();
            if (amount < 0)
            {
                amount += 10;
                GetAura()->GetEffect(EFFECT_1)->SetAmount(amount);
                GetAura()->GetEffect(EFFECT_2)->SetAmount(amount);
            }
        }
        tick = !tick;
    }
    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_gla_cripple::Apply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

class spell_gla_devour_magic : public SpellScript
{
    PrepareSpellScript(spell_gla_devour_magic);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_RITUAL_STRIKE });
    }

    void Hit(SpellMissInfo missInfo)
    {
        uint32 damage = GetHitDamage();
        Unit* unit = GetCaster();
        uint8 debuffs = 0;
        for (auto aura : GetHitUnit()->GetAppliedAuras())
        {
            if (aura.second->GetBase()->GetOwner() != unit
                || aura.second->GetBase()->GetSpellInfo()->GetSchoolMask() != SPELL_SCHOOL_MASK_SHADOW)
                continue;
            for (uint8 i = EFFECT_0; i <= EFFECT_2; i++)
            {
                if (!aura.second->GetBase()->GetEffect(i))
                    continue;
                if (aura.second->GetBase()->GetEffect(i)->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE
                    || aura.second->GetBase()->GetEffect(i)->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE_PERCENT
                    || aura.second->GetBase()->GetEffect(i)->GetAuraType() == SPELL_AURA_PERIODIC_LEECH)
                {
                    aura.second->GetBase()->Remove();
                    debuffs++;
                }
            }
        }
        SetHitDamage(damage * debuffs);
        HealInfo heal(unit, unit, float(unit->GetMaxHealth()) * float(GetSpellInfo()->Effects[EFFECT_1].CalcValue(unit, nullptr, GetHitUnit())) / 100.f, GetSpellInfo(), GetSpellInfo()->GetSchoolMask());
        unit->HealBySpell(heal, false);
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_gla_devour_magic::Hit);
    }
};

class spell_gla_ritual_of_tower : public SpellScript
{
    PrepareSpellScript(spell_gla_ritual_of_tower);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_RITUAL_OF_TOWER });
    }
    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        TempSummon* summon = GetCaster()->SummonCreature(WARLOCK_TOWER, GetCaster()->GetPosition(), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 100000, 0, sSummonPropertiesStore.LookupEntry(36070));
        summon->SetFaction(GetCaster()->GetFaction());
        summon->SetOwnerGUID(GetCaster()->GetGUID());
        GetCaster()->m_Controlled.insert(summon);
        summon->m_ControlledByPlayer = true;
        summon->SetUnitFlag(UNIT_FLAG_PLAYER_CONTROLLED);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gla_ritual_of_tower::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class spell_gla_wither : public SpellScript
{
    PrepareSpellScript(spell_gla_wither);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_RITUAL_STRIKE });
    }

    void Hit(SpellMissInfo missInfo)
    {
        float damage = float(GetHitUnit()->GetHealth() * float(GetSpellInfo()->GetEffect(EFFECT_0).BasePoints) / 100.f);
        float max = float(GetCaster()->GetMaxHealth() * float(GetSpellInfo()->GetEffect(EFFECT_1).BasePoints) / 100.f);
        if (damage > max)
            damage = max;
        SetHitDamage(damage);
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_gla_wither::Hit);
    }
};

class spell_gla_curse_of_imprudence : public AuraScript
{
    PrepareAuraScript(spell_gla_curse_of_imprudence);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_CURSE_OF_IMPRUDENCE });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = -1;
    }

    void Absorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        absorbAmount = 0;
        if (dmgInfo.GetAttacker() == GetUnitOwner() || dmgInfo.GetAttacker()->GetOwner() == GetUnitOwner())
            dmgInfo.ModifyDamage(float(dmgInfo.GetDamage()) * 1.f + float(GetAura()->GetEffect(EFFECT_0)->GetAmount()) / 100);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gla_curse_of_imprudence::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_gla_curse_of_imprudence::Absorb, EFFECT_0);
    }
};
class spell_gla_ritual_strike : public SpellScript
{
    PrepareSpellScript(spell_gla_ritual_strike);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_RITUAL_STRIKE });
    }

    void Hit(SpellMissInfo missInfo)
    {
        uint32 damage = GetHitDamage();
        Unit* unit = GetCaster();
        uint32 bonus = GetSpellInfo()->Effects[EFFECT_1].CalcValue(unit, nullptr, GetHitUnit());
        for (auto aura : GetHitUnit()->GetAppliedAuras())
        {
            if (aura.second->GetBase()->GetOwner() != unit)
                continue;
            for (uint8 i = EFFECT_0; i <= EFFECT_2; i++)
            {
                if (!aura.second->GetBase()->GetEffect(i))
                    continue;
                if (aura.second->GetBase()->GetEffect(i)->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE
                    || aura.second->GetBase()->GetEffect(i)->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE_PERCENT
                    || aura.second->GetBase()->GetEffect(i)->GetAuraType() == SPELL_AURA_PERIODIC_LEECH)
                    damage += bonus;
            }
        }
        SetHitDamage(damage);
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_gla_ritual_strike::Hit);
    }
};

class spell_gla_bane_of_despair : public AuraScript
{
    PrepareAuraScript(spell_gla_bane_of_despair);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_BANE_OF_DESPAIR,
        SPELL_WARLOCK_BANE_OF_DESPAIR_WARRIOR,
        SPELL_WARLOCK_BANE_OF_DESPAIR_PALADIN,
        SPELL_WARLOCK_BANE_OF_DESPAIR_HUNTER,
        SPELL_WARLOCK_BANE_OF_DESPAIR_ROGUE,
        SPELL_WARLOCK_BANE_OF_DESPAIR_PRIEST,
        SPELL_WARLOCK_BANE_OF_DESPAIR_DEATH_KNIGHT,
        SPELL_WARLOCK_BANE_OF_DESPAIR_SHAMAN,
        SPELL_WARLOCK_BANE_OF_DESPAIR_MAGE,
        SPELL_WARLOCK_BANE_OF_DESPAIR_WARLOCK,
        SPELL_WARLOCK_BANE_OF_DESPAIR_DRUID
            });
    }

    void Apply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        Aura* newAura;
        switch (GetTarget()->getClass())
        {
        case CLASS_WARRIOR:
            newAura = GetCaster()->AddAura(SPELL_WARLOCK_BANE_OF_DESPAIR_WARRIOR, GetTarget());
            break;
        case CLASS_PALADIN:
            newAura = GetCaster()->AddAura(SPELL_WARLOCK_BANE_OF_DESPAIR_PALADIN, GetTarget());
            break;
        case CLASS_HUNTER:
            newAura = GetCaster()->AddAura(SPELL_WARLOCK_BANE_OF_DESPAIR_HUNTER, GetTarget());
            break;
        case CLASS_ROGUE:
            newAura = GetCaster()->AddAura(SPELL_WARLOCK_BANE_OF_DESPAIR_ROGUE, GetTarget());
            break;
        case CLASS_PRIEST:
            newAura = GetCaster()->AddAura(SPELL_WARLOCK_BANE_OF_DESPAIR_PRIEST, GetTarget());
            break;
        case CLASS_DEATH_KNIGHT:
            newAura = GetCaster()->AddAura(SPELL_WARLOCK_BANE_OF_DESPAIR_DEATH_KNIGHT, GetTarget());
            break;
        case CLASS_SHAMAN:
            newAura = GetCaster()->AddAura(SPELL_WARLOCK_BANE_OF_DESPAIR_SHAMAN, GetTarget());
            break;
        case CLASS_MAGE:
            newAura = GetCaster()->AddAura(SPELL_WARLOCK_BANE_OF_DESPAIR_MAGE, GetTarget());
            break;
        case CLASS_WARLOCK:
            newAura = GetCaster()->AddAura(SPELL_WARLOCK_BANE_OF_DESPAIR_WARLOCK, GetTarget());
            break;
        case CLASS_DRUID:
            newAura = GetCaster()->AddAura(SPELL_WARLOCK_BANE_OF_DESPAIR_DRUID, GetTarget());
            break;
        default:
            return;
            break;
        }
        newAura->SetDuration(GetAura()->GetDuration());
        newAura->GetEffect(EFFECT_0)->SetAmount(GetAura()->GetEffect(EFFECT_0)->GetAmount());
        newAura->GetEffect(EFFECT_1)->SetAmount(GetAura()->GetEffect(EFFECT_1)->GetAmount());
        GetAura()->Remove();

    }
    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_gla_bane_of_despair::Apply, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);

    }
};

class spell_gla_drain_life : public SpellScript
{
    PrepareSpellScript(spell_gla_drain_life);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_DRAIN_LIFE, SPELL_WARLOCK_DRAIN_LIFE_ENEMY, SPELL_WARLOCK_DRAIN_LIFE_ALLY });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetHitUnit())
        {
            if (caster->IsFriendlyTo(target))
            {
                int32 bp1 = -1 * GetEffectValue();
                int32 bp2 = GetEffectValue();
                caster->CastCustomSpell(target, SPELL_WARLOCK_DRAIN_LIFE_ENEMY, &bp1, &bp2, nullptr, true);
            }
            else
            {
                caster->CastSpell(target, SPELL_WARLOCK_DRAIN_LIFE_ENEMY, false);
            }
        }
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetExplTargetUnit())
        {
            if (!caster->IsFriendlyTo(target) && !caster->isInFront(target))
                return SPELL_FAILED_UNIT_NOT_INFRONT;

            if (target->IsFriendlyTo(caster))
                return SPELL_FAILED_BAD_TARGETS;
        }
        else
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_gla_drain_life::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_gla_drain_life::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class spell_gla_agony : public AuraScript
{
    PrepareAuraScript(spell_gla_agony);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_AGONY });
    }

    float amount = 0;
    void Apply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        amount = float(GetAura()->GetEffect(EFFECT_0)->GetAmount());
        
    }
    void Calc(AuraEffect const* aurEff)
    {

        float tickMulti = float(aurEff->GetTickNumber()) / (float(aurEff->GetTotalTicks()) / 2.f);
        GetAura()->GetEffect(EFFECT_0)->SetAmount(amount * tickMulti);

    }
    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_gla_agony::Apply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_gla_agony::Calc, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

class spell_gla_bane_of_havoc : public AuraScript
{
    PrepareAuraScript(spell_gla_bane_of_havoc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_BANE_OF_HAVOC });
    }

    void Proc(ProcEventInfo& eventInfo)
    {
        uint32 heal = eventInfo.GetHealInfo()->GetHeal();
        Unit* target = eventInfo.GetProcTarget();
        float pct = GetSpellInfo()->GetEffect(EFFECT_1).BasePoints;
        if (target->GetHealthPct() > pct)
            if (100.f * (target->GetHealth() - heal) / target->GetMaxHealth() > pct)
                target->SetHealth(target->GetHealth() - heal);
            else
                target->SetHealth((pct / 100.f)* target->GetMaxHealth());
    }
    void Register() override
    {
        AfterProc += AuraProcFn(spell_gla_bane_of_havoc::Proc);
    }
};

class spell_gla_banish_aura : public AuraScript
{
    PrepareAuraScript(spell_gla_banish_aura);


    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_BANISH_AURA, SPELL_WARLOCK_BANISH_AURA_DUMMY });
    }

    void Apply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetTarget())
            GetTarget()->AddAura(SPELL_WARLOCK_BANISH_AURA_DUMMY, GetUnitOwner());
    }
    void Remove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetTarget())
            GetTarget()->RemoveAura(SPELL_WARLOCK_BANISH_AURA_DUMMY);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_gla_banish_aura::Apply, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_gla_banish_aura::Remove, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_gla_finger_of_death_aura : public AuraScript
{
    PrepareAuraScript(spell_gla_finger_of_death_aura);


    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_FINGER_OF_DEATH, SPELL_WARLOCK_FINGER_OF_DEATH_AURA });
    }

    void Handle(ProcEventInfo& eventInfo)
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return;
        if(caster->isHonorOrXPTarget(GetTarget()))
            caster->RemoveSpellCooldown(SPELL_WARLOCK_FINGER_OF_DEATH, true);
    }

    void Register() override
    {
        AfterProc += AuraProcFn(spell_gla_finger_of_death_aura::Handle);
    }
};

void AddSC_gla_warlock_spell_scripts()
{

    RegisterSpellScript(spell_gla_feral_demon);
    RegisterSpellScript(spell_gla_enrage);
    RegisterSpellScript(spell_gla_hardened_skin);
    RegisterSpellScript(spell_gla_blood_boil);
    RegisterSpellScript(spell_gla_demonic_circle_demonport);
    RegisterSpellScript(spell_gla_demonic_circle_imprison);
    RegisterSpellScript(spell_gla_felgaurds_persistence);
    RegisterSpellScript(spell_gla_demonic_sacrifice);
    RegisterSpellScript(spell_gla_demonic_enhancements);
    RegisterSpellScript(spell_gla_bane_of_portals);
    RegisterSpellScript(spell_gla_bane_of_despair);
    RegisterSpellScript(spell_gla_cripple);
    RegisterSpellScript(spell_gla_devour_magic);
    RegisterSpellScript(spell_gla_ritual_of_tower);
    RegisterSpellScript(spell_gla_wither);
    RegisterSpellScript(spell_gla_curse_of_imprudence);
    RegisterSpellScript(spell_gla_ritual_strike);
    RegisterSpellScript(spell_gla_drain_life);
    RegisterSpellScript(spell_gla_agony);
    RegisterSpellScript(spell_gla_bane_of_havoc);
    RegisterSpellScript(spell_gla_banish_aura);
    RegisterSpellScript(spell_gla_finger_of_death_aura); 
    RegisterSpellScript(spell_gla_soul_fragments_hit);
}
