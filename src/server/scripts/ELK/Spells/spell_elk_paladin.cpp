#include "spell_elk_include.h"


class spell_elk_retribution_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_retribution_aura);
     
    void Apply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    { 
        Unit* target = GetTarget();
        if (!target)
            return;
        auto info = target->GetLastSpellUsed();
        if (!info)
            return;
        aurEff->GetSpellModifier()->SetSpellTarget(info->Id); 
    } 
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_elk_retribution_aura::Apply, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL); 
    }
};

class spell_elk_retribution_aura_2 : public AuraScript
{
    PrepareAuraScript(spell_elk_retribution_aura_2);

    uint8 hits = 0;
    uint8 hitsMax = 1;
    Unit* target;
    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        target = GetTarget();
        auto caster = GetCaster();
        if (!caster)
            return;
        auto aura = caster->GetAura(SUBSPELL_LONG_RETRIBUTION);
        if (!aura)
            return;
        hitsMax += aura->GetDevelopment();
    }
    void OnHit(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (dmgInfo.GetDamageType() == DIRECT_DAMAGE && hits < hitsMax)
        {
            hits++;
        }
    }
    void OnAttack(Unit* const target, DamageInfo const dmgInfo)
    {  
        if (target && hits && GetCaster())
            GetCaster()->DealDamage(GetCaster(), target, uint32(float(dmgInfo.GetDamage()) / 3.33f), nullptr, SPELL_DIRECT_DAMAGE, GetSpellInfo()->GetSchoolMask());
    }
    void AfterAtk()
    { 
        if(hits)
            hits--;
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    { 
        amount = -1;
    }
    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_elk_retribution_aura_2::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectApply += AuraEffectApplyFn(spell_elk_retribution_aura_2::OnApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_elk_retribution_aura_2::OnHit, EFFECT_0);
        OnAttackHit += OnAttackHitFn(spell_elk_retribution_aura_2::OnAttack);
        AfterAttack += AfterAttackFn(spell_elk_retribution_aura_2::AfterAtk);
    }
};

