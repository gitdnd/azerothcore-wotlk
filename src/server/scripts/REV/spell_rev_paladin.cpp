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
  * Scripts for spells with SPELLFAMILY_WARRIOR and SPELLFAMILY_GENERIC spells used by warrior players.
  * Ordered alphabetically using scriptname.
  * Scriptnames of files in this file should be prefixed with "spell_warr_".
  */

#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"

enum PaladinSpells
{
    SPELL_PALADIN_DIVINE_PLEA = 54428,
    SPELL_PALADIN_BLESSING_OF_SANCTUARY_BUFF = 67480,
    SPELL_PALADIN_BLESSING_OF_SANCTUARY_ENERGIZE = 57319,

    SPELL_PALADIN_HOLY_SHOCK_R1 = 20473,
    SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE = 25912,
    SPELL_PALADIN_HOLY_SHOCK_R1_HEALING = 25914,

    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_DRUID = 37878,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PALADIN = 37879,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PRIEST = 37880,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_SHAMAN = 37881,

    SPELL_PALADIN_DIVINE_STORM = 53385,
    SPELL_PALADIN_DIVINE_STORM_DUMMY = 54171,
    SPELL_PALADIN_DIVINE_STORM_HEAL = 54172,

    SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE = 25997,

    SPELL_PALADIN_FORBEARANCE = 25771,
    SPELL_PALADIN_AVENGING_WRATH_MARKER = 61987,
    SPELL_PALADIN_IMMUNE_SHIELD_MARKER = 61988,

    SPELL_PALADIN_HAND_OF_SACRIFICE = 6940,
    SPELL_PALADIN_DIVINE_SACRIFICE = 64205,

    SPELL_PALADIN_JUDGEMENT_DAMAGE = 54158,
    SPELL_PALADIN_JUDGEMENT_OF_JUSTICE = 20184,
    SPELL_PALADIN_JUDGEMENT_OF_LIGHT = 20185,
    SPELL_PALADIN_JUDGEMENT_OF_WISDOM = 20186,

    SPELL_PALADIN_GLYPH_OF_SALVATION = 63225,

    SPELL_PALADIN_RIGHTEOUS_DEFENSE_TAUNT = 31790,

    SPELL_PALADIN_SANCTIFIED_WRATH = 57318,
    SPELL_PALADIN_SANCTIFIED_WRATH_TALENT_R1 = 53375,

    SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS = 25742,

    SPELL_PALADIN_CONCENTRACTION_AURA = 19746,
    SPELL_PALADIN_SANCTIFIED_RETRIBUTION_R1 = 31869,
    SPELL_PALADIN_SWIFT_RETRIBUTION_R1 = 53379,

    SPELL_PALADIN_IMPROVED_CONCENTRACTION_AURA = 63510,
    SPELL_PALADIN_IMPROVED_DEVOTION_AURA = 63514,
    SPELL_PALADIN_SANCTIFIED_RETRIBUTION_AURA = 63531,
    SPELL_PALADIN_AURA_MASTERY_IMMUNE = 64364,

    SPELL_GENERIC_ARENA_DAMPENING = 74410,
    SPELL_GENERIC_BATTLEGROUND_DAMPENING = 74411,

    SPELL_PALADIN_SHIELD_OF_RIGHTEOUSNESS = 53600,

    REV_HOLY_POWER = 9000009,
    REV_AVENGED_GLORY = 9000010,
    REV_AVENGED_SUFFERING = 9000011,
    REV_AVENGED_INNOCENCE = 9000023,
    REV_AVENGED_INNOCENCE_HEAL = 9000013,
    REV_UNSEALED_POWER = 9000014,

    REV_SEAL_OF_THE_VENGFUL = 9000006,
    REV_SEAL_OF_HOLINESS = 9000018,
    REV_SEAL_OF_THE_VANGUARD = 9000019,
    REV_SEAL_OF_THE_VANGUARD_PROC = 9000020,

    REV_PERSONAL_GUIDE = 9000021,

    REV_LAST_WISH = 9000022,

    REV_EXORCIST = 9000012,

    REV_HOLY_CRUSADER = 9000025,
    REV_HOLY_CRUSADER_PROC = 9000026,

    REV_GOLDEN_JUDGEMENT = 9000027,
    REV_AZURE_JUDGEMENT= 9000030,
    REV_CRIMSON_JUDGEMENT = 9000031,

    REV_RIGHTEOUSS_BLOCK = 9000029,

    REV_ENDLESS_JUDGEMENT = 9000032,
    REV_WISEST_JUDGEMENT = 9000033,

    REV_HOW_STUN = 9000034,

    REV_TURN_UNDEAD = 9000036,

    REV_AURA_MASTER = 9000035,
    REV_AURA_MASTERY = 9000038,
    REV_AURA_MASTERY_PROTECTION = 9000039,
    REV_AURA_MASTERY_CRUSADER = 9000040,
    REV_AURA_MASTERY_RETRIBUTION = 9000041,
    REV_AURA_MASTERY_CONENTRATION = 9000042,
    REV_AURA_MASTERY_RESISTANCCE = 9000043,

