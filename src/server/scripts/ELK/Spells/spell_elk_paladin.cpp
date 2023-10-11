#include "spell_elk_include.h"


class spell_elk_crusader_strike_expert_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_crusader_strike_expert_aura);

    const uint8 comboLimit = 7;
    uint8 trackedCombo = 0;
    uint8 lastCombo = 0;
    void SpellApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        Unit* target = GetTarget();
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
    void ComboRemove(Aura* aura, bool added)
    {
        if (!added)
        {
            trackedCombo = 0;
            lastCombo = 0;
        }
    } 
    void ComboAura(Aura* aura, int16 amount)
    {
        if (aura->GetSpellInfo()->Id == COMBO_COUNT)
        {
            int8 comboChange = (amount - lastCombo);
            if (-1 * comboChange > trackedCombo)
            {
                trackedCombo = 0;
                lastCombo = 0;
            }
            else
            {
                lastCombo = amount;
                if (comboChange < 0)
                {
                    return;
                }
                trackedCombo += comboChange;
                if (trackedCombo >= comboLimit)
                {
                    GetCaster()->CastSpell(GetCaster(), 210007, true);
                    trackedCombo -= comboLimit;
                }
            }
        } 
    }
    void Register() override
    {
        AuraAddRemove += AuraAddRemoveFn(spell_elk_crusader_strike_expert_aura::ComboRemove);
        OnEffectApply += AuraEffectApplyFn(spell_elk_crusader_strike_expert_aura::SpellApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_elk_crusader_strike_expert_aura::SpellRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnAuraStack += OnAuraStackFn(spell_elk_crusader_strike_expert_aura::ComboAura);
    }
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
    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
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
        if (GetTargetApplication() && GetTargetApplication()->GetTarget() && hits && GetCaster())
            GetCaster()->DoDamageYourself(target, uint32(float(dmgInfo.GetDamage()) / 3.33f), GetSpellInfo(), EFFECT_0);
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
        if (!(dmgInfo.GetSpellInfo()) || hit)
            return;
        hit = true;
        GetCaster()->DoDamageYourself(target, 7 + uint32(float(GetCaster()->SpellBasePowerBonusDone(GetSpellInfo()->GetSchoolMask(), GetAura()->GetSpellPowerBonus())) / 7.f), GetSpellInfo(), EFFECT_0);
        GetAura()->SetCharges(GetAura()->GetCharges() - 1);
        for (auto aura : GetCaster()->GetAppliedAuras())
        {
            if (aura.second->GetBase()->IsPermanent())
                continue;
            if (!(aura.second->IsPositive()))
            {
                aura.second->GetBase()->SetDuration(int32(float(aura.second->GetBase()->GetDuration()) * 0.93));
            }
        }
        if (dmgInfo.GetSpellInfo()->Id == 210011)
            GetAura()->SetCharges(7);
        else
        {
            if (!(GetAura()->GetCharges()))
                GetAura()->Remove();
        }
    }
    void After()
    {
        hit = false;
    }
    void Register() override
    {
        OnAttackHit += OnAttackHitFn(spell_elk_seal_of_righteousness_aura::OnAttack);
        AfterAttack += AfterAttackFn(spell_elk_seal_of_righteousness_aura::After);
    }
};
class spell_elk_seal_of_righteousness_aura_basic : public AuraScript
{
    PrepareAuraScript(spell_elk_seal_of_righteousness_aura_basic);
    bool hit = false; 
    void OnAttack(Unit* const target, DamageInfo const dmgInfo)
    {
        if (!(dmgInfo.GetSpellInfo()) || hit)
            return;
        hit = true;
        GetCaster()->DoDamageYourself(target, 7 + uint32(float(GetCaster()->SpellBasePowerBonusDone(GetSpellInfo()->GetSchoolMask(), GetAura()->GetSpellPowerBonus())) / 7.f), GetSpellInfo(), EFFECT_0);
        GetAura()->SetCharges(GetAura()->GetCharges() - 1);
        if (dmgInfo.GetSpellInfo()->Id == 210011)
            GetAura()->SetCharges(7);
        else
        {
            if (!(GetAura()->GetCharges()))
                GetAura()->Remove();
        }
    }
    void After()
    {
        hit = false;
    }
    void Register() override
    {
        OnAttackHit += OnAttackHitFn(spell_elk_seal_of_righteousness_aura_basic::OnAttack);
        AfterAttack += AfterAttackFn(spell_elk_seal_of_righteousness_aura_basic::After);
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
        SetHitDamage(float(GetHitDamage() + caster->ConsumePercentOffense(100.0f)) * (1.0f + 2 * float(target->GetHealth()) / float(target->GetMaxHealth())));
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
class spell_elk_hammer_of_wrath_basic : public SpellScript
{
    PrepareSpellScript(spell_elk_hammer_of_wrath_basic);

    void Hit()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;
        Unit* target = GetHitUnit();
        if (!target)
            return;
        SetHitDamage(float(GetHitDamage() + caster->ConsumePercentOffense(100.0f)) * (1.0f + 2 * float(target->GetHealth()) / float(target->GetMaxHealth())));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_elk_hammer_of_wrath_basic::Hit);
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
                GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 100, false);
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
                    GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 1, false);
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
            if (aura->GetStackAmount() > 6)
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
        {
            spell->skip = true;
            GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 100, false);
        }
    }
    void SpellHit()
    {
        Unit* victim = GetHitUnit();

        CalcDamageInfo damageInfo;
        damageInfo.damages[0].damageSchoolMask = GetSpellInfo()->GetSchoolMask();
        GetCaster()->CalculateMeleeDamage(victim, &damageInfo); 
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION; 
        damageInfo.damages[0].damage *= combo;
        Unit::DealDamageMods(victim, damageInfo.damages[0].damage, &damageInfo.damages[0].absorb);

        GetCaster()->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        dmgInfo.SetSpellInfo(GetSpellInfo());
        GetCaster()->ProcDamageAndSpell(GetCaster(), damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damages[0].damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);
        GetCaster()->DoOnAttackHitScripts(victim, dmgInfo);


    }
    void SpellFinish()
    {
        AfterAttack();
        GetCaster()->CastSpell(GetCaster(), 210029, false);
    }
    void Register() override
    {
        BeforeSpellLoad += SpellCastFn(spell_elk_divine_storm::SpellQue);
        AfterFullChannel += SpellCastFn(spell_elk_divine_storm::SpellFinish);
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
        CustomSpellValues values;
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, ticks + GetCaster()->ConsumePercentOffense(30.f));
        GetCaster()->CastCustomSpell(210006, values, GetHitUnit(), TRIGGERED_FULL_MASK); 

    } 
    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_elk_blessing_of_might::Apply);
        AfterHit += SpellHitFn(spell_elk_blessing_of_might::AfterApply);
    }
};
class spell_elk_blessing_of_might_basic : public SpellScript
{
    PrepareSpellScript(spell_elk_blessing_of_might_basic);
     
