#include "spell_elk_include.h"











class spell_elk_attack_hit : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_attack_hit);

    bool WasInAir = false;
    std::map<MapDummy, std::optional<std::any>> spellMap;
    uint32 strike = 0;
    Unit* caster;
    uint8 targetsHit = 0;
    void SpellBegin()
    {
        Spell* spell = GetSpell();
        caster = GetCaster(); 
        strike = caster->GetStrikeAura();
        spellMap = spell->GetTriggerDummy();
        auto& inAir = spellMap[MapDummy::WasInAir];
        if (inAir.has_value())
        {
            if (std::any_cast<bool>(inAir.value()) == true)
            {
                WasInAir = true;
            }
            else
            {
                WasInAir = false;
            }
        }
        else
            WasInAir = false;
        AttackBegin();
    }
    void SpellHit()
    { 
        AttackHit();

        targetsHit++;

        Spell* spell = GetSpell();
        caster->ModifyPower(POWER_MANA, caster->GetStat(STAT_SPIRIT));
        if (WasInAir)
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_AURA_DURATION, 2000);
            caster->CastCustomSpell(ATTACK_SLOW_DEBUFF, values, caster, TRIGGERED_FULL_MASK);
        }
        else
        {
            caster->CastSpell(GetHitUnit(), ATTACK_SLOW_DEBUFF, true);
        }
        switch (strike)

        {
        default:
            break; 
        }
    }
    void SpellFinish()
    {
        if (!(caster))
            return;

        AfterAttack(); 


        if (targetsHit)
        {
            auto& optComboLength = spellMap[MapDummy::ComboLength];
            if (optComboLength.has_value())
                GetCaster()->SetAuraStack(COMBO_COUNT, GetCaster(), std::any_cast<uint8>(optComboLength.value()) + 1);
            else
                caster->AddAura(COMBO_COUNT, caster);
            caster->ModCritTempo(uint16(caster->GetFloatValue(PLAYER_CRIT_PERCENTAGE) / (caster->GetFloatValue(static_cast<uint16>(UNIT_FIELD_BASEATTACKTIME))/1000)));
        }

        switch (strike)
        {
        case SPELL_CRUSADER_STRIKE:
        case SPELL_CRUSADER_STRIKE_BASIC:
        case SPELL_CRUSADER_STRIKE_EXPERT:
            if (targetsHit)
            {
                uint8 devBonus = 0;
                if (Player* player = caster->ToPlayer())
                    devBonus = player->GetSpellData(SPELL_CRUSADER_STRIKE).development;
                if (Aura* aura = caster->GetAura(SPELL_HOLY_POWER); aura && aura->GetStackAmount() >= 2 + devBonus)
                {
                }
                else
                    caster->CastSpell(caster, 210004, true);
            }
            break;
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

    bool WasInAir = false;
    std::map<MapDummy, std::optional<std::any>> spellMap;
    uint32 strike = 0;
    Unit* caster;
    uint8 targetsHit = 0;
    std::vector<Unit*> targetsHitVec;

    void SpellBegin()
    {
        Spell* spell = GetSpell();
        caster = GetCaster();
        strike = caster->GetStrikeAura();
        spellMap = spell->GetTriggerDummy();
        auto& inAir = spellMap[MapDummy::WasInAir];
        if (inAir.has_value())
        {
            if (std::any_cast<bool>(inAir.value()) == true)
            {
                WasInAir = true;
            }
            else
            {
                WasInAir = false;
            }
        }
        else
            WasInAir = false;
        AttackBegin();
    }
    void SpellHit()
    {
        Unit* victim = GetHitUnit();

        CalcDamageInfo damageInfo;
        GetCaster()->CalculateMeleeDamage(victim, 0, &damageInfo);
        damageInfo.hitOutCome = MELEE_HIT_CRIT;
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION;
        damageInfo.damage *= 2;

        Spell* spell = GetSpell();
        caster->ModifyPower(POWER_MANA, caster->GetStat(STAT_SPIRIT));
        if (WasInAir)
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_AURA_DURATION, 2000);
            caster->CastCustomSpell(ATTACK_SLOW_DEBUFF, values, caster, TRIGGERED_FULL_MASK);
        }
        else
        {
            caster->CastSpell(GetHitUnit(), ATTACK_SLOW_DEBUFF, true);
        }

        targetsHit++;
        switch (strike)
        {
        case SPELL_CRUSADER_STRIKE:
        case SPELL_CRUSADER_STRIKE_EXPERT:
            targetsHitVec.push_back(victim);
            return;
            break;
        default:
            break;

        }
        Unit::DealDamageMods(victim, damageInfo.damage, &damageInfo.absorb);
        GetCaster()->PlayDistanceSound(129);

        GetCaster()->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        dmgInfo.SetSpellInfo(GetSpellInfo());
        GetCaster()->ProcDamageAndSpell(damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);
        GetCaster()->DoOnAttackHitScripts(victim, dmgInfo);
    }
    void SpellFinish()
    {
        if (!(caster))
            return;

        AfterAttack();


        if (targetsHit)
        {
            auto& optComboLength = spellMap[MapDummy::ComboLength];
            if (optComboLength.has_value())
                GetCaster()->SetAuraStack(COMBO_COUNT, GetCaster(), std::any_cast<uint8>(optComboLength.value()) + 1);
            else
                caster->AddAura(COMBO_COUNT, caster);
        }

        switch (strike)
        {
        case SPELL_CRUSADER_STRIKE:
        case SPELL_CRUSADER_STRIKE_EXPERT:
            if (targetsHit)
            {
                if(caster->HasAura(210015))
                    caster->CastSpell(caster, 210016, true);

                for (auto victim : targetsHitVec)
                {
                    CalcDamageInfo damageInfo;
                    damageInfo.damageSchoolMask = SPELL_SCHOOL_MASK_HOLY;
                    GetCaster()->CalculateMeleeDamage(victim, 0, &damageInfo);
                    damageInfo.HitInfo |= HITINFO_NO_ANIMATION;
                    damageInfo.damage *= (2.f / float(targetsHit));
                    Unit::DealDamageMods(victim, damageInfo.damage, &damageInfo.absorb);

                    GetCaster()->DealMeleeDamage(&damageInfo, true);

                    DamageInfo dmgInfo(damageInfo);
                    dmgInfo.SetSpellInfo(GetSpellInfo());
                    GetCaster()->ProcDamageAndSpell(damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damage,
                        damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);
                    GetCaster()->DoOnAttackHitScripts(victim, dmgInfo);
                     
                }
            }
            break;
        }
    }
    void Register() override
    {
        BeforeCast += SpellCastFn(spell_elk_critical_attack_hit::SpellBegin);
        AfterCast += SpellCastFn(spell_elk_critical_attack_hit::SpellFinish);
        AfterHit += SpellHitFn(spell_elk_critical_attack_hit::SpellHit);
    }
};