    REV_TRAVELING_STAR = 9000044,

    REV_AURA_PROPHECY_STREAK = 9000052,
    REV_AURA_PROPHECY_STREAK_PROC = 9000053,

    REV_AURA_ANGELIC_HYMN = 9000058,

    REV_AURA_INTERVENTION = 9000060,
    REV_AURA_PROTECTION = 9000061,
    REV_AURA_SACRIFICE = 9000062,

    REV_AURA_INTERVENTION_BUFF = 9000063,
    REV_AURA_PROTECTION_BUFF = 9000064,
    REV_AURA_SACRIFICE_BUFF = 9000065,

    REV_AURA_INTERVENTION_TRIGGER = 9000066,
    REV_AURA_PROTECTION_TRIGGER = 9000067,
    REV_AURA_SACRIFICE_TRIGGER = 9000068,
};

#define SaveCDs()   player->AddSpellCooldown(REV_AURA_INTERVENTION_TRIGGER, 9, 60000, true);        \
player->AddSpellCooldown(REV_AURA_PROTECTION_TRIGGER, 9, 60000, true);                              \
player->AddSpellCooldown(REV_AURA_SACRIFICE_TRIGGER, 9, 60000, true)                       

class spell_rev_holy_shock : public SpellScript
{
    PrepareSpellScript(spell_rev_holy_shock);

    bool Validate(SpellInfo const* spellInfo) override
    {
        SpellInfo const* firstRankSpellInfo = sSpellMgr->GetSpellInfo(SPELL_PALADIN_HOLY_SHOCK_R1);
        if (!firstRankSpellInfo)
            return false;

        // can't use other spell than holy shock due to spell_ranks dependency
        if (!spellInfo->IsRankOf(firstRankSpellInfo))
            return false;

        uint8 rank = spellInfo->GetRank();
        if (!sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE, rank, true) || !sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_HEALING, rank, true))
            return false;

        return true;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* unitTarget = GetHitUnit())
        {
            uint8 rank = GetSpellInfo()->GetRank();
            if (caster->IsFriendlyTo(unitTarget))
                caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_HEALING, rank), true);
            else
                caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE, rank), true);
        }
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetExplTargetUnit())
        {
            if (!caster->IsFriendlyTo(target))
            {
                if (!caster->IsValidAttackTarget(target))
                    return SPELL_FAILED_BAD_TARGETS;

                if (!caster->isInFront(target))
                    return SPELL_FAILED_UNIT_NOT_INFRONT;
            }
        }
        else
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void Hit()
    {
        if (!GetHitUnit())
            return;
        if (!GetHitUnit()->IsFriendlyTo(GetCaster()))
            return;
        for (auto aura : GetHitUnit()->GetAppliedAuras())
        {
            if (aura.second->GetBase()->IsPermanent())
                continue;
            if (aura.second->IsPositive() && aura.second->GetBase()->GetSpellInfo()->SpellFamilyName == 10 && aura.second->GetBase()->GetOwner() == GetCaster())
            {
                uint32 duration = aura.second->GetBase()->GetDuration() + 8000;
                if (duration > aura.second->GetBase()->GetMaxDuration())
                    duration = aura.second->GetBase()->GetMaxDuration();
                aura.second->GetBase()->SetDuration(duration);
            }
        }
    }
    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_rev_holy_shock::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_rev_holy_shock::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnHit += SpellHitFn(spell_rev_holy_shock::Hit);
    }
};

class spell_rev_crusader_strike : public SpellScript
{
    PrepareSpellScript(spell_rev_crusader_strike);


    void Hit()
    {
        for (auto aura : GetCaster()->GetAppliedAuras())
        {
            if (aura.second->GetBase()->IsPermanent())
                continue;
            if (aura.second->IsPositive() && aura.second->GetBase()->GetSpellInfo()->SpellFamilyName == 10 && aura.second->GetBase()->GetOwner() == GetCaster())
            {
                uint32 duration = aura.second->GetBase()->GetDuration() + 5000;
                if (duration > aura.second->GetBase()->GetMaxDuration())
                    duration = aura.second->GetBase()->GetMaxDuration();
                aura.second->GetBase()->SetDuration(duration);
            }
        }
        if (GetCaster()->HasAura(REV_HOLY_CRUSADER))
        {
            GetCaster()->CastSpell(GetCaster(), REV_HOLY_CRUSADER_PROC, true);
        }
    }
    void Register() override
    {
        OnHit += SpellHitFn(spell_rev_crusader_strike::Hit);
    }
};

class spell_rev_hammer_of_the_righteous : public SpellScript
{
    PrepareSpellScript(spell_rev_hammer_of_the_righteous);