    void Apply(SpellMissInfo missInfo)
    {
        Unit* target = GetHitUnit();
        if (!target)
            return;
        auto aura = target->GetAura(210013);
        if (!aura)
            return; 
        aura->Remove();
    }
    void AfterApply()
    { 
        CustomSpellValues values;
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, GetCaster()->ConsumePercentOffense(30.f));
        GetCaster()->CastCustomSpell(210006, values, GetHitUnit(), TRIGGERED_FULL_MASK); 

    }
    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_elk_blessing_of_might_basic::Apply);
        AfterHit += SpellHitFn(spell_elk_blessing_of_might_basic::AfterApply);
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

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    { 
        GetAura()->SetStackAmount(GetAura()->GetStackAmount() + 6);
    }
    void DamageProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        
        if(eventInfo.GetDamageInfo() && (eventInfo.GetDamageInfo()->GetSchoolMask() & GetAura()->GetSpellInfo()->GetSchoolMask()) && eventInfo.GetDamageInfo()->GetDamageType() != DOT &&  eventInfo.GetDamageInfo()->GetDamage() > 0)
            GetAura()->SetStackAmount(GetAura()->GetStackAmount() + 1);
    }
    void SpellCast(Spell* spell)
    {
        if (spell->m_targets.GetUnitTarget() && GetCaster() && GetCaster()->IsAlive())
            spell->m_targets.SetUnitTarget(GetCaster());
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_elk_justice_aura::OnApply, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectProc += AuraEffectProcFn(spell_elk_justice_aura::DamageProc, EFFECT_2, SPELL_AURA_DUMMY);
        BeforeSpellCast += BeforeSpellCastFn(spell_elk_justice_aura::SpellCast);
    }
};
class spell_elk_avenging_wrath_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_avenging_wrath_aura);


    void OnAttack(Spell* spell)
    {
        if (spell->m_spellInfo->Id != ATTACK || !(GetCaster()->IsFalling()))
            return;
        Aura* aura = GetCaster()->GetAura(210027);
        if(!aura)
            aura = GetCaster()->AddAura(210027, GetCaster());
        spell->skip = true;
        GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 100, false);
    }
    void AddDoubleJumps(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        auto player = GetCaster()->ToPlayer();
        if (!player)
            return;
        player->ModDoubleJumpMax(3);
    }
    void RemoveDoubleJumps(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        auto player = GetCaster()->ToPlayer();
        if (Aura* aura = player->GetAura(210027); aura)
            aura->Remove();
        if (!player)
            return;
        player->ModDoubleJumpMax(-3);
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_elk_avenging_wrath_aura::AddDoubleJumps, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_elk_avenging_wrath_aura::RemoveDoubleJumps, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        BeforeSpellCast += BeforeSpellCastFn(spell_elk_avenging_wrath_aura::OnAttack);
    }
};

