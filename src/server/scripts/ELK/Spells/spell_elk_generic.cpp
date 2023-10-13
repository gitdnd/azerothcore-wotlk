#include "spell_elk_include.h"


class spell_elk_attack_hit : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_attack_hit);

    bool WasInAir = false;
    uint32 strike = 0;
    uint8 targetsHit = 0;

    void SpellBegin()
    {
        AttackBegin();
    }
    void SpellHit()
    { 
        AttackHit();
        targetsHit++;
        GetCaster()->ModifyPower(POWER_MANA, GetCaster()->GetStat(STAT_SPIRIT));
        if (WasInAir)
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_AURA_DURATION, 2000);
            GetCaster()->CastCustomSpell(ELKS(ATTACK_SLOW_DEBUFF), values, GetCaster(), TRIGGERED_FULL_MASK);
        }
        else
        {
            GetCaster()->CastSpell(GetHitUnit(), ELKS(ATTACK_SLOW_DEBUFF), true);
        }
    }
    void SpellFinish()
    {
        if (!(GetCaster()))
            return;

        AfterAttack(); 


        if (targetsHit)
        {
            Aura* aura = GetCaster()->GetAura(ELKS(COMBO_COUNT));
            if (!aura)
                GetCaster()->AddAura(ELKS(COMBO_COUNT), GetCaster());
            else
                aura->SetStackAmount(aura->GetStackAmount() + 1);
        }
    }
    void Register() override
    {
        BeforeCast += SpellCastFn(spell_elk_attack_hit::SpellBegin);
        AfterCast += SpellCastFn(spell_elk_attack_hit::SpellFinish);
        AfterHit += SpellHitFn(spell_elk_attack_hit::SpellHit);
    }
};


class spell_elk_critical_attack_hit : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_critical_attack_hit);
    CRITICAL_ATTACK_HIT_BLOCK(spell_elk_critical_attack_hit);

    virtual void DamageComponent() 
    { 
        CalcDamageInfo damageInfo; 
        GetCaster()->CalculateMeleeDamage(GetHitUnit(), &damageInfo); 
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


class spell_elk_spin_attack_hit : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_spin_attack_hit);

    bool WasInAir = false;
    uint32 strike = 0;
    uint8 targetsHit = 0;

    void SpellBegin()
    {
        AttackBegin();
    }
    void SpellHit()
    {
        Unit* victim = GetHitUnit();

        CalcDamageInfo damageInfo;
        GetCaster()->CalculateMeleeDamage(victim, &damageInfo);
        damageInfo.hitOutCome = MELEE_HIT_CRIT;
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION;
        damageInfo.damages[0].damage *= 2;

        GetCaster()->ModifyPower(POWER_MANA, GetCaster()->GetStat(STAT_SPIRIT));
        if (WasInAir)
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_AURA_DURATION, 2000);
            GetCaster()->CastCustomSpell(ELKS(ATTACK_SLOW_DEBUFF), values, GetCaster(), TRIGGERED_FULL_MASK);
        }
        else
        {
            GetCaster()->CastSpell(GetHitUnit(), ELKS(ATTACK_SLOW_DEBUFF), true);
        }

        targetsHit++;
        Unit::DealDamageMods(victim, damageInfo.damages[0].damage, &damageInfo.damages[0].absorb);
        GetCaster()->PlayDistanceSound(129);

        GetCaster()->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        dmgInfo.SetSpellInfo(GetSpellInfo());
        GetCaster()->ProcDamageAndSpell(GetCaster(), damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damages[0].damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);
        GetCaster()->DoOnAttackHitScripts(victim, dmgInfo);
    }
    void SpellFinish()
    {
        if (!(GetCaster()))
            return;

        AfterAttack();


        if (targetsHit)
        {
            Aura* aura = GetCaster()->GetAura(ELKS(COMBO_COUNT));
            if (!aura)
                GetCaster()->AddAura(ELKS(COMBO_COUNT), GetCaster());
            else
                aura->SetStackAmount(aura->GetStackAmount() + 1);
        }
    }
    void Register() override
    {
        BeforeCast += SpellCastFn(spell_elk_spin_attack_hit::SpellBegin);
        AfterCast += SpellCastFn(spell_elk_spin_attack_hit::SpellFinish);
        AfterHit += SpellHitFn(spell_elk_spin_attack_hit::SpellHit);
    }
};

