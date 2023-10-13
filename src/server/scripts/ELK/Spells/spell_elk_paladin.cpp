#include "spell_elk_include.h"

class spell_elk_crusader_strike : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_crusader_strike);
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

        CRITICAL_ANIMATION_SEARCH_BLOCK;

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
        GetCaster()->CastSpell(GetCaster(), ELKS(CRITICAL_ATTACK_HIT), false);
        Spell* spell = GetCaster()->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (!spell)
            return;
        auto auraMap = spell->GetTriggerDummy();
        auraMap.emplace(MapDummy::TriggeringSpell, std::optional<uint32>(ELKS(CRUSADER_STRIKE)));
        AttackUnique();
    }
    virtual void Register()
    {
        BeforeSpellLoad += SpellCastFn(spell_elk_crusader_strike::SpellClick);
        AfterFullChannel += SpellCastFn(spell_elk_crusader_strike::AttackHit);
        BeforeCast += SpellCastFn(spell_elk_crusader_strike::TriggerExtension);
    }
};

class spell_elk_crusader_strike_hit : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_crusader_strike_hit);

    CRITICAL_ATTACK_HIT_BLOCK(spell_elk_crusader_strike_hit);

    void DamageComponent()
    {
        CalcDamageInfo damageInfo;
        GetCaster()->CalculateMeleeDamage(GetHitUnit(), &damageInfo);
        damageInfo.damages[0].damageSchoolMask = GetSpellInfo()->GetSchoolMask();
        damageInfo.hitOutCome = MELEE_HIT_CRIT;
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION;
        damageInfo.damages[0].damage *= 2;

        Unit::DealDamageMods(GetHitUnit(), damageInfo.damages[0].damage, &damageInfo.damages[0].absorb);
        GetCaster()->PlayDistanceSound(129);

        GetCaster()->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        dmgInfo.SetSpellInfo(GetSpellInfo());
        GetCaster()->ProcDamageAndSpell(GetCaster(), damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damages[0].damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);
        GetCaster()->DoOnAttackHitScripts(GetHitUnit(), dmgInfo);
    }

};

class spell_elk_exorcism : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_exorcism);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_elk_exorcism::TriggerExtension);
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
        if (info && info->GetSchoolMask() & GetAura()->GetSpellInfo()->GetSchoolMask() && info->CastTimeEntry->CastTime > 200)
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
        GetCaster()->AddAura(ELKS(HOLY_POWER), GetCaster());
        if (!(GetAura()->GetCharges()))
            GetAura()->Remove();
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

class spell_elk_judgement_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_judgement_aura);

    void AttackProc(AuraEffect const* aurEff, ProcEventInfo & eventInfo)
    {
        if (eventInfo.GetActor() != GetCaster())
            return;
        auto mask = GetSpellInfo()->GetSchoolMask();
        for (auto aura : GetTarget()->GetAppliedAuras())
        {
            if (aura.second->GetBase()->IsPermanent())
                continue;
            if (!(aura.second->IsPositive()) && (aura.second->GetBase()->GetSpellInfo()->GetSchoolMask() & mask))
            {
                aura.second->GetBase()->SetDuration(int32(float(aura.second->GetBase()->GetDuration()) + 1000));
            }
        }
    }
    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_elk_judgement_aura::AttackProc, EFFECT_2, SPELL_AURA_DUMMY);
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
        SetHitDamage(float(GetHitDamage() * (1.0f + float(target->GetHealth()) / float(target->GetMaxHealth()))));
        float value = (1.f + (float)caster->SpellBasePowerBonusDone(GetSpellInfo()->GetSchoolMask(), 0, true));
        for (auto aura : caster->GetAppliedAuras())
        {
            if (aura.second->GetBase()->IsPermanent())
                continue;
            if (aura.second->IsPositive() && aura.second->GetBase()->GetSpellInfo()->GetSchoolMask() & GetSpellInfo()->GetSchoolMask())
            {
                aura.second->GetBase()->ModSpellPowerBonus(value*0.1f);
            }
            else
            {
                aura.second->GetBase()->ModSpellPowerBonus(-1 * value*0.1f);
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
            if (curAtk->m_spellInfo->Id == ELKS(ATTACK))
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
                if (curAtk->m_spellInfo->Id == ELKS(ATTACK_HIT) && curAtk->GetSpellTimer() > 0)
                {
                    spell->skip = true;
                    GetCaster()->AddSpellCooldown(spell->m_spellInfo->Id, 0, 1, false);
                    QueSpell(caster);
                    return;
                }
            }
        }
        if (Aura* aura = GetCaster()->GetAura(ELKS(COMBO_COUNT)))
        {
            uint16 cd = 0;
            if (GetCaster()->CanUseAttackType(BASE_ATTACK))
                cd += GetCaster()->GetAttackTime(BASE_ATTACK);
            if (GetCaster()->CanUseAttackType(OFF_ATTACK))
                cd += GetCaster()->GetAttackTime(OFF_ATTACK);
            if (aura->GetStackAmount() > 6)
            {
                combo = 6;
                aura->SetStackAmount(aura->GetStackAmount() - 6);
            }
            else
            {
                combo = aura->GetStackAmount();
                aura->Remove();
            }
            spell->SetRuneCooldown(cd * 2);
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
        GetCaster()->CastSpell(GetCaster(), 1000020, false);
    }
    void Register() override
    {
        BeforeSpellLoad += SpellCastFn(spell_elk_divine_storm::SpellQue);
        AfterFullChannel += SpellCastFn(spell_elk_divine_storm::SpellFinish);
        AfterHit += SpellHitFn(spell_elk_divine_storm::SpellHit);
        BeforeCast += SpellCastFn(spell_elk_divine_storm::TriggerExtension);
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

class spell_elk_avenging_wrath : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_avenging_wrath);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_elk_avenging_wrath::TriggerExtension);
    }
};