class spell_elk_spin_attack_hit : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_spin_attack_hit);

    bool WasInAir = false;
    std::map<MapDummy, std::optional<std::any>> spellMap;
    uint32 strike = 0;
    Unit* caster;
    uint8 targetsHit = 0;
    std::vector<Unit*> targetsHitVec;

    void SpellBegin()
    {
        Spell* spell = GetSpell();
        caster = GetCaster();
        strike = caster->GetStrikeAura();
        spellMap = spell->GetTriggerDummy();
        auto& inAir = spellMap[MapDummy::WasInAir];
        if (inAir.has_value())
        {
            if (std::any_cast<bool>(inAir.value()) == true)
            {
                WasInAir = true;
            }
            else
            {
                WasInAir = false;
            }
        }
        else
            WasInAir = false;
        AttackBegin();
    }
    void SpellHit()
    {
        Unit* victim = GetHitUnit();

        CalcDamageInfo damageInfo;
        GetCaster()->CalculateMeleeDamage(victim, 0, &damageInfo);
        damageInfo.hitOutCome = MELEE_HIT_CRIT;
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION;
        damageInfo.damage *= 2;

        Spell* spell = GetSpell();
        caster->ModifyPower(POWER_MANA, caster->GetStat(STAT_SPIRIT));
        if (WasInAir)
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_AURA_DURATION, 2000);
            caster->CastCustomSpell(ATTACK_SLOW_DEBUFF, values, caster, TRIGGERED_FULL_MASK);
        }
        else
        {
            caster->CastSpell(GetHitUnit(), ATTACK_SLOW_DEBUFF, true);
        }

        targetsHit++;
        switch (strike)
        {
        default:
            break;

        }
        Unit::DealDamageMods(victim, damageInfo.damage, &damageInfo.absorb);
        GetCaster()->PlayDistanceSound(129);

        GetCaster()->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        dmgInfo.SetSpellInfo(GetSpellInfo());
        GetCaster()->ProcDamageAndSpell(damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);
        GetCaster()->DoOnAttackHitScripts(victim, dmgInfo);
    }
    void SpellFinish()
    {
        if (!(caster))
            return;

        AfterAttack();


        if (targetsHit)
        {
            auto& optComboLength = spellMap[MapDummy::ComboLength];
            if (optComboLength.has_value())
                GetCaster()->SetAuraStack(COMBO_COUNT, GetCaster(), std::any_cast<uint8>(optComboLength.value()) + 1);
            else
                caster->AddAura(COMBO_COUNT, caster);
            caster->ModCritTempo(uint16(caster->GetFloatValue(PLAYER_CRIT_PERCENTAGE) / (caster->GetFloatValue(static_cast<uint16>(UNIT_FIELD_BASEATTACKTIME)) / 1000)));
        }

        switch (strike)
        {
        default:
            break;
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

    Unit* caster;
    bool success = false;
    uint8 comboLength = 0;

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        caster = GetTarget();
        if (auto aura = caster->GetAura(COMBO_COUNT))
            comboLength = aura->GetStackAmount();
    }
    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* attacker = dmgInfo.GetAttacker();
        Position pos1 = caster->GetPosition();
        Position pos2 = attacker->GetPosition();
        float angle = caster->NormalizeOrientation(pos1.GetRelativeAngle(pos2.GetPositionX(), pos2.GetPositionY()));
        if (angle < 0.7 || angle > 5.585)
        {
            if (!success)
            {
                Aura* aura = GetAura();
                success = true; 
                Player* player = caster->ToPlayer();
                if (player)
                {
                    auto auraMap = aura->GetTriggerDummy();
                    auraMap.emplace(MapDummy::Misc_1, std::optional<bool>(true));
                }
            }
            if (caster->GetUnitMovementFlags() & MOVEMENTFLAG_FALLING)
            {
                //caster->CastSpell(caster, SPELL_ACTION_AIR_DEFLECT_HIT, true);
            }
            caster->PlayDirectSound(20000);
            dmgInfo.AbsorbDamage(-1 * dmgInfo.GetDamage());
            if (attacker)
            {
                //caster->CastSpell(attacker, SPELL_ACTION_POSTURE_DAMAGE, true);
                //Aura* postureStacaks = attacker->GetAura(SPELL_ACTION_POSTURE_DAMAGE);
                /*if (postureStacaks)
                {
                    int stacks = postureStacaks->GetStackAmount();
                    if (stacks >= 10)
                    {
                        attacker->RemoveAura(postureStacaks);
                        caster->CastSpell(attacker, SPELL_ACTION_POSTURE_STUN, true);
                    }
                }*/
                caster->ModifyPower(POWER_MANA, (dmgInfo.GetDamage() * caster->GetStat(STAT_SPIRIT)) / 1000);
            }
        }
    }
    void OnEnd(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (success)
        {
            for (int i = 0; i < MAX_RUNES; i++)
            {
                uint32 cd = caster->GetRuneCooldown(i);
                caster->SetRuneCooldown(i, (cd > 15000) ? cd - 15000 : 0);
            }

            auto auraCombo = caster->AddAura(COMBO_COUNT, caster);
            if (auraCombo)
            {
                auraCombo->SetStackAmount(comboLength + 1);
            }
        }
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_elk_deflect_aura::OnApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_elk_deflect_aura::Absorb, EFFECT_0);
        OnEffectRemove += AuraEffectRemoveFn(spell_elk_deflect_aura::OnEnd, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};