    void Hit()
    {
        std::list<Unit*> party = {};
        GetCaster()->GetPartyMembers(party);
        for (Unit* guy : party)
        {
            for (auto aura : guy->GetAppliedAuras())
            {
                if (aura.second->GetBase()->IsPermanent())
                    continue;
                if (aura.second->IsPositive() && aura.second->GetBase()->GetSpellInfo()->SpellFamilyName == 10 && aura.second->GetBase()->GetOwner() == GetCaster())
                {
                    uint32 duration = aura.second->GetBase()->GetDuration() + 2000;
                    if (duration > aura.second->GetBase()->GetMaxDuration())
                        duration = aura.second->GetBase()->GetMaxDuration();
                    aura.second->GetBase()->SetDuration(duration);
                }
            }
        }
    }
    void Register() override
    {
        OnHit += SpellHitFn(spell_rev_hammer_of_the_righteous::Hit);
    }
};

class spell_rev_endless_dawn : public AuraScript
{
    PrepareAuraScript(spell_rev_endless_dawn);

    void Proc(ProcEventInfo& proc)
    {
        for (auto aura : GetUnitOwner()->GetAppliedAuras())
        {
            if (aura.second->GetBase()->IsPermanent())
                continue;
            if (aura.second->IsPositive() && aura.second->GetBase()->GetSpellInfo()->SpellFamilyName == 10 && aura.second->GetBase()->GetOwner() == GetCaster())
            {
                uint32 duration = aura.second->GetBase()->GetDuration() + 1000;
                if (duration > aura.second->GetBase()->GetMaxDuration())
                    duration = aura.second->GetBase()->GetMaxDuration();
                aura.second->GetBase()->SetDuration(duration);
            }
        }
        for (auto aura : proc.GetDamageInfo()->GetVictim()->GetAppliedAuras())
        {
            if (aura.second->GetBase()->IsPermanent())
                continue;
            if (!aura.second->IsPositive() && aura.second->GetBase()->GetSpellInfo()->SpellFamilyName == 10 && aura.second->GetBase()->GetOwner() == GetCaster())
            {
                uint32 duration = aura.second->GetBase()->GetDuration() + 1000;
                if (duration > aura.second->GetBase()->GetMaxDuration())
                    duration = aura.second->GetBase()->GetMaxDuration();
                aura.second->GetBase()->SetDuration(duration);
            }
        }
    }
    void Register() override
    {
        OnProc += AuraProcFn(spell_rev_endless_dawn::Proc);
    }
};


class spell_rev_holy_shield : public AuraScript
{
    PrepareAuraScript(spell_rev_holy_shield);

    void Proc(ProcEventInfo& proc)
    {
        Player* player = GetUnitOwner()->ToPlayer();
        if (player)
        {
            player->RemoveSpellCooldown(SPELL_PALADIN_SHIELD_OF_RIGHTEOUSNESS, true);
        }
    }
    void Register() override
    {
        OnProc += AuraProcFn(spell_rev_holy_shield::Proc);
    }
};

class spell_rev_righteous_defense : public AuraScript
{
    PrepareAuraScript(spell_rev_righteous_defense);

    void Proc(ProcEventInfo& proc)
    {
        if (proc.GetDamageInfo()->GetVictim() == GetUnitOwner())
            GetUnitOwner()->ModifyPowerPct(POWER_MANA, 3.f);
    }


    void Register() override
    {
        OnProc += AuraProcFn(spell_rev_righteous_defense::Proc);
    }
};
class spell_rev_blessing : public AuraScript
{
    PrepareAuraScript(spell_rev_blessing);