class spell_elk_holy_power_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_holy_power_aura);
    void RemoveCharge(Spell* spell)
    {
        auto info = spell->GetSpellInfo();
        if (info && info->GetSchoolMask() & SPELL_SCHOOL_MASK_HOLY && info->CastTimeEntry->CastTime > 200)
        {
            auto aura = GetAura();
            if (!aura)
                return;
            uint8 stackAmount = aura->GetStackAmount();
            if (stackAmount > 3)
                aura->SetStackAmount(stackAmount - 3);
            else
                aura->Remove();
        }
    }
    void Calc(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    { 
        aurEff->GetSpellModifier()->SetSpellSchool(GetSpellInfo()->GetSchoolMask());
    }
    void Register() override
    {
        OnSpellCast += OnSpellCastFn(spell_elk_holy_power_aura::RemoveCharge);
        OnEffectApply += AuraEffectApplyFn(spell_elk_holy_power_aura::Calc, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_elk_seal_of_righteousness_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_seal_of_righteousness_aura);
    bool hit = false;
    void OnAttack(Unit* const target, DamageInfo const dmgInfo)
    {
        if (dmgInfo.GetSpellInfo()->Id == 210011)
            GetAura()->SetCharges(7);
        else
            for (auto aura : GetCaster()->GetAppliedAuras())
            {
                if (aura.second->GetBase()->IsPermanent())
                    continue;
                if (!(aura.second->IsPositive()))
                {
                    aura.second->GetBase()->SetDuration(int32(float(aura.second->GetBase()->GetDuration()) * 0.93));
                }
            }
    } 
    void Register() override
    {
        OnAttackHit += OnAttackHitFn(spell_elk_seal_of_righteousness_aura::OnAttack); 
    }
};

class spell_elk_judgement_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_judgement_aura);

    void Apply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        Unit* target = GetTarget();
        if (!target)
            return;
        Unit* caster = GetCaster();
        if (!caster)
            return;
        auto aura = GetAura();
        if (!aura)
            return;
        uint32 bonus = caster->ConsumePercentOffense(30.0f);

        aura->SetDuration(aura->GetDuration() + bonus / 10);
    } 
         
    void AttackProc(AuraEffect const* aurEff, ProcEventInfo & eventInfo)
    {
        if (eventInfo.GetActor() != GetCaster())
            return;
        auto mask = GetSpellInfo()->GetSchoolMask();
        for (auto aura : GetCaster()->GetAppliedAuras())
        {
            if (aura.second->GetBase()->IsPermanent())
                continue;
            if (aura.second->IsPositive() && aura.second->GetBase()->GetSpellInfo()->GetSchoolMask() & mask)
            {
                aura.second->GetBase()->SetDuration(int32(float(aura.second->GetBase()->GetDuration()) + 2000));
            }
        }
        for (auto aura : GetTarget()->GetAppliedAuras())
        {
            if (aura.second->GetBase()->IsPermanent())
                continue;
            if (!(aura.second->IsPositive()) && (aura.second->GetBase()->GetSpellInfo()->GetSchoolMask() & mask))
            {
                aura.second->GetBase()->SetDuration(int32(float(aura.second->GetBase()->GetDuration()) + 2000));
            }
        }
    }
    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_elk_judgement_aura::AttackProc, EFFECT_2, SPELL_AURA_DUMMY);
        OnEffectApply += AuraEffectApplyFn(spell_elk_judgement_aura::Apply, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_elk_hammer_of_wrath : public SpellScript
{
    PrepareSpellScript(spell_elk_hammer_of_wrath);

    void Hit()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;
        Unit* target = GetHitUnit();
        if (!target)
            return;
        SetHitDamage(float(GetHitDamage()) * (1.0f + 2 * float(target->GetHealth()) / float(target->GetMaxHealth())) + caster->ConsumePercentOffense(100.0f));
        float value = (1.f + (float)caster->SpellBasePowerBonusDone(GetSpellInfo()->GetSchoolMask(), 0, true));
        for (auto aura : caster->GetAppliedAuras())
        {
            if (aura.second->GetBase()->IsPermanent())
                continue;
            if (aura.second->IsPositive() && aura.second->GetBase()->GetSpellInfo()->GetSchoolMask() & GetSpellInfo()->GetSchoolMask())
            {
                aura.second->GetBase()->ModSpellPowerBonus(value/10);
            }
            else
            {
                aura.second->GetBase()->ModSpellPowerBonus(-1 * value/ 10);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_elk_hammer_of_wrath::Hit);
    }
};
class spell_elk_divine_storm : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_divine_storm);

    uint8 combo = 0;
    void SpellQue()
    {
        Spell* spell = GetSpell();
        Unit* caster = GetCaster();

        Spell* curAtk = caster->GetCurrentSpell(CURRENT_CHANNELED_SPELL);

        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == ATTACK)
            {
                spell->skip = true;
                QueSpell(caster);
                return;
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
                    QueSpell(caster);
                    return;
                }
            }
        }
        if (Aura* aura = GetCaster()->GetAura(COMBO_COUNT))
        {
            uint16 cd = 0;
            if (GetCaster()->CanUseAttackType(BASE_ATTACK))
                cd += GetCaster()->GetAttackTime(BASE_ATTACK);
            if (GetCaster()->CanUseAttackType(OFF_ATTACK))
                cd += GetCaster()->GetAttackTime(OFF_ATTACK);
            if (aura->GetStackAmount() >= 6)
            {
                cd *= 6;
                combo = 6;
                aura->SetStackAmount(aura->GetStackAmount() - 6);
            }
            else
            {
                cd *= aura->GetStackAmount();
                combo = aura->GetStackAmount();
                aura->Remove();
            }
            spell->SetRuneCooldown(cd);
            spell->SetRuneCost(2);
            AttackBegin();
        }
        else
            spell->skip = true;
    }
    void SpellHit()
    {
        Unit* victim = GetHitUnit();

        CalcDamageInfo damageInfo;
        damageInfo.damageSchoolMask = GetSpellInfo()->GetSchoolMask();
        GetCaster()->CalculateMeleeDamage(victim, 0, &damageInfo); 
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION; 
        damageInfo.damage *= combo;
        Unit::DealDamageMods(victim, damageInfo.damage, &damageInfo.absorb); 

        GetCaster()->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        dmgInfo.SetSpellInfo(GetSpellInfo());
        GetCaster()->DoOnAttackHitScripts(victim, dmgInfo);
        GetCaster()->ProcDamageAndSpell(damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);

    }
    void SpellFinish()
    {
        AfterAttack();
    }
    void Register() override
    {
        BeforeCastTime += SpellCastFn(spell_elk_divine_storm::SpellQue);
        AfterCast += SpellCastFn(spell_elk_divine_storm::SpellFinish);
        AfterHit += SpellHitFn(spell_elk_divine_storm::SpellHit);
    }
};

class spell_elk_blessing_of_might : public SpellScript
{
    PrepareSpellScript(spell_elk_blessing_of_might);