class spell_elk_deflect_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_deflect_aura);

    bool success = false;

    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* attacker = dmgInfo.GetAttacker();
        Position pos1 = GetCaster()->GetPosition();
        Position pos2 = attacker->GetPosition();
        float angle = GetCaster()->NormalizeOrientation(pos1.GetRelativeAngle(pos2.GetPositionX(), pos2.GetPositionY()));
        if (angle < 0.7 || angle > 5.585)
        {
            if (!success)
            {
                auto auraMap = GetAura()->GetTriggerDummy();
                auraMap.emplace(MapDummy::Misc_1, std::optional<bool>(true));
                for (int i = 0; i < MAX_RUNES; i++)
                {
                    uint32 cd = GetCaster()->GetRuneCooldown(i);
                    GetCaster()->SetRuneCooldown(i, (cd > 15000) ? cd - 15000 : 0);
                }
                success = true;
                Aura* aura = GetCaster()->GetAura(ELKS(COMBO_COUNT));
                if (!aura)
                    GetCaster()->AddAura(ELKS(COMBO_COUNT), GetCaster());
                else
                    aura->SetStackAmount(aura->GetStackAmount() + 1);
            }
            GetCaster()->PlayDirectSound(20000);
            dmgInfo.AbsorbDamage(-1 * dmgInfo.GetDamage());
            if (attacker)
            {
                GetCaster()->ModifyPower(POWER_MANA, (dmgInfo.GetDamage() * GetCaster()->GetStat(STAT_SPIRIT)) / 1000);
            }
        }
    }
    void End(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        Spell* spell = GetCaster()->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (spell && (spell->m_spellInfo->Id == ELKS(SPELL_DEFLECT) || spell->m_spellInfo->Id == ELKS(SPELL_DEFLECT_SHORT)));
            GetCaster()->InterruptSpell(CURRENT_CHANNELED_SPELL);
    }
    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_elk_deflect_aura::Absorb, EFFECT_0);
        AfterEffectRemove += AuraEffectRemoveFn(spell_elk_deflect_aura::End, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_elk_deflect : public SpellScript
{
    PrepareSpellScript(spell_elk_deflect);
    bool success = false; 
    void SpamCheck()
    {

        GetCaster()->SetSheath(SHEATH_STATE_MELEE);
        GetSpell()->SetRuneCooldown(15000);
        GetSpell()->SetRuneCost(1);
        Aura* aura = GetCaster()->GetAura(ELKS(SPELL_DEFLECT));
        if (aura)
        {
            auto spellMap = aura->GetTriggerDummy();
            if (spellMap[MapDummy::Misc_1].has_value())
            {
                success = std::any_cast<bool>(spellMap[MapDummy::Misc_1].value());
            }
            if (!success and GetSpell()->GetSpellInfo()->Id == ELKS(SPELL_DEFLECT) and aura->GetDuration() > 100)
            {
                
                GetSpell()->cancel();
                aura->Remove();
                GetCaster()->CastSpell(GetCaster(), ELKS(SPELL_DEFLECT_SHORT), false);
                return;
            }
        }
    } 
    void Register() override
    {
     
        BeforeCastTime += SpellCastFn(spell_elk_deflect::SpamCheck);
    }
};

class spell_elk_double_jump : public SpellScript
{
    PrepareSpellScript(spell_elk_double_jump);
    
    void DoubleJump()
    {
        auto player = GetCaster()->ToPlayer();
        if (!player || !(player->CanDoubleJump()))
            return;
        GetSpell()->ModifyDamage(GetSpell()->m_spellInfo->Effects[0].BasePoints * 1.25);
        GetSpell()->EffectRoll(EFFECT_0, GetCaster()->GetSpeed(MOVE_RUN) - 7);
        player->AddDoubleJump();
    }
    void Register() override
    {

        AfterCast += SpellCastFn(spell_elk_double_jump::DoubleJump);
    }
};

class spell_elk_sprint_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_sprint_aura);

    void MovePacket()
    {
        if (GetCaster()->IsFalling())
            return;
        float facing = GetCaster()->NormalizeOrientation(abs(GetCaster()->GetOldOrientation()) - GetCaster()->GetOrientation());
        float x, y, z = 0;
        GetCaster()->GetOldPosition(x, y, z);
        float weightBonus = std::max(0.0f, (1.2f - GetCaster()->GetWeight()));
        if (facing < 0.5f + weightBonus || facing > 5.98f - weightBonus)
        {
            float angle = GetCaster()->NormalizeOrientation(Position(x, y, z, GetCaster()->NormalizeOrientation(GetCaster()->GetOldOrientation())).GetRelativeAngle(GetCaster()->GetPositionX(), GetCaster()->GetPositionY()));
            if (angle < 0.7f || angle > 5.585f)
            {
                return;
            }
        } 
        GetCaster()->RemoveAura(GetAura());
    }
    void EnergyCheck(AuraEffect const*  /*aurEff*/)
    {
        if(GetCaster()->GetPower(POWER_ENERGY) < abs(GetSpellInfo()->Effects[1].BasePoints))
            GetCaster()->RemoveAura(GetAura());
    }
    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elk_sprint_aura::EnergyCheck, EFFECT_1, SPELL_AURA_PERIODIC_ENERGIZE);
        OnMovementPacket += OnMovementPacketFn(spell_elk_sprint_aura::MovePacket);
    }
};