    void Apply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetCaster()->HasAura(REV_PERSONAL_GUIDE))
        {
            if (GetCaster() == GetTarget())
            {
                GetAura()->SetMaxDuration(10000);
                GetAura()->SetDuration(10000);
            }
            else
                GetAura()->Remove();
        }
    }
    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_rev_blessing::Apply, EFFECT_0, SPELL_AURA_MOD_ATTACK_POWER, AURA_EFFECT_HANDLE_REAL);
        AfterEffectApply += AuraEffectApplyFn(spell_rev_blessing::Apply, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN, AURA_EFFECT_HANDLE_REAL);
        AfterEffectApply += AuraEffectApplyFn(spell_rev_blessing::Apply, EFFECT_0, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectApply += AuraEffectApplyFn(spell_rev_blessing::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_rev_aura_mastery : public SpellScript
{
    PrepareSpellScript(spell_rev_aura_mastery);


    void HandleAfterCast()
    {
        if (GetCaster()->HasAura(REV_AURA_MASTER))
        {
            GetCaster()->RemoveAura(REV_AURA_MASTERY);
            switch (GetSpell()->GetSpellInfo()->GetFirstRankSpell()->Id)
            {
            case 465:
                if (!GetCaster()->HasAura(REV_AURA_MASTERY_PROTECTION))
                {
                    GetCaster()->AddAura(REV_AURA_MASTERY_PROTECTION, GetCaster());
                    GetCaster()->AddAura(REV_AURA_MASTERY, GetCaster());
                }
                break;
            case 7294:
                if (!GetCaster()->HasAura(REV_AURA_MASTERY_RETRIBUTION))
                {
                    GetCaster()->AddAura(REV_AURA_MASTERY_RETRIBUTION, GetCaster());
                    GetCaster()->AddAura(REV_AURA_MASTERY, GetCaster());
                }
                break;
            case 19746:
                if (!GetCaster()->HasAura(REV_AURA_MASTERY_CONENTRATION))
                {
                    GetCaster()->AddAura(REV_AURA_MASTERY_CONENTRATION, GetCaster());
                    GetCaster()->AddAura(REV_AURA_MASTERY, GetCaster());
                }
                break;
            case 32223:
                if (!GetCaster()->HasAura(REV_AURA_MASTERY_CRUSADER))
                {
                    GetCaster()->AddAura(REV_AURA_MASTERY_CRUSADER, GetCaster());
                    GetCaster()->AddAura(REV_AURA_MASTERY, GetCaster());
                }
                break;
            case 19876:
            case 19888:
            case 19891:
                if (!GetCaster()->HasAura(REV_AURA_MASTERY_RESISTANCCE))
                {
                    GetCaster()->AddAura(REV_AURA_MASTERY_RESISTANCCE, GetCaster());
                    GetCaster()->AddAura(REV_AURA_MASTERY, GetCaster());
                }
                break;
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_rev_aura_mastery::HandleAfterCast);
    }
};
class spell_rev_last_wish : public AuraScript
{
    PrepareAuraScript(spell_rev_last_wish);

    void Proc(ProcEventInfo& proc)
    {
        GetAura()->Remove();
        GetCaster()->AddAura(REV_LAST_WISH, GetCaster());
    }
    void BeforeApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        GetAura()->GetEffect(EFFECT_0)->SetAmount(GetCaster()->GetMaxHealth() - GetCaster()->GetHealth());
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_rev_last_wish::BeforeApply, EFFECT_0, SPELL_AURA_MOD_RESISTANCE, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_rev_last_wish::Proc);

    }
};

class spell_rev_exorcism : public SpellScript
{
    PrepareSpellScript(spell_rev_exorcism);

    void Hit()
    {
        if (GetCaster()->HasAura(REV_EXORCIST))
            GetCaster()->CastSpell(GetHitUnit(), 370, true);
    }
    void Register() override
    {
        OnHit += SpellHitFn(spell_rev_exorcism::Hit);
    }
};


class spell_rev_hammer_of_wrath : public SpellScript
{
    PrepareSpellScript(spell_rev_hammer_of_wrath);

    void Hit()
    {
        GetCaster()->CastSpell(GetHitUnit(), REV_HOW_STUN, true);
    }
    void Register() override
    {
        OnHit += SpellHitFn(spell_rev_hammer_of_wrath::Hit);
    }
};
class spell_rev_avenging_wrath : public AuraScript
{
    PrepareAuraScript(spell_rev_avenging_wrath);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_SANCTIFIED_WRATH, SPELL_PALADIN_SANCTIFIED_WRATH_TALENT_R1, REV_HOLY_POWER, REV_AVENGED_GLORY, REV_AVENGED_SUFFERING, REV_AVENGED_INNOCENCE, REV_AVENGED_INNOCENCE_HEAL });
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (AuraEffect const* aurEff = target->GetAuraEffectOfRankedSpell(SPELL_PALADIN_SANCTIFIED_WRATH_TALENT_R1, EFFECT_2))
        {
            int32 basepoints = aurEff->GetAmount();
            target->CastCustomSpell(target, SPELL_PALADIN_SANCTIFIED_WRATH, &basepoints, &basepoints, nullptr, true, nullptr, aurEff);
        }
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_PALADIN_SANCTIFIED_WRATH);
        uint8 holyPowerStacks = 1;
        if (Aura* holyPower = GetCaster()->GetAura(REV_HOLY_POWER))
        {
            holyPowerStacks += holyPower->GetStackAmount();
            holyPower->Remove();
        }
        if (Aura* avengedGlory = GetCaster()->GetAura(REV_AVENGED_GLORY))
        {
            GetCaster()->ModifyPowerPct(POWER_MANA, 30.f);
        }
        if (Aura* avengedSuffering = GetCaster()->GetAura(REV_AVENGED_SUFFERING))
        {
            avengedSuffering->GetEffect(EFFECT_0)->SetAmount(0);
        }
        if (Aura* avengedInnocence = GetCaster()->GetAura(REV_AVENGED_INNOCENCE))
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_BASE_POINT0, holyPowerStacks * 400);
            GetCaster()->CastCustomSpell(REV_AVENGED_INNOCENCE_HEAL, values);
        }
    }
    void Proc(ProcEventInfo& eventInfo)
    {
        uint32 duration = GetAura()->GetDuration() + 1000;
        if (duration > GetAura()->GetMaxDuration())
            duration = GetAura()->GetMaxDuration();
        GetAura()->SetDuration(duration);
        GetCaster()->CastSpell(GetCaster(), REV_HOLY_POWER, true);
        if(Aura* avengedSuffering = GetCaster()->GetAura(REV_AVENGED_SUFFERING); avengedSuffering && GetCaster()->GetAura(REV_AVENGED_SUFFERING))
        {
            avengedSuffering->GetEffect(EFFECT_0)->SetAmount(300 * GetCaster()->GetAura(REV_AVENGED_SUFFERING)->GetStackAmount());
        }
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_rev_avenging_wrath::HandleApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_rev_avenging_wrath::HandleRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_rev_avenging_wrath::Proc);
    }
};