class spell_elk_deflect : public SpellScript
{
    PrepareSpellScript(spell_elk_deflect);
    Unit* caster;
    bool success = false; 
    void SpamCheck()
    {

        caster = GetCaster();
        caster->SetSheath(SHEATH_STATE_MELEE);
        if (caster->GetUnitMovementFlags() & MOVEMENTFLAG_FALLING)
        {
            //caster->CastSpell(caster, SPELL_ACTION_AIR_DEFLECT, true);
        }
        Spell* spell = GetSpell();
        spell->SetRuneCooldown(15000);
        spell->SetRuneCost(1);
        Aura* aura = caster->GetAura(SPELL_DEFLECT);
        if (aura)
        {
            auto spellMap = aura->GetTriggerDummy();
            if (spellMap[MapDummy::Misc_1].has_value())
            {
                success = std::any_cast<bool>(spellMap[MapDummy::Misc_1].value());
            }
            if (!success and spell->GetSpellInfo()->Id == SPELL_DEFLECT and aura->GetDuration() > 100)
            {
                
                spell->cancel();
                aura->Remove();
                caster->CastSpell(caster, SPELL_DEFLECT_SHORT, false);
                /*
                Aura* auraFat = caster->GetAura(DEFLECT_FATIGUE);
                if (auraFat)
                {
                    auraFat->SetDuration(auraFat->GetDuration() - 3000);
                    Player* player = dynamic_cast<Player*>(caster);
                    if (player)
                    {
                        int dur = auraFat->GetDuration();
                        int def = player->GetRatingBonusValue(CR_PARRY);
                        int dif = dur - def;
                        if (dif > 0)
                        {
                            player->AddSpellCooldown(SPELL_DEFLECT, 0, dif);
                        }
                    }
                }
                */
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
        auto caster = GetCaster()->ToPlayer();
        if (!caster || !(caster->CanDoubleJump()))
            return;
        auto spell = GetSpell();
        spell->ModifyDamage(spell->m_spellInfo->Effects[0].BasePoints * 1.25);
        spell->EffectRoll(EFFECT_0, caster->GetSpeed(MOVE_RUN) - 7);
        caster->AddDoubleJump();
    }
    void Register() override
    {

        AfterCast += SpellCastFn(spell_elk_double_jump::DoubleJump);
    }
};

class spell_elk_rush_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_rush_aura);