class spell_elk_avenging_wrath_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_avenging_wrath_aura);


    void OnAttack(Spell* spell)
    {
        if (spell->m_spellInfo->Id != ELKS(ATTACK) || !(GetCaster()->IsFalling()))
            return;
        Aura* aura = GetCaster()->GetAura(1000018);
        if(!aura)
            aura = GetCaster()->AddAura(1000018, GetCaster());
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
        if (Aura* aura = player->GetAura(1000018); aura)
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
        GetCaster()->CastCustomSpell(1000006, values, GetCaster(), TRIGGERED_FULL_MASK);
    }
    void MovePacket()
    {
        if (!(GetCaster()->IsFalling()))
        {
            if (Aura* aura = GetCaster()->GetAura(1000018); aura)
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

class spell_elk_consecration : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_consecration);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_elk_consecration::TriggerExtension);
    }
};

class spell_elk_seal_of_silver : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_seal_of_silver);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_elk_seal_of_silver::TriggerExtension);
    }
};

class spell_elk_flash_heal : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_flash_heal);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_elk_flash_heal::TriggerExtension);
    }
};

class spell_elk_flash_focus : public AuraScript
{
    PrepareAuraScript(spell_elk_flash_focus);

    void FlashFocus(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo()->Id == 1000010)
            return;
        Spell* spell = eventInfo.GetProcSpell();
        spell->ModBonusSpellPower(GetAura()->GetStackAmount() * 5);
    }
    void Register() override
    {
        DoPrepareProc += AuraProcFn(spell_elk_flash_focus::FlashFocus);
    }
};

#pragma region

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


void DivineStorm(spell_extension_system* ext, Spell* spell)
{
    for (auto* aura : ext->GetUnitOwner()->GetSingleCastAuras())
    {
        if (aura->GetSpellInfo()->GetSchoolMask() & SpellSchoolMask::SPELL_SCHOOL_MASK_HOLY)
            aura->SetDuration(aura->GetDuration() + 1000);
    }
}
void FlashHeal(spell_extension_system* ext, Spell* spell)
{
    ext->GetCaster()->AddAura(ELKS(FLASH_FOCUS), ext->GetCaster());
}
#pragma endregion




void AddSC_elk_paladin_scripts()
{
    RegisterSpellScript(spell_elk_crusader_strike);
    AddExtension(1000001, CrusaderStrike, 4, false, UNIT_MOD_STAT_STRENGTH, 0.6f);
    RegisterSpellScript(spell_elk_crusader_strike_hit);

    RegisterSpellScript(spell_elk_exorcism);
    AddExtension(1000002, Exorcism, 25, false, UNIT_MOD_STAT_SPIRIT, 1.0f);

    RegisterSpellScript(spell_elk_judgement_aura);

    RegisterSpellScript(spell_elk_hammer_of_wrath);

    RegisterSpellAndAuraScriptPair(spell_elk_avenging_wrath, spell_elk_avenging_wrath_aura);
    AddExtension(1000005, AvengingWrath, 10, true, UNIT_MOD_RESISTANCE_HOLY, 2.0f);
    RegisterSpellScript(spell_elk_avenging_wrath_falling_aura);

    RegisterSpellScript(spell_elk_consecration);
    AddExtension(1000006, Consecration, 30, true, UNIT_MOD_STAT_STAMINA, 1.5f);


    RegisterSpellScript(spell_elk_seal_of_silver);
    AddExtension(1000007, SealOfSilver, -15, true, UNIT_MOD_STAT_STRENGTH, 0.3f);

    RegisterSpellScript(spell_elk_divine_storm);
    AddExtension(1000008, DivineStorm, 12, true, UNIT_MOD_STAT_INTELLECT, 1.0f);

    RegisterSpellScript(spell_elk_retribution_aura);
    RegisterSpellScript(spell_elk_retribution_aura_2);

    RegisterSpellScript(spell_elk_flash_heal);
    AddExtension(1000010, FlashHeal, 5, false, UNIT_MOD_STAT_INTELLECT, 0.3f);
    RegisterSpellScript(spell_elk_flash_focus);

    RegisterSpellScript(spell_elk_holy_power_aura);
    RegisterSpellScript(spell_elk_seal_of_righteousness_aura);

    RegisterSpellAndAuraScriptPair(spell_elk_justice, spell_elk_justice_aura);
}