class spell_rev_unseal : public AuraScript
{
    PrepareAuraScript(spell_rev_unseal);


    bool Proc(ProcEventInfo& eventInfo)
    {
        if (!GetCaster()->HasSpellCooldown(REV_UNSEALED_POWER))
        {
            GetCaster()->ToPlayer()->AddSpellCooldown(REV_UNSEALED_POWER, 0, 4000);
            if (Aura* seal = GetCaster()->GetAura(REV_SEAL_OF_THE_VENGFUL))
            {
                seal->ModStackAmount(1);
            }
            else if (Aura* seal = GetCaster()->GetAura(REV_SEAL_OF_HOLINESS))
            {
                seal->ModStackAmount(1);
            }
            else if (Aura* seal = GetCaster()->GetAura(REV_SEAL_OF_THE_VANGUARD))
            {
                seal->ModStackAmount(1);
            }
        }
        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rev_unseal::Proc);
    }
};

class spell_rev_traveling_star : public AuraScript
{
    PrepareAuraScript(spell_rev_traveling_star);

    static const inline Position posCheck = (-99999, -99999, -99999);
    Position posLast = (-99999, -99999, -99999);
    bool Proc(ProcEventInfo& eventInfo)
    {
        if (posLast == posCheck)
        {
            posLast.Relocate(GetUnitOwner()->GetPosition());
            return false;
        }
        if (eventInfo.GetHealInfo() && eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id != REV_TRAVELING_STAR)
        {
            float dist = GetUnitOwner()->GetPosition().GetExactDist2d(posLast);
            if (dist > 20.f)
                dist = 20.f;
            HealInfo heal(GetUnitOwner(), eventInfo.GetHealInfo()->GetTarget(),
                float(eventInfo.GetHealInfo()->GetHeal()) * (dist / 20.f), GetSpellInfo(), SPELL_SCHOOL_MASK_HOLY
                );
            eventInfo.GetHealInfo()->GetTarget()->HealBySpell(heal);
            posLast.Relocate(GetUnitOwner()->GetPosition());
        }
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rev_traveling_star::Proc);
    }
};
class spell_rev_seal : public SpellScript
{
    PrepareSpellScript(spell_rev_seal);


    void Cast()
    {
        GetCaster()->RemoveAura(9000006);
        GetCaster()->RemoveAura(9000018);
        GetCaster()->RemoveAura(9000019);
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_rev_seal::Cast);
    }
};
class spell_rev_seal_of_the_vengful_aura : public AuraScript
{
    PrepareAuraScript(spell_rev_seal_of_the_vengful_aura);


    bool Proc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetHealInfo())
        {
            if (eventInfo.GetProcTarget() != GetCaster())
                GetCaster()->ModifyHealth(int32(eventInfo.GetHealInfo()->GetHeal()) / 2 * -1);
            return false;
        }
        else if(eventInfo.GetDamageInfo())
        {
            uint8 repeats = 1;
            if (eventInfo.GetProcSpell())
                switch (eventInfo.GetProcSpell()->m_spellInfo->Id) {
                case REV_GOLDEN_JUDGEMENT:
                    GetAura()->ModStackAmount(6);
                    break;
                case REV_AZURE_JUDGEMENT:
                    repeats = 3;
                    GetAura()->SetDuration(GetAura()->GetDuration() / 2);
                    break;
                case REV_CRIMSON_JUDGEMENT:
                    repeats = 7;
                    GetAura()->SetDuration(1);
                    break;
                default:
                    return false;
                }
            uint32 amount = GetStackAmount() * repeats;
            GetAura()->GetEffect(EFFECT_0)->SetAmount(amount);
            GetCaster()->DealDamage(GetCaster(), GetCaster(), amount / 2, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, GetSpellInfo());
            return true;
        }
        return false;
    }

    void Tick(AuraEffect const* aurEff)
    {
        GetAura()->ModStackAmount(1);
    }
    void Apply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        GetAura()->SetStackAmount(100);
    }
    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rev_seal_of_the_vengful_aura::Proc);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rev_seal_of_the_vengful_aura::Tick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        AfterEffectApply += AuraEffectApplyFn(spell_rev_seal_of_the_vengful_aura::Apply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};
