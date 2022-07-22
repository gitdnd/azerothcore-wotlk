#include "spell_elk_include.h"

enum SpellsELKPaladin : uint32
{
    SPELL_RETRIBUTION_AURA      = 210002,
    SPELL_RETRIBUTION_AURA_2    = 210003,
    SPELL_HOLY_POWER            = 210004,

    SUBSPELL_LONG_RETRIBUTION   = 219999,
};

class crusader_strike_aura : public AuraScript
{

};
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
     
    void Calc(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    { 
        aurEff->GetSpellModifier()->SetSpellSchool(SPELL_SCHOOL_MASK_ALL);
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_holy_power_aura::Calc, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};


void AddSC_elk_paladin_scripts()
{
    RegisterSpellScript(spell_elk_retribution_aura);
    RegisterSpellScript(spell_elk_retribution_aura_2);
    RegisterSpellScript(spell_holy_power_aura);


}
