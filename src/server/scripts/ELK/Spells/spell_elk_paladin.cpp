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
    void OnAttack(Unit*& const target, DamageInfo& const dmgInfo)
    {  
        if (target && hits)
            target->DealDamage(target, target, uint32(float(dmgInfo.GetDamage()) / 3.33f), nullptr, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_HOLY);
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

class spell_holy_power_aura : public AuraScript
{
    PrepareAuraScript(spell_holy_power_aura);
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
        aurEff->GetSpellModifier()->SetSpellSchool(SPELL_SCHOOL_MASK_HOLY);
    }
    void Register() override
    {
        OnSpellCast += OnSpellCastFn(spell_holy_power_aura::RemoveCharge);
        OnEffectApply += AuraEffectApplyFn(spell_holy_power_aura::Calc, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_hammer_of_wrath : public SpellScript
{
    PrepareSpellScript(spell_hammer_of_wrath);

    void Damage(SpellEffIndex effIndex)
    {
        Unit* target = GetHitUnit();
        if (!target)
            return;
        SetHitDamage(float(GetHitDamage()) * (1.0f + 2 * float(target->GetHealth()) / float(target->GetMaxHealth())));
        Unit* caster = GetCaster();
        if (!caster)
            return;
        for (auto aura : caster->GetAppliedAuras())
        {
            if (aura.second->IsPositive())
            {
                aura.second->GetBase()->GetEffect()
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hammer_of_wrath::Damage, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};
void AddSC_elk_paladin_scripts()
{
    RegisterSpellScript(spell_elk_retribution_aura);
    RegisterSpellScript(spell_elk_retribution_aura_2);
    RegisterSpellScript(spell_holy_power_aura);


    RegisterSpellScript(spell_hammer_of_wrath);
}