class spell_rev_seal_of_the_vanguard_aura : public AuraScript
{
    PrepareAuraScript(spell_rev_seal_of_the_vanguard_aura);


    bool Proc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetDamageInfo())
            return false;
        uint8 repeats = 1;
        CustomSpellValues values;
        if (eventInfo.GetProcSpell())
            switch (eventInfo.GetProcSpell()->m_spellInfo->Id) {
            case REV_GOLDEN_JUDGEMENT:
                GetAura()->ModStackAmount(6);
                break;
            case REV_AZURE_JUDGEMENT:
                repeats = 3;
                GetAura()->SetDuration(GetAura()->GetDuration() / 2);
                break;
            case REV_CRIMSON_JUDGEMENT:
                repeats = 7;
                GetAura()->SetDuration(1);
                break;
            default:
                return false;
            }
        values.AddSpellMod(SPELLVALUE_BASE_POINT1, (GetCaster()->GetMaxHealth() * 100 * repeats) / 5000);
        GetCaster()->CastCustomSpell(REV_SEAL_OF_THE_VANGUARD_PROC, values);
        return false;
    }

    void Tick(AuraEffect const* aurEff)
    {
        GetAura()->ModStackAmount(1);
    }
    void Apply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        GetAura()->SetStackAmount(100);
    }
    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rev_seal_of_the_vanguard_aura::Proc);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rev_seal_of_the_vanguard_aura::Tick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        AfterEffectApply += AuraEffectApplyFn(spell_rev_seal_of_the_vanguard_aura::Apply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_rev_seal_of_holiness_aura : public AuraScript
{
    PrepareAuraScript(spell_rev_seal_of_holiness_aura);


    bool Proc(ProcEventInfo& eventInfo)
    {

        uint8 repeats = 1;
        if (eventInfo.GetProcSpell())
            switch (eventInfo.GetProcSpell()->m_spellInfo->Id) {
            case REV_GOLDEN_JUDGEMENT:
                GetAura()->ModStackAmount(6);
                break;
            case REV_AZURE_JUDGEMENT:
                repeats = 3;
                GetAura()->SetDuration(GetAura()->GetDuration() / 2);
                break;
            case REV_CRIMSON_JUDGEMENT:
                repeats = 7;
                GetAura()->SetDuration(1);
                break;
            default:
                return false;
            }
        GetCaster()->ModifyPowerPct(POWER_MANA, float(GetAura()->GetStackAmount() * 0.1f) * repeats);
        return false;
    }

    void Tick(AuraEffect const* aurEff)
    {
        GetAura()->ModStackAmount(1);
    }
    void Apply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        GetAura()->SetStackAmount(100);
    }
    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rev_seal_of_holiness_aura::Proc);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rev_seal_of_holiness_aura::Tick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        AfterEffectApply += AuraEffectApplyFn(spell_rev_seal_of_holiness_aura::Apply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_rev_coat_of_flames : public AuraScript
{
    PrepareAuraScript(spell_rev_coat_of_flames);



    void Tick(AuraEffect const* aurEff)
    {
        if (GetUnitOwner()->GetMaxHealth() > GetUnitOwner()->GetHealth() * 2)
            GetAura()->Remove();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rev_coat_of_flames::Tick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};
class spell_rev_judgement : public AuraScript
{
    PrepareAuraScript(spell_rev_judgement);


    bool OnProc(ProcEventInfo& eventInfo)
    {
        if (!GetUnitOwner())
            return false;
        if (GetUnitOwner()->HasAura(REV_ENDLESS_JUDGEMENT))
        {
            std::list<Unit*> party = {};
            GetUnitOwner()->GetPartyMembers(party);
            for (Unit* guy : party)
            {
                HealInfo heal(GetCaster(), guy, float(guy->GetHealth()) * 0.02f, eventInfo.GetSpellInfo(), SPELL_SCHOOL_MASK_HOLY);
                guy->HealBySpell(heal);
            }
        }
        if (GetUnitOwner()->HasAura(REV_WISEST_JUDGEMENT))
        {
            std::list<Unit*> party = {};
            GetUnitOwner()->GetPartyMembers(party);
            for (Unit* guy : party)
            {
                guy->ModifyPowerPct(POWER_MANA, 2.f);
            }
        }
        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rev_judgement::OnProc);
    }
};

class spell_rev_righteouss_block : public AuraScript
{
    PrepareAuraScript(spell_rev_righteouss_block);


    bool OnProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetDamageInfo() && eventInfo.GetDamageInfo()->GetBlock())
            GetCaster()->AddAura(REV_RIGHTEOUSS_BLOCK, GetCaster());
        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rev_righteouss_block::OnProc);
    }
};

class spell_rev_prophecy_streak : public AuraScript
{
    PrepareAuraScript(spell_rev_prophecy_streak);