class spell_elk_dash_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_dash_aura);

    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        dmgInfo.ModifyDamage(-1 * dmgInfo.GetDamage());
    }
    void Cast(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes  /*mode*/)
    {
        GetCaster()->CastSpell(GetCaster(), 100042, true);
    }
    void MovePacket()
    {
        float x, y, z = 0;
        GetCaster()->GetOldPosition(x, y, z);
        float degree = GetCaster()->NormalizeOrientation(Position(x, y, z, GetCaster()->NormalizeOrientation(GetCaster()->GetOldOrientation())).GetRelativeAngle(GetCaster()->GetPositionX(), GetCaster()->GetPositionY()));
        if ((degree > 0.78 && degree < 2.35) || (degree > 3.92 && degree < 5.495))
        {
            return;
        }  
        GetAura()->Remove();
    } 
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_elk_dash_aura::Cast, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
        OnMovementPacket += OnMovementPacketFn(spell_elk_dash_aura::MovePacket);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_elk_dash_aura::Absorb, EFFECT_1);
    }

};

class spell_elk_tempo_dash_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_tempo_dash_aura);

    void Apply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        Unit* target = GetTarget(); 
        if (!target)
            return;
        aurEff->GetSpellModifier()->SetSpellSchool(GetAura()->GetSpellInfo()->GetSchoolMask());
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_elk_tempo_dash_aura::Apply, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_elk_spell_power_mana_cost_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_spell_power_mana_cost_aura);

    void Apply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        GetAura()->GetEffect(EFFECT_0)->SetEnabled(true);
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_elk_spell_power_mana_cost_aura::Apply, EFFECT_0, SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_elk_generic_finale : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_generic_finale);
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
    void SpellBegin()
    {
    }
    virtual void AttackUnique()
    {

    }
    void AttackHit()
    {
        uint32 hitSpell = 0;
        switch (GetSpell()->m_spellInfo->Id)
        {
        case CRUSHING_WAVE:
            hitSpell = CRUSHING_WAVE_HIT;
            break;
        }
        if (!hitSpell)
            return;
        GetCaster()->CastSpell(GetCaster(), hitSpell, false);
        AttackUnique();

        GetCaster()->SetAuraStack(ELKS(COMBO_COUNT), GetCaster(), std::min(comboLength, uint8(6)));
        if (comboLength > 6)
            comboLength -= 6;
        else
            comboLength = 0;

        auto curAtk = GetCaster()->GetCurrentSpell(CURRENT_GENERIC_SPELL);
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
        BeforeSpellLoad += SpellCastFn(spell_elk_generic_finale::SpellClick);
        BeforeCast += SpellCastFn(spell_elk_generic_finale::SpellBegin);
        AfterFullChannel += SpellCastFn(spell_elk_generic_finale::AttackHit);
    }

};

