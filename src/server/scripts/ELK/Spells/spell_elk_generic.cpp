#include "spell_elk_include.h"











class spell_elk_attack_hit : public SpellScript
{
    PrepareSpellScript(spell_elk_attack_hit);

    bool WasInAir = false;
    std::map<MapDummy, std::optional<std::any>> spellMap;
    int32 dummies = 0;
    Unit* caster;
    void SpellBegin()
    {
        Spell* spell = GetSpell();
        caster = GetCaster();
        if (caster->isMoving())
        {
            int dodge = caster->GetRealDodge() / 2;
            spell->ModifySpellValue(SPELLVALUE_RADIUS_MOD, 100 * dodge);
        }
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
        if (spellMap[MapDummy::TriggeringSpell].has_value())
        {
            dummies = std::any_cast<uint32>(spellMap[MapDummy::TriggeringSpell].value());
        }
        
    }
    void SpellHit()
    {
        Unit* victim = GetHitUnit();

        if (!(caster && victim))
        {
            return;
        }
        SetHitDamage(0);
        CalcDamageInfo damageInfo;
        caster->CalculateMeleeDamage(victim, 0, &damageInfo);
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION;
        Unit::DealDamageMods(victim, damageInfo.damage, &damageInfo.absorb);
        caster->SendAttackStateUpdate(&damageInfo);

        caster->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        caster->ProcDamageAndSpell(damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);

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
            caster->CastSpell(victim, ATTACK_SLOW_DEBUFF, true);
        }
        switch (dummies)

        {
        case 0:
            break;
            break;
        case SPELL_UNDEATH_STRIKE:
        {
            caster->CastSpell(victim, 1, true);
            Aura* festeringStacks = victim->GetAura(1);
            if (festeringStacks)
            {
                int stacks = festeringStacks->GetStackAmount();
                if (stacks >= 6)
                {
                    caster->DealDamage(caster, victim, stacks * 300);
                    victim->RemoveAura(festeringStacks);
                    caster->CastSpell(victim, 1, true);
                }
            }
            break;
        }
        }
    }
    void SpellFinish()
    {
        if (!(caster))
            return;

        Spell* spell = GetSpell();
        spellMap = spell->GetTriggerDummy();


        switch (dummies)
        {
        case SPELL_CRUSADER_STRIKE:
            caster->CastSpell(caster, 1, true); // cast a spell i guess
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

class spell_elk_deflect_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_deflect_aura);

    Unit* caster;
    bool success = false;
    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        caster = GetTarget();
        Unit* attacker = dmgInfo.GetAttacker();
        Position pos1 = caster->GetPosition();
        Position pos2 = attacker->GetPosition();
        if (abs(pos1.GetRelativeAngle(pos2.GetPositionX(), pos2.GetPositionY())) < 0.7)
        {
            if (!success)
            {
                Aura* aura = GetAura();
                success = true;
                Aura* auraFat = caster->GetAura(DEFLECT_FATIGUE);
                if (auraFat)
                {
                    auraFat->SetDuration(auraFat->GetDuration() - 500);
                }
                Player* player = dynamic_cast<Player*>(caster);
                if (player)
                {
                    player->RemoveSpellCooldown(SPELL_DEFLECT, true);
                    auto auraMap = aura->GetTriggerDummy();
                    auraMap.emplace(MapDummy::Misc_1, std::optional<bool>(true));
                }
            }
            if (caster->GetUnitMovementFlags() & MOVEMENTFLAG_FALLING)
            {
                //caster->CastSpell(caster, SPELL_ACTION_AIR_DEFLECT_HIT, true);
            }
            caster->PlayDirectSound(20000);
            dmgInfo.ModifyDamage(-1 * dmgInfo.GetDamage());
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
        if (!success)
        {
            caster = GetTarget();
            Aura* auraFat = caster->GetAura(DEFLECT_FATIGUE);
            if (!auraFat)
            {
                caster->CastSpell(caster, DEFLECT_FATIGUE, true);
                auraFat = caster->GetAura(DEFLECT_FATIGUE);
                if (auraFat)
                    auraFat->SetDuration(auraFat->GetDuration() + 2000);
                else
                    return;
            }
            else
                auraFat->SetDuration(auraFat->GetDuration() + 3000);
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
    }
    void Register() override
    {
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
        auto caster = GetCaster();
        if (caster->HasAura(100016))
            return;
        auto spell = GetSpell();
        spell->ModifyDamage(spell->m_spellInfo->Effects[0].BasePoints * 1.25);
        spell->EffectRoll(EFFECT_0, caster->GetSpeed(MOVE_RUN) - 7);
        caster->AddAura(100016, caster);
    }
    void Register() override
    {

        AfterCast += SpellCastFn(spell_elk_double_jump::DoubleJump);
    }
};

class spell_elk_rush : public SpellScript
{
    PrepareSpellScript(spell_elk_rush);

    void Rush()
    {
        auto caster = GetCaster();
        auto spell = GetSpell();
        spell->EffectRoll(EFFECT_0);
    }
    void Register() override
    {
        AfterCast += SpellCastFn(spell_elk_rush::Rush);
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
        float facing = abs(caster->NormalizeOrientation(caster->GetOldOrientation()) - caster->NormalizeOrientation(caster->GetOrientation()));
        float x, y, z = 0;
        caster->GetOldPosition(x, y, z);
        if (facing < 0.7)
        {
            float angle = abs(Position(x, y, z, caster->GetOldOrientation()).GetRelativeAngle(caster->GetPositionX(), caster->GetPositionY()));
            if (angle < 0.7)
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
    }
    void MovePacket()
    {
        float x, y, z = 0;
        caster->GetOldPosition(x, y, z);
        float degree = abs(Position(x, y, z, 0).GetRelativeAngle(caster->GetPositionX(), caster->GetPositionY()));
        if ((degree > 2.35 && degree < 3.926991) || (degree > 0.78 && degree < 5.49))
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


void AddSC_elk_spell_scripts()
{
    RegisterSpellScript(spell_elk_attack);
    RegisterSpellScript(spell_elk_attack_hit);
    RegisterSpellAndAuraScriptPair(spell_elk_deflect, spell_elk_deflect_aura);
    RegisterSpellScript(spell_elk_double_jump);
    RegisterSpellScript(spell_elk_rush);
    RegisterSpellScript(spell_elk_sprint_aura);
    RegisterSpellScript(spell_elk_dash_aura);
    

}