    bool OnProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetDamageInfo() || !eventInfo.GetDamageInfo()->GetDamage() || (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == REV_AURA_PROPHECY_STREAK))
            return false;
        CustomSpellValues values;
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, eventInfo.GetDamageInfo()->GetDamage());
        GetCaster()->CastCustomSpell(REV_AURA_PROPHECY_STREAK_PROC, values);
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rev_prophecy_streak::OnProc);
    }
};

class spell_rev_consecration : public SpellScript
{
    PrepareSpellScript(spell_rev_consecration);


    void HandleAfterCast()
    {
        GetCaster()->CastSpell(GetCaster(), REV_TURN_UNDEAD, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_rev_consecration::HandleAfterCast);
    }
};

class spell_rev_grace_of_gold : public SpellScript
{
    PrepareSpellScript(spell_rev_grace_of_gold);


    void Hit()
    {
        if (GetCaster()->HasAura(REV_AURA_ANGELIC_HYMN) && rand() % 10 == 0)
        {
            Player* player = GetHitUnit()->ToPlayer();
            if (player)
                player->RemoveAllSpellCooldown();
        }
    }
    void Register() override
    {
        OnHit += SpellHitFn(spell_rev_grace_of_gold::Hit);
    }
};

class spell_rev_intervention : public SpellScript
{
    PrepareSpellScript(spell_rev_intervention);


    void HandleAfterCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;
        uint32 aura = 0;
        if (player->GetAura(REV_AURA_INTERVENTION))
        {
            player->RemoveAura(REV_AURA_INTERVENTION);
            aura = REV_AURA_INTERVENTION_BUFF;
        }
        if (player->GetAura(REV_AURA_PROTECTION))
        {
            player->RemoveAura(REV_AURA_PROTECTION);
            aura = REV_AURA_PROTECTION_BUFF;
        }
        if (player->GetAura(REV_AURA_SACRIFICE))
        {
            player->RemoveAura(REV_AURA_SACRIFICE);
            aura = REV_AURA_SACRIFICE_BUFF;
        }
        if (!aura)
        {
            player->AddAura(REV_AURA_INTERVENTION, player);
            return;
        }

        if (!player->HasAura(SPELL_PALADIN_FORBEARANCE))
        {
            player->AddAura(SPELL_PALADIN_FORBEARANCE, player);
            player->AddAura(aura, player);
        }
        SaveCDs();
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_rev_intervention::HandleAfterCast);
    }
};
class spell_rev_protection : public SpellScript
{
    PrepareSpellScript(spell_rev_protection);


    void HandleAfterCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;
        uint32 aura = 0;
        if (player->GetAura(REV_AURA_INTERVENTION))
        {
            player->RemoveAura(REV_AURA_INTERVENTION);
            aura = REV_AURA_INTERVENTION_BUFF;
        }
        if (player->GetAura(REV_AURA_PROTECTION))
        {
            player->RemoveAura(REV_AURA_PROTECTION);
            aura = REV_AURA_PROTECTION_BUFF;
        }
        if (player->GetAura(REV_AURA_SACRIFICE))
        {
            player->RemoveAura(REV_AURA_SACRIFICE);
            aura = REV_AURA_SACRIFICE_BUFF;
        }
        if (!aura)
        {
            player->AddAura(REV_AURA_PROTECTION, player);
            return;
        }

        if (Unit* target = player->GetSelectedUnit(); target && !target->HasAura(SPELL_PALADIN_FORBEARANCE))
        {
            player->AddAura(aura, target);
            target->AddAura(SPELL_PALADIN_FORBEARANCE, target);
        }
        else if (!player->HasAura(SPELL_PALADIN_FORBEARANCE))
        {
            player->AddAura(aura, player);
            player->AddAura(SPELL_PALADIN_FORBEARANCE, player);
        }
        SaveCDs();

    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_rev_protection::HandleAfterCast);
    }
};
class spell_rev_sacrifice : public SpellScript
{
    PrepareSpellScript(spell_rev_sacrifice);


    void HandleAfterCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;
        uint32 aura = 0;
        if (player->GetAura(REV_AURA_INTERVENTION))
        {
            player->RemoveAura(REV_AURA_INTERVENTION);
            aura = REV_AURA_INTERVENTION_BUFF;
        }
        if (player->GetAura(REV_AURA_PROTECTION))
        {
            player->RemoveAura(REV_AURA_PROTECTION);
            aura = REV_AURA_PROTECTION_BUFF;
        }
        if (player->GetAura(REV_AURA_SACRIFICE))
        {
            player->RemoveAura(REV_AURA_SACRIFICE);
            aura = REV_AURA_SACRIFICE_BUFF;
        }
        if (!aura)
        {
            player->AddAura(REV_AURA_SACRIFICE, player);
            return;
        }