class spell_elk_generic_finale_hit : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_generic_finale_hit);

    bool WasInAir = false;
    uint8 combo = 0;
    uint32 strike = 0;

    uint8 targetsHit = 0;
    std::vector<Unit*> targetsHitVec;

     
    void SpellBegin()
    {
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
            AttackBegin();
        }
        else
        {
            GetSpell()->skip = true;
            GetCaster()->AddSpellCooldown(GetSpell()->m_spellInfo->Id, 0, 100, false);
        }
    }
    void SpellHit()
    {
        Unit* victim = GetHitUnit();

        CalcDamageInfo damageInfo;
        GetCaster()->CalculateMeleeDamage(victim, &damageInfo);
        damageInfo.hitOutCome = MELEE_HIT_CRIT;
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION;


        GetCaster()->ModifyPower(POWER_MANA, GetCaster()->GetStat(STAT_SPIRIT));
        if (WasInAir)
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_AURA_DURATION, 2000);
            GetCaster()->CastCustomSpell(ELKS(ATTACK_SLOW_DEBUFF), values, GetCaster(), TRIGGERED_FULL_MASK);
        }
        else
        {
            GetCaster()->CastSpell(GetHitUnit(), ELKS(ATTACK_SLOW_DEBUFF), true);
        }

        targetsHit++;

        damageInfo.damages[0].damage *= combo;
        Unit::DealDamageMods(victim, damageInfo.damages[0].damage, &damageInfo.damages[0].absorb);
        GetCaster()->PlayDistanceSound(129);

        GetCaster()->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        dmgInfo.SetSpellInfo(GetSpellInfo());
        GetCaster()->ProcDamageAndSpell(GetCaster(), damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damages[0].damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);
        GetCaster()->DoOnAttackHitScripts(victim, dmgInfo);
    }
    void SpellFinish()
    {
        if (!(GetCaster()))
            return;

        AfterAttack();


        if (targetsHit)
        {
            Aura* aura = GetCaster()->GetAura(ELKS(COMBO_COUNT));
            if (!aura)
                GetCaster()->AddAura(ELKS(COMBO_COUNT), GetCaster());
            else
                aura->SetStackAmount(aura->GetStackAmount() + 1);
        }

        switch (strike)
        {
        default:
            break;
        }
    }
    void Register() override
    {
        BeforeCast += SpellCastFn(spell_elk_generic_finale_hit::SpellBegin);
        AfterCast += SpellCastFn(spell_elk_generic_finale_hit::SpellFinish);
        AfterHit += SpellHitFn(spell_elk_generic_finale_hit::SpellHit);
    }
};


void AddSC_elk_spell_scripts()
{

    RegisterSpellScript(spell_elk_attack);
    RegisterSpellScript(spell_elk_spin_attack);
    RegisterSpellScript(spell_elk_attack_hit);
    RegisterSpellScript(spell_elk_critical_attack_hit);
    RegisterSpellScript(spell_elk_spin_attack_hit);
    RegisterSpellAndAuraScriptPair(spell_elk_deflect, spell_elk_deflect_aura);
    RegisterSpellScript(spell_elk_double_jump);
    RegisterSpellScript(spell_elk_sprint_aura);
    RegisterSpellScript(spell_elk_dash_aura);
    RegisterSpellScript(spell_elk_tempo_dash_aura);
    RegisterSpellScript(spell_elk_spell_power_mana_cost_aura);
    RegisterSpellScript(spell_elk_generic_finale);
    RegisterSpellScript(spell_elk_generic_finale_hit);

    RegisterSpellScript(spell_combo_counter_aura);
    RegisterSpellScript(spell_extension_system);
}