    Player* caster = nullptr;
    void SpellApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetCaster()->IsPlayer())
            caster = GetCaster()->ToPlayer();
        else
            GetAura()->Remove();
    }
    void MovePacket()
    {
        if (!caster)
            return;
        if (caster->IsFalling())
            return;
        else
        {
            GetAura()->Remove();
        }
    };
    void RushEnd(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        auto caster = GetCaster();
        caster->CastSpell(caster, DISENGAGE_MINI_HOP, true); 
    }  
    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_elk_rush_aura::RushEnd, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnMovementPacket += OnMovementPacketFn(spell_elk_rush_aura::MovePacket);
        OnEffectApply += AuraEffectApplyFn(spell_elk_rush_aura::SpellApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_elk_sprint_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_sprint_aura);
    Unit* caster;
    void Cast(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes  /*mode*/)
    {
        caster = GetCaster();
    }
    void MovePacket()
    {
        if (caster->IsFalling())
            return;
        float facing = caster->NormalizeOrientation(abs(caster->GetOldOrientation()) - caster->GetOrientation());
        float x, y, z = 0;
        caster->GetOldPosition(x, y, z);
        float weightBonus = std::max(0.0f, (1.2f - caster->GetWeight()));
        if (facing < 0.5f + weightBonus || facing > 5.98f - weightBonus)
        {
            float angle = caster->NormalizeOrientation(Position(x, y, z, caster->NormalizeOrientation(caster->GetOldOrientation())).GetRelativeAngle(caster->GetPositionX(), caster->GetPositionY()));
            if (angle < 0.7f || angle > 5.585f)
            {
                return;
            }
        } 
        caster->RemoveAura(GetAura());
    }
    void EnergyCheck(AuraEffect const*  /*aurEff*/)
    {
        if(caster->GetPower(POWER_ENERGY) < abs(GetSpellInfo()->Effects[1].BasePoints))
            caster->RemoveAura(GetAura());
    }
    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elk_sprint_aura::EnergyCheck, EFFECT_1, SPELL_AURA_PERIODIC_ENERGIZE);
        OnEffectApply += AuraEffectApplyFn(spell_elk_sprint_aura::Cast, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
        OnMovementPacket += OnMovementPacketFn(spell_elk_sprint_aura::MovePacket);
    }
};
class spell_elk_dash_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_dash_aura);
    Unit* caster;
    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        dmgInfo.ModifyDamage(-1 * dmgInfo.GetDamage());
    }
    void Cast(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes  /*mode*/)
    {
        caster = GetCaster();
        caster->CastSpell(caster, 100042, true);
    }
    void MovePacket()
    {
        float x, y, z = 0;
        caster->GetOldPosition(x, y, z);
        float degree = caster->NormalizeOrientation(Position(x, y, z, caster->NormalizeOrientation(caster->GetOldOrientation())).GetRelativeAngle(caster->GetPositionX(), caster->GetPositionY()));
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

class spell_elk_escape : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_escape);
     
    void SpellBegin()
    {
        Spell* spell = GetSpell();
        spell->SetRuneCost(1);
        uint16 cd = 1000;
        for (auto aura : GetCaster()->GetAppliedAuras())
        {
            if (aura.second->GetBase()->IsPermanent())
                continue;
            if (!(aura.second->IsPositive()))
            {
                if (aura.second->GetBase()->GetSpellInfo()->GetAllEffectsMechanicMask() & IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK)
                {
                    cd += aura.second->GetBase()->GetDuration();
                    GetCaster()->RemoveAura(aura.second);
                    continue;
                }
            }
        }
        spell->SetRuneCooldown(cd);
    } 
    void Register() override
    {
        BeforeSpellLoad += SpellCastFn(spell_elk_escape::SpellBegin);
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
        uint32 hitSpell = 0;
        switch (GetSpell()->m_spellInfo->Id)
        {
        case CRUSHING_WAVE:
            hitSpell = CRUSHING_WAVE_HIT;
            break;
        }
        if (!hitSpell)
            return;
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
        BeforeSpellLoad += SpellCastFn(spell_elk_generic_finale::SpellClick);
        BeforeCast += SpellCastFn(spell_elk_generic_finale::SpellBegin);
        AfterFullChannel += SpellCastFn(spell_elk_generic_finale::AttackHit);
    }

};