    uint8 ticks = 0;
    void Apply(SpellMissInfo missInfo)
    {
        Unit* target = GetHitUnit();
        if (!target)
            return;
        auto aura = target->GetAura(210013);
        if (!aura)
            return;
        ticks = aura->GetStackAmount();
        aura->Remove();
    }
    void AfterApply()
    {
        if (ticks)
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_BASE_POINT0, ticks);
            GetCaster()->CastCustomSpell(210006, values, GetHitUnit(), TRIGGERED_FULL_MASK);
        }
        else
            GetCaster()->CastSpell(GetHitUnit(), 210006, true);

    } 
    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_elk_blessing_of_might::Apply);
        AfterHit += SpellHitFn(spell_elk_blessing_of_might::AfterApply);
    }
}; 
class spell_elk_blessing_of_might_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_blessing_of_might_aura);
     
    void UpdateEffect(AuraEffect const* aurEff)
    {
        if (GetAura()->GetStackAmount() >= 60)
            return;
        GetAura()->SetStackAmount(GetAura()->GetStackAmount() + 1);
    }
    void Register() override
    { 
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elk_blessing_of_might_aura::UpdateEffect, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    } 
};
class spell_elk_justice : public SpellScript
{
    PrepareSpellScript(spell_elk_justice);
     
    void SpellCast()
    {
        GetSpell()->SetBonusRange(GetCaster()->GetCombatReach());
    }
    void Register() override
    { 
        BeforeCastTime += SpellCastFn(spell_elk_justice::SpellCast);
    }
};
class spell_elk_justice_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_justice_aura);


    void DamageProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        if(eventInfo.GetSpellInfo() && (eventInfo.GetSpellInfo()->GetSchoolMask() & GetAura()->GetSpellInfo()->GetSchoolMask()))
            GetAura()->SetStackAmount(GetAura()->GetStackAmount() + 1);
    }
    void SpellCast(Spell* spell)
    {
        if (spell->m_targets.GetUnitTarget() && GetCaster() && GetCaster()->IsAlive())
            spell->m_targets.SetUnitTarget(GetCaster());
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_elk_justice_aura::DamageProc, EFFECT_1, SPELL_AURA_MOD_RESISTANCE);
        BeforeSpellCast += BeforeSpellCastFn(spell_elk_justice_aura::SpellCast);
    }
};
class spell_elk_avenging_wrath_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_avenging_wrath_aura);

    bool airAttacked = false;

    void OnAttack(Spell* spell)
    {
        if (spell->m_spellInfo->Id != ATTACK || airAttacked || !(GetCaster()->IsFalling()))
            return;
        airAttacked = true;
        GetAura()->GetEffect(EFFECT_2)->SetEnabled(false);
        GetCaster()->CastSpell(GetCaster(), 100014, true);
    }
    void MovePacket()
    {
        if (GetCaster()->IsFalling())
            return;
        if (!airAttacked)
            return;
        airAttacked = false;
        GetAura()->GetEffect(EFFECT_2)->SetEnabled(true);
    }

    void Register() override
    {
        OnSpellCast += OnSpellCastFn(spell_elk_avenging_wrath_aura::OnAttack);
        OnMovementPacket += OnMovementPacketFn(spell_elk_avenging_wrath_aura::MovePacket);
    }
};

class spell_elk_inquisite_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_inquisite_aura);
    void RemoveCharge(Spell* spell)
    {
        auto info = spell->GetSpellInfo();
        if (info && info->GetSchoolMask() & SPELL_SCHOOL_MASK_HOLY)
        {
            auto aura = GetAura();
            if (!aura)
                return;
            uint8 stackAmount = aura->GetStackAmount();
            if (stackAmount > 1)
                aura->SetStackAmount(stackAmount - 1);
            else
                aura->Remove();
        }
    }
    void Calc(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        aurEff->GetSpellModifier()->SetSpellSchool(GetSpellInfo()->GetSchoolMask());
    }
    void Register() override
    {
        OnSpellCast += OnSpellCastFn(spell_elk_inquisite_aura::RemoveCharge);
        OnEffectApply += AuraEffectApplyFn(spell_elk_inquisite_aura::Calc, EFFECT_0, SPELL_AURA_MOD_DAMAGE_DONE, AURA_EFFECT_HANDLE_REAL);
    }
};
void AddSC_elk_paladin_scripts()
{
    RegisterSpellScript(spell_elk_retribution_aura);
    RegisterSpellScript(spell_elk_retribution_aura_2);
    RegisterSpellScript(spell_elk_holy_power_aura);
    RegisterSpellScript(spell_elk_seal_of_righteousness_aura);
    RegisterSpellScript(spell_elk_judgement_aura);
    RegisterSpellScript(spell_elk_hammer_of_wrath);
    RegisterSpellScript(spell_elk_divine_storm);
    RegisterSpellAndAuraScriptPair(spell_elk_blessing_of_might, spell_elk_blessing_of_might_aura);
    RegisterSpellAndAuraScriptPair(spell_elk_justice, spell_elk_justice_aura);
    RegisterSpellScript(spell_elk_avenging_wrath_aura);
    RegisterSpellScript(spell_elk_inquisite_aura);
}