class spell_elk_avenging_wrath_falling_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_avenging_wrath_falling_aura);
    uint16 ticks = 0;
    bool falling = false;
    void Removed(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (!falling || GetCaster()->IsFalling())
            return;
        CustomSpellValues values;
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, (ticks / 10) + 1);
        GetCaster()->CastCustomSpell(210007, values, GetCaster(), TRIGGERED_FULL_MASK);
    }
    void MovePacket()
    {
        if (!(GetCaster()->IsFalling()))
        {
            if (Aura* aura = GetCaster()->GetAura(210027); aura)
                aura->Remove();
            return;
        }
        if (GetAura()->GetEffect(EFFECT_0)->IsPeriodic() && !falling)
        {
            falling = true;
            return;
        }
    }
    void Tick(AuraEffect const* aurEff)
    {
        ticks++;
    }
    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_elk_avenging_wrath_falling_aura::Removed, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnMovementPacket += OnMovementPacketFn(spell_elk_avenging_wrath_falling_aura::MovePacket);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elk_avenging_wrath_falling_aura::Tick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
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
    void Register() override
    {
        OnSpellCast += OnSpellCastFn(spell_elk_inquisite_aura::RemoveCharge);
    }
};



void CrusaderStrike(spell_extension_system* ext, Spell* spell)
{
    Aura* aura = ext->GetUnitOwner()->GetAura(1000007);
    if (!aura)
        return;
    float value = (1.f + (float)ext->GetUnitOwner()->SpellBasePowerBonusDone(ext->GetSpellInfo()->GetSchoolMask(), 0, true));
    aura->ModSpellPowerBonus(value * 0.04f);
    aura->SetDuration(aura->GetDuration() - 400);
}


void Exorcism(spell_extension_system* ext, Spell* spell)
{
    ext->GetUnitOwner()->CastSpell(ext->GetUnitOwner(), 527, true);
}

void DivineStorm(spell_extension_system* ext, Spell* spell)
{
    for (auto* aura : ext->GetUnitOwner()->GetSingleCastAuras())
    {
        if (aura->GetSpellInfo()->GetSchoolMask() & SpellSchoolMask::SPELL_SCHOOL_MASK_HOLY)
            aura->SetDuration(aura->GetDuration() + 1000);
    }
}

void AvengingWrath(spell_extension_system* ext, Spell* spell)
{
    Aura* aura = ext->GetUnitOwner()->GetAura(1000005);
    if (!aura)
        return;
    aura->SetDuration(aura->GetDuration() + 1000);
}


