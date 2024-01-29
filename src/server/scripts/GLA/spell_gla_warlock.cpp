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
    SPELL_WARLOCK_FINGER_OF_DEATH           = 110001,
    SPELL_WARLOCK_FINGER_OF_DEATH_AURA      = 110002,
    SPELL_WARLOCK_BANISH_AURA               = 110005,
    SPELL_WARLOCK_BANISH_AURA_DUMMY         = 110006,
    SPELL_WARLOCK_BANE_OF_HAVOC             = 110007,
    SPELL_WARLOCK_AGONY                     = 110014,
    SPELL_WARLOCK_DRAIN_LIFE                = 110013,
    SPELL_WARLOCK_DRAIN_LIFE_ENEMY          = 110015,
    SPELL_WARLOCK_DRAIN_LIFE_ALLY           = 110016,
    SPELL_WARLOCK_RITUAL_STRIKE             = 110019,
    SPELL_WARLOCK_CURSE_OF_IMPRUDENCE       = 110020,
    SPELL_WARLOCK_WITHER                    = 110022,
    SPELL_WARLOCK_RITUAL_OF_TOWER           = 110027,
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
};
enum GlaWarlockUnits
{
    IMP                                     = 688,
    VOIDWALKER                              = 1860,
    SUCCUBUS                                = 1863,
    FELHUNTER                               = 417,
    FELGUARD                                = 30146,
    WARLOCK_TOWER                           = 1000001
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
        OnProc += AuraProcFn(spell_gla_bane_of_portals::Proc);
        AfterEffectRemove += AuraEffectRemoveFn(spell_gla_bane_of_portals::Remove, SPELL_AURA_DUMMY, EFFECT_1, AURA_EFFECT_HANDLE_REAL);
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
        HealInfo heal(unit, unit, float(unit->GetMaxHealth()) * float(GetSpellInfo()->Effects[EFFECT_1].CalcValue(unit, nullptr, GetHitUnit())) / 100.f, GetSpellInfo(), SPELL_SCHOOL_MASK_SHADOW);
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
        GetCaster()->SetMinion(summon, true);
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

    void Calc(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {

        float tickMulti = float(aurEff->GetTickNumber()) / (float(aurEff->GetTotalTicks()) / 2.f);
        amount = float(amount) * tickMulti;

    }
    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gla_agony::Calc, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
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
        AfterEffectApply += AuraEffectApplyFn(spell_gla_banish_aura::Apply, SPELL_AURA_MOD_DECREASE_SPEED, EFFECT_1, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_gla_banish_aura::Remove, SPELL_AURA_MOD_DECREASE_SPEED, EFFECT_1, AURA_EFFECT_HANDLE_REAL);
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

void AddSC_gla_rogue_spell_scripts()
{

    RegisterSpellScript(spell_gla_finger_of_death_aura);
}
