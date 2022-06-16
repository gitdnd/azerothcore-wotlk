#include "spell_elk_include.h"










class spell_elk_attack_hit : public SpellScript
{
    PrepareSpellScript(spell_elk_attack_hit);

    bool WasInAir = false;
    std::map<MapDummy, std::optional<std::any>> spellMap;
    int32 dummies = 0;
    void SpellBegin()
    {
        spellMap = GetSpell()->GetTriggerDummy();
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
        Unit* caster  = GetCaster();
        Unit* victim = GetHitUnit();

        if (!(caster && victim))
        {
            return;
        }

        CalcDamageInfo damageInfo;
        caster->CalculateMeleeDamage(victim, 0, &damageInfo);
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION;
        Unit::DealDamageMods(victim, damageInfo.damage, &damageInfo.absorb);
        caster->SendAttackStateUpdate(&damageInfo);

        caster->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        caster->ProcDamageAndSpell(damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);


        if (damageInfo.damage <= 0)
        {
            return;
        }
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
        Unit* caster = GetCaster();
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
    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* caster = GetTarget();
        Unit* attacker = dmgInfo.GetAttacker();

        if (caster->GetUnitMovementFlags() & MOVEMENTFLAG_FALLING)
        {
            //caster->CastSpell(caster, SPELL_ACTION_AIR_DEFLECT_HIT, true);
        }
        caster->PlayDirectSound(18023);
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
            caster->ModifyPower(POWER_RUNIC_POWER, (dmgInfo.GetDamage() * caster->GetStat(STAT_STAMINA)) / 1000);
        }
        Player* player = dynamic_cast<Player*>(caster);
        player->RemoveSpellCooldown(SPELL_DEFLECT, true);
    }

    void Register() override
    {
        //OnEffectAbsorb += AuraEffectAbsorbFn(spell_elk_deflect_aura::Absorb, EFFECT_1);
    }
};
class spell_elk_deflect : public SpellScript
{
    PrepareSpellScript(spell_elk_deflect);
    Unit* caster;
    bool alreadyDeflecting = false; // if you cast while deflecting
    bool deflectedSomething = false; // this goes into aura, if you cast AND overlap with already deflecting after having blocked, you dont get the malus or antispam cooldown.
    bool triggeredCast = false; // prevents triggered casts from firing infinitely, should be handled by a dummy.
    /*
    DummyDeflect:
        If you're not deflecting and cast this, cast RealDeflect.
        If Deflect Fatigue goes above your parry rating (100 is 1s), deflect goes on CD equal to amount over fatigue.
        If you've deflected with RealDeflect at least once, ignore the rest and reduce Deflect Fatigue by 5s.
        If you're deflecting with over 0.1s remaining and cast this, cast RealDeflect with 0.2s time and put DummyDeflect on a .5s CD.
        If you failed to deflect, add 5s to your Deflect Fatigue.
        If you deflect in air you get a momentum reset/pause. getting hit mid air also does this.
        Test setting movespeed on attack start to 0% then every tick raising it back to 100%
        Or starting with 100% move speed in a tick and seting it back right after. there's a spell aura for ignoring move speed affecting stuff, make the dummy cast that as part of the first tick of the event then remove it the tick after.
        Reminds me, add items that on loot give a buff or on collect 20 give a buff using hidden stacking buffs. mostly in starting zones so loot matters. every piece of loot/vendoring needs to matter. spread them around, repeat NPCs in many places for the sake of story.
        Each NPC especially class trainers have dialogue.
        */
    void SpamCheck()
    {
        caster = GetCaster();
        alreadyDeflecting = caster->HasAura(SPELL_DEFLECT);
        Spell* spell = GetSpell();
        auto spellMap = spell->GetTriggerDummy();
        int trigger = 0;
        if (spellMap[MapDummy::TriggeringSpell].has_value())
        {
            trigger = std::any_cast<int>(spellMap[MapDummy::TriggeringSpell].value());
        }
        if (spellMap[MapDummy::Misc_1].has_value())
        {
            deflectedSomething = std::any_cast<bool>(spellMap[MapDummy::Misc_1].value());
        }
        if (alreadyDeflecting and trigger != 0)
        {
            spell->cancel();
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_SPELL_TIME, 200);
            caster->CastCustomSpell(SPELL_DEFLECT, values, caster,
                TRIGGERED_NONE, nullptr, nullptr, ObjectGuid::Empty,
                std::map<MapDummy, std::optional<std::any>>{ { MapDummy::TriggeringSpell, std::optional(1) }
        } );
            caster->AddSpellCooldown(SPELL_DEFLECT, 0, 500);
        }
    } 
    void SpellFinish()
    {
        caster->SetSheath(SHEATH_STATE_MELEE);
        if (caster->GetUnitMovementFlags() & MOVEMENTFLAG_FALLING)
        {
            //caster->CastSpell(caster, SPELL_ACTION_AIR_DEFLECT, true);
        }
    }

    void Register() override
    {
        
        BeforeCastTime += SpellCastFn(spell_elk_deflect::SpamCheck);
        AfterCast += SpellCastFn(spell_elk_deflect::SpellFinish);
    }
};
 

void SC_elk_include_scripts();

void AddSC_elk_spell_scripts()
{
    RegisterSpellScript(spell_elk_attack_hit);
    RegisterSpellAndAuraScriptPair(spell_elk_deflect, spell_elk_deflect_aura);

    SC_elk_include_scripts();

}