void Consecration(spell_extension_system* ext, Spell* spell)
{
    if (spell->m_spellInfo->Id == 1000001)
        return;
    Player* player = ext->GetUnitOwner()->ToPlayer();
    if (!player)
        return;
    player->RemoveSpellCooldown(1000001, true);

}



void SealOfSilver(spell_extension_system* ext, Spell* spell)
{
    Aura* aura = ext->GetUnitOwner()->GetAura(1000007);
    if (!aura)
        return;
    float value = (1.f + (float)ext->GetUnitOwner()->SpellBasePowerBonusDone(ext->GetSpellInfo()->GetSchoolMask(), 0, true));
    aura->ModSpellPowerBonus(value * 0.1f);
}


class spell_ext_crusader_strike : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_crusader_strike);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_crusader_strike::TriggerExtension);
    }
};
class spell_ext_exorcism : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_exorcism);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_exorcism::TriggerExtension);
    }
};
class spell_ext_divine_storm : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_divine_storm);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_divine_storm::TriggerExtension);
    }
};
class spell_ext_avenging_wrath : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_avenging_wrath);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_avenging_wrath::TriggerExtension);
    }
};
class spell_ext_consecration : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_consecration);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_consecration::TriggerExtension);
    }
};

class spell_ext_seal_of_silver : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_seal_of_silver);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_seal_of_silver::TriggerExtension);
    }
};
class spell_ext_flash_heal : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_flash_heal);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_flash_heal::TriggerExtension);
    }
};
class spell_ext_flash_focus : public AuraScript
{
    PrepareAuraScript(spell_ext_flash_focus);

    void FlashFocus(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo()->Id == 1000010)
            return;
        Spell* spell = eventInfo.GetProcSpell();
        spell->ModBonusSpellPower(GetAura()->GetStackAmount() * 5);
    }
    void Register() override
    {
        DoPrepareProc += AuraProcFn(spell_ext_flash_focus::FlashFocus);
    }
};

void AddSC_elk_paladin_scripts()
{
    RegisterSpellScript(spell_elk_crusader_strike_expert_aura);
    RegisterSpellScript(spell_elk_retribution_aura);
    RegisterSpellScript(spell_elk_retribution_aura_2);
    RegisterSpellScript(spell_elk_holy_power_aura);
    RegisterSpellScript(spell_elk_seal_of_righteousness_aura_basic);
    RegisterSpellScript(spell_elk_seal_of_righteousness_aura);
    RegisterSpellScript(spell_elk_judgement_aura);
    RegisterSpellScript(spell_elk_hammer_of_wrath_basic);
    RegisterSpellScript(spell_elk_hammer_of_wrath);
    RegisterSpellScript(spell_elk_divine_storm);
    RegisterSpellAndAuraScriptPair(spell_elk_blessing_of_might_basic, spell_elk_blessing_of_might_aura);
    RegisterSpellAndAuraScriptPair(spell_elk_blessing_of_might, spell_elk_blessing_of_might_aura);
    RegisterSpellAndAuraScriptPair(spell_elk_justice, spell_elk_justice_aura);
    RegisterSpellScript(spell_elk_avenging_wrath_aura);
    RegisterSpellScript(spell_elk_avenging_wrath_falling_aura);
    RegisterSpellScript(spell_elk_inquisite_aura);


    RegisterSpellScript(spell_ext_crusader_strike);
    AddExtension(1000001, ExtensionObj(CrusaderStrike, 4, false));
    RegisterSpellScript(spell_ext_exorcism);
    AddExtension(1000002, ExtensionObj(Exorcism, 25, false));
    RegisterSpellScript(spell_ext_divine_storm);
    AddExtension(1000004, ExtensionObj(DivineStorm, 12, true));
    RegisterSpellScript(spell_ext_avenging_wrath);
    AddExtension(1000005, ExtensionObj(AvengingWrath, 10, true));
    RegisterSpellScript(spell_ext_consecration);
    AddExtension(1000006, ExtensionObj(Consecration, 30, true));
    RegisterSpellScript(spell_ext_seal_of_silver);
    AddExtension(1000007, ExtensionObj(SealOfSilver, -15, true));
}