class spell_elk_generic_finale_hit : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_generic_finale_hit);

    bool WasInAir = false;
    std::map<MapDummy, std::optional<std::any>> spellMap;
    uint32 strike = 0;
    Unit* caster;
    uint8 targetsHit = 0;
    std::vector<Unit*> targetsHitVec;
    uint8 combo = 0;
     
    void SpellBegin()
    {
        Spell* spell = GetSpell();
        caster = GetCaster();

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
            spell->SetRuneCost(1);
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
        GetCaster()->CalculateMeleeDamage(victim, 0, &damageInfo);
        damageInfo.hitOutCome = MELEE_HIT_CRIT;
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION;

        Spell* spell = GetSpell();
        caster->ModifyPower(POWER_MANA, caster->GetStat(STAT_SPIRIT));
        if (WasInAir)
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_AURA_DURATION, 2000);
            caster->CastCustomSpell(ATTACK_SLOW_DEBUFF, values, caster, TRIGGERED_FULL_MASK);
        }
        else
        {
            caster->CastSpell(GetHitUnit(), ATTACK_SLOW_DEBUFF, true);
        }

        targetsHit++;
        switch (strike)
        {
        default:
            break;

        }
        damageInfo.damage *= combo;
        Unit::DealDamageMods(victim, damageInfo.damage, &damageInfo.absorb);
        GetCaster()->PlayDistanceSound(129);

        GetCaster()->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        dmgInfo.SetSpellInfo(GetSpellInfo());
        GetCaster()->ProcDamageAndSpell(damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);
        GetCaster()->DoOnAttackHitScripts(victim, dmgInfo);
    }
    void SpellFinish()
    {
        if (!(caster))
            return;

        AfterAttack();


        if (targetsHit)
        {
            auto& optComboLength = spellMap[MapDummy::ComboLength];
            if (optComboLength.has_value())
                GetCaster()->SetAuraStack(COMBO_COUNT, GetCaster(), std::any_cast<uint8>(optComboLength.value()) + 1);
            else
                caster->AddAura(COMBO_COUNT, caster);
            caster->ModCritTempo(uint16(caster->GetFloatValue(PLAYER_CRIT_PERCENTAGE) / (caster->GetFloatValue(static_cast<uint16>(UNIT_FIELD_BASEATTACKTIME)) / 1000)));
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
    RegisterSpellScript(spell_elk_strike_aura);
    RegisterSpellScript(spell_elk_attack);
    RegisterSpellScript(spell_elk_critical_attack);
    RegisterSpellScript(spell_elk_spin_attack);
    RegisterSpellScript(spell_elk_attack_hit);
    RegisterSpellScript(spell_elk_critical_attack_hit);
    RegisterSpellScript(spell_elk_spin_attack_hit);
    RegisterSpellAndAuraScriptPair(spell_elk_deflect, spell_elk_deflect_aura);
    RegisterSpellScript(spell_elk_double_jump);
    RegisterSpellScript(spell_elk_rush_aura);
    RegisterSpellScript(spell_elk_sprint_aura);
    RegisterSpellScript(spell_elk_escape);
    RegisterSpellScript(spell_elk_dash_aura);
    RegisterSpellScript(spell_elk_tempo_dash_aura);
    RegisterSpellScript(spell_elk_spell_power_mana_cost_aura);
    RegisterSpellScript(spell_elk_generic_finale);
    RegisterSpellScript(spell_elk_generic_finale_hit);

}