        std::list<Unit*> party = {};
        player->GetPartyMembers(party);
        for (Unit* guy : party)
        {
            if (guy->HasAura(SPELL_PALADIN_FORBEARANCE))
                continue;
            Aura* aura2 = player->AddAura(aura, guy);
            if (aura2)
            {
                aura2->SetMaxDuration((aura2->GetMaxDuration() / 10) * 3);
                aura2->SetDuration(aura2->GetMaxDuration());
            }
            guy->AddAura(SPELL_PALADIN_FORBEARANCE, guy);
        }
        SaveCDs();
        return;

    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_rev_sacrifice::HandleAfterCast);
    }
};

class spell_rev_guardian_of_ancient_kings : public AuraScript
{
    PrepareAuraScript(spell_rev_guardian_of_ancient_kings);
    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* player = eventInfo.GetActor();
        Unit* guardian = nullptr;
        for (Unit::ControlSet::const_iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
            if (int32((*itr)->GetEntry()) == GetSpellInfo()->Effects[EFFECT_0].MiscValue)
            {
                guardian = *itr;
                break;
            }
        if (!guardian)
            return;
        Unit* target = eventInfo.GetActionTarget();
        std::list<Unit*> targets;
        Acore::AnyUnfriendlyUnitInObjectRangeCheck u_check(guardian, guardian, 10.f);
        Acore::UnitListSearcher<Acore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(guardian, targets, u_check);
        Cell::VisitAllObjects(guardian, searcher, 10.0f);
        targets.remove(target);
        if (targets.size() == 0)
            return;
        Unit* guardianTarget = *targets.begin();
        guardian->SetOrientation(guardian->GetAngle(guardianTarget));
        if (SpellInfo const* procSpell = eventInfo.GetSpellInfo())
        {
            if (procSpell->IsPositive())
            {
                guardian->CastSpell(guardian, procSpell->Id, true, nullptr, aurEff, guardian->GetGUID());
            }
            else
            {
                if (!procSpell->IsAffectingArea())
                    GetUnitOwner()->SetFloatValue(UNIT_FIELD_COMBATREACH, 10.0f);
                guardian->CastSpell(guardianTarget, procSpell->Id, true, nullptr, aurEff, guardian->GetGUID());
                GetUnitOwner()->SetFloatValue(UNIT_FIELD_COMBATREACH, 0.01f);
            }
        }
        else if (eventInfo.GetDamageInfo())
        {
            guardianTarget = player->GetMeleeHitRedirectTarget(guardianTarget);
            CalcDamageInfo damageInfo;
            player->CalculateMeleeDamage(guardianTarget, &damageInfo, eventInfo.GetDamageInfo()->GetAttackType());
            for (uint8 i = 0; i < MAX_ITEM_PROTO_DAMAGES; ++i)
            {
                Unit::DealDamageMods(guardianTarget, damageInfo.damages[i].damage, &damageInfo.damages[i].absorb);
                damageInfo.damages[i].damage /= 2.0f;
            }
            damageInfo.attacker = guardian;
            guardian->SendAttackStateUpdate(&damageInfo);
            guardian->DealMeleeDamage(&damageInfo, true);
        }
    }
    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rev_guardian_of_ancient_kings::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

void AddSC_paladin_rev_scripts()
{
    RegisterSpellScript(spell_rev_holy_shock);
    RegisterSpellScript(spell_rev_crusader_strike);
    RegisterSpellScript(spell_rev_hammer_of_the_righteous);
    RegisterSpellScript(spell_rev_endless_dawn);
    RegisterSpellScript(spell_rev_holy_shield);
    RegisterSpellScript(spell_rev_righteous_defense);
    RegisterSpellScript(spell_rev_blessing);
    RegisterSpellScript(spell_rev_aura_mastery);
    RegisterSpellScript(spell_rev_last_wish);
    RegisterSpellScript(spell_rev_exorcism);
    RegisterSpellScript(spell_rev_hammer_of_wrath);
    RegisterSpellScript(spell_rev_avenging_wrath);
    RegisterSpellScript(spell_rev_unseal);
    RegisterSpellScript(spell_rev_traveling_star);
    RegisterSpellScript(spell_rev_seal);
    RegisterSpellScript(spell_rev_seal_of_the_vengful_aura);
    RegisterSpellScript(spell_rev_seal_of_the_vanguard_aura);
    RegisterSpellScript(spell_rev_seal_of_holiness_aura);

    RegisterSpellScript(spell_rev_coat_of_flames);
    RegisterSpellScript(spell_rev_righteouss_block);
    RegisterSpellScript(spell_rev_judgement);
    RegisterSpellScript(spell_rev_prophecy_streak);

    RegisterSpellScript(spell_rev_consecration);
    RegisterSpellScript(spell_rev_grace_of_gold);
    RegisterSpellScript(spell_rev_guardian_of_ancient_kings);
    RegisterSpellScript(spell_rev_intervention);
    RegisterSpellScript(spell_rev_protection);
    RegisterSpellScript(spell_rev_sacrifice);

}
