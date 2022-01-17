
#include "Group.h"
#include "PetDefines.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Totem.h"
#include "UnitAI.h"
#include <MoveSplineInit.h>



enum OutsideSpells
{

    SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE  = 91004,
    SPELL_PALADIN_HOLY_SHOCK_R1_HEALING = 91005,
    SPELL_DK_DEATH_COIL_DAMAGE = 90008,
    SPELL_DK_DEATH_COIL_HEAL   = 90009,
};
enum ActionSpells
{
    SPELL_ACTION_HARD_ATTACK        = 89900,
    SPELL_ACTION_HEAVY_ATTACK       = 89901,
    SPELL_ACTION_DIVINE_BLOOD       = 89985,
    SPELL_ACTION_HEALING_POTION   = 89986,
    SPELL_ACTION_POTION_FATIGUE   = 89987,
    SPELL_ACTION_POTION   = 89988,
    SPELL_ACTION_ATTACK_SLOW_DEBUFF     = 89989,
    SPELL_ACTION_POSTURE_STUN   = 89990,
    SPELL_ACTION_POSTURE_DAMAGE = 89991,
    SPELL_ACITON_QUICK_FOLLOW_UP      = 89992,
    SPELL_ACITON_QUICK_ATTACK     = 89993,
    SPELL_ACITON_ATTACK_SLOW      = 89994,
    SPELL_ACITON_ATTACK_SLOW_HEAVY  = 89995,
    SPELL_ACTION_THRUST_ATTACK        = 89996,
    SPELL_ACTION_STAB_ATTACK                = 89997,
    SPELL_ACTION_BASIC_ATTACK                   = 89998,
    SPELL_ACTION_DEFLECT                        = 89999,
    SPELL_ACTION_ATTACK                         = 90000,
    SPELL_ACTION_UNDEATH_STRIKE                 = 90001,
    SPELL_ACTION_RAISE_GHOUL                    = 90002,
    SPELL_ACTION_DEATH_COIL                     = 90004,
    SPELL_ACTION_FESTERING_PLAGUE               = 90005,
    SPELL_ACTION_DEATH_COIL_KILLS = 90006,
    SPELL_ACTION_PRESENCE_OF_SOUL       = 91001,
    SPELL_ACTION_HOLY_SHOCK  = 91002,
    SPELL_ACTION_CRUSADER_STRIKE  = 91003,
    SPELL_ACTION_BLOOD_HOWL       = 100003,
    SPELL_WOLF_FORM               = 100004,
    SPELL_ACTION_WOLF_BITE  = 100005,
    SPELL_ACTION_SHOCKWAVE_GRAPHIC = 100007,
    SPELL_ACTION_MASSIVE_SMASH = 100008,
};

class spell_action_hard_attack : public SpellScript
{
    PrepareSpellScript(spell_action_hard_attack);

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_action_hard_attack::ForceHeavy);
    }
};

class spell_action_divine_blood : public SpellScript
{
    PrepareSpellScript(spell_action_divine_blood);

    void PotionFatigue()
    {
        auto  caster        = GetCaster();
        Aura* potionFatigue   = caster->GetAura(SPELL_ACTION_POTION_FATIGUE);
        if (potionFatigue)
        {
            potionFatigue->Remove();
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_action_divine_blood::PotionFatigue);
    }
};

class spell_action_potions : public SpellScript
{
    PrepareSpellScript(spell_action_potions);

    void PotionFatigue()
    {
        auto item = GetCastItem();
        if (!item)
        {
            return;
        }
        Unit* caster = GetCaster();
        
        Aura* potionFatigue = caster->GetAura(SPELL_ACTION_POTION_FATIGUE);
        int   potionFatStacks = 0;
        if (potionFatigue)
        {
            potionFatStacks = potionFatigue->GetStackAmount();
            potionFatStacks = std::min(potionFatStacks, 10);
        }
        caster->CastSpell(caster, SPELL_ACTION_POTION_FATIGUE, true);

        auto potionEnum = 0;
        switch (item->GetEntry())
        {
        case 60002:
        {
            potionEnum          = SPELL_ACTION_HEALING_POTION;
            break;
        }
        default:
            break;
        }
        if (potionEnum)
        {
            const SpellInfo* si = sSpellMgr->GetSpellInfo(potionEnum);
            if (!si)
                return;

            caster->CastSpell(caster, SPELL_ACITON_ATTACK_SLOW, true);
            CustomSpellValues values;
            int32             effectiveness = si->Effects[0].CalcValue() * (1 - 0.1 * potionFatStacks);
            values.AddSpellMod(SPELLVALUE_BASE_POINT0, effectiveness);
            caster->CastCustomSpell(potionEnum, values, caster, TRIGGERED_NONE);
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_action_potions::PotionFatigue);
    }
};

class spell_action_quick_attack : public SpellScript
{
    PrepareSpellScript(spell_action_quick_attack);

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_action_quick_attack::ForceQuick);
    }
};

class spell_action_stab_attack : public SpellScript
{
    PrepareSpellScript(spell_action_stab_attack);
    void Register() override
    {
        BeforeCast += SpellCastFn(spell_action_stab_attack::ForceThrust);
    }
};


class spell_action_basic_attack : public SpellScript
{
    PrepareSpellScript(spell_action_basic_attack);

    void SpellFinish()
    {
        Unit* caster = GetCaster();
        if (!(caster))
            return;
        if (auto dummyRef = GetSpell()->GetTriggerDummy(); dummyRef)
        {
            uint32 dummies = std::any_cast<uint32>((*dummyRef)[0]);
            switch (dummies)
            {
            case SPELL_ACTION_CRUSADER_STRIKE:
                caster->CastSpell(caster, SPELL_ACTION_PRESENCE_OF_SOUL, true);
                break;
            }
        }
    }
    void SpellCast()
    {
        Unit* caster          = GetCaster();
        Aura* slowSpell = caster->GetAura(SPELL_ACITON_ATTACK_SLOW);
        if (slowSpell)
        {
            slowSpell->Remove();
        }
    }
    void SpellHit()
    {
        Unit* caster  = GetCaster();
        Unit* unitHit = GetHitUnit();
        if (!(caster && unitHit) || GetHitDamage() <= 0)
        {
            return;
        }
        caster->ModifyPower(POWER_MANA, caster->GetStat(STAT_SPIRIT));
        caster->CastSpell(unitHit, SPELL_ACTION_ATTACK_SLOW_DEBUFF, true);
        if (auto dummyRef = GetSpell()->GetTriggerDummy(); dummyRef)
        {
            uint32 dummies = std::any_cast<uint32>((*dummyRef)[0]);
            switch (dummies)
            {
            case SPELL_ACTION_UNDEATH_STRIKE:
            {
                caster->CastSpell(unitHit, SPELL_ACTION_FESTERING_PLAGUE, true);
                Aura* festeringStacks = unitHit->GetAura(SPELL_ACTION_FESTERING_PLAGUE);
                if (festeringStacks)
                {
                    int stacks = festeringStacks->GetStackAmount();
                    if (stacks >= 6)
                    {
                        caster->DealDamage(caster, unitHit, stacks * 300);
                        unitHit->RemoveAura(festeringStacks);
                        caster->CastSpell(unitHit, SPELL_ACTION_RAISE_GHOUL, true);
                    }
                }
                break;
            }
            case SPELL_ACTION_WOLF_BITE:
            {
                unitHit->CastSpell(unitHit, SPELL_WOLF_FORM, true);
                break;
            }
            }
        }
    }
    void Register() override
    {
        AfterCast += SpellCastFn(spell_action_basic_attack::SpellFinish);
        OnCast += SpellCastFn(spell_action_basic_attack::SpellCast);
        AfterHit += SpellHitFn(spell_action_basic_attack::SpellHit);
    }
};
class spell_action_deflect_aura : public AuraScript
{
    PrepareAuraScript(spell_action_deflect_aura);

    void SpellFinish()
    {
        Unit* unitCast = GetCaster();
        unitCast->SetSheath(SHEATH_STATE_MELEE);
    }
    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* unitCast   = GetTarget();
        Unit* unitAttacker = dmgInfo.GetAttacker();

        unitCast->PlayDirectSound(18023);
        dmgInfo.ModifyDamage(-1 * dmgInfo.GetDamage());
        if (unitAttacker)
        {
            unitCast->CastSpell(unitAttacker, SPELL_ACTION_POSTURE_DAMAGE, true);
            Aura* postureStacaks = unitAttacker->GetAura(SPELL_ACTION_POSTURE_DAMAGE);
            if (postureStacaks)
            {
                int stacks = postureStacaks->GetStackAmount();
                if (stacks >= 10)
                {
                    unitAttacker->RemoveAura(postureStacaks);
                    unitCast->CastSpell(unitAttacker, SPELL_ACTION_POSTURE_STUN, true);
                }
            }
            unitCast->ModifyPower(POWER_RUNIC_POWER, dmgInfo.GetDamage() * unitCast->GetStat(STAT_STAMINA) / 1000);

            Position tempPoint  = unitCast->GetPosition();
            Position tempPoint2 = unitAttacker->GetPosition();
            Position tempPoint3 = tempPoint;
            float    tempO      = tempPoint2.GetAbsoluteAngle(tempPoint);
            tempPoint3.SetOrientation(0);
            tempPoint3.RelocatePolarOffset(tempO, 16.f);
            float tempX = tempPoint3.GetPositionX();
            float tempY = tempPoint3.GetPositionY();
            float tempZ = tempPoint3.GetPositionZ();

            // I have no clue how to move
            // try this; disable ground movement, make it fly only then do the jump and spam it for 1s every frame.

            Player* tempPlayer = dynamic_cast<Player*>(unitCast);

            Aura* tempAura = GetAura();
            unitCast->RemoveSpellCooldown(SPELL_ACTION_DEFLECT, true);
            unitCast->CastSpell(unitCast, 89999, false);
        }
        unitCast->RemoveSpellCooldown(SPELL_ACTION_DEFLECT, true);
    }

    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_action_deflect_aura::Absorb, EFFECT_1);
    }
};
class spell_action_deflect : public SpellScript
{
    PrepareSpellScript(spell_action_deflect);

    void SpellFinish()
    {
        GetCaster()->SetSheath(SHEATH_STATE_MELEE);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_action_deflect::SpellFinish);
    }
};
class spell_action_attack : public SpellScript
{
    PrepareSpellScript(spell_action_attack);



    void Register() override
    {
        AfterCast += SpellCastFn(spell_action_attack::ForceAttack);
    }

};


class spell_action_undeath_strike : public SpellScript
{
    PrepareSpellScript(spell_action_undeath_strike);

    void Register() override
    {
        AfterCast += SpellCastFn(spell_action_undeath_strike::ForceAttack);
    }
};

class spell_action_death_coil : public SpellScript
{
    PrepareSpellScript(spell_action_death_coil);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        int32 damage = GetEffectValue();
        Unit* caster = GetCaster();
        if (Unit* target = GetHitUnit())
        {
            if (caster->IsFriendlyTo(target))
            {
                caster->CastCustomSpell(target, SPELL_DK_DEATH_COIL_HEAL, &damage, nullptr, nullptr, false);
            }
            else
            {
                caster->CastCustomSpell(target, SPELL_DK_DEATH_COIL_DAMAGE, &damage, nullptr, nullptr, false);
            }
        }
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetExplTargetUnit())
        {
            if (!caster->IsFriendlyTo(target) && !caster->isInFront(target))
                return SPELL_FAILED_UNIT_NOT_INFRONT;

            bool undead = target->HasSpell(90003);

            if (target->IsFriendlyTo(caster) && (target->GetCreatureType() != CREATURE_TYPE_UNDEAD && !undead))
                return SPELL_FAILED_BAD_TARGETS;
        }
        else
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_action_death_coil::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_action_death_coil::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class spell_dk_death_coil_damage : public SpellScript
{
    PrepareSpellScript(spell_dk_death_coil_damage);

    void SpellHit() {
        Unit* target = GetHitUnit();
        if (target->isDead())
        {
            Unit* caster = GetCaster();
            caster->CastSpell(caster, SPELL_ACTION_DEATH_COIL_KILLS, true);
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_dk_death_coil_damage::SpellHit);
    }
};

class spell_action_holy_shock : public SpellScript
{
    PrepareSpellScript(spell_action_holy_shock);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* unitTarget = GetHitUnit())
        {
            if (caster->IsFriendlyTo(unitTarget))
                caster->CastSpell(unitTarget, SPELL_PALADIN_HOLY_SHOCK_R1_HEALING, true);
            else
                caster->CastSpell(unitTarget, SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE, true);
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

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_action_holy_shock::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_action_holy_shock::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};
class spell_action_crusader_strike : public SpellScript
{
    PrepareSpellScript(spell_action_crusader_strike);

    void Register() override
    {
        AfterCast += SpellCastFn(spell_action_crusader_strike::ForceAttack);
    }
};
class spell_action_blood_howl : public SpellScript
{
    PrepareSpellScript(spell_action_blood_howl);

    void Cast()
    {
        auto             caster = GetCaster();
        auto wolfC  = dynamic_cast<Creature*>(caster);
        if (wolfC)
        {
            auto nearestWolf = wolfC->FindNearestCreature(wolfC->GetEntry(), 900);
            if (wolfC->GetVictim())
            {
                nearestWolf->GetMotionMaster()->MoveChase(wolfC->GetVictim());
                nearestWolf->SetTarget(wolfC->GetVictim()->GetGUID());
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_action_blood_howl::Cast);
    }
};

class spell_action_devouring_rage_auraa : public AuraScript
{
    PrepareAuraScript(spell_action_devouring_rage_auraa);

    void CheckProc(ProcEventInfo& eventInfo)
    {
        auto damage = eventInfo.GetDamageInfo();
        if (damage)
        {
            auto caster = GetCaster();
            if(caster)
            {
                caster->DealHeal(caster, caster, damage->GetDamage());
                caster->setAttackTimer(BASE_ATTACK, 0);
            }

        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_action_devouring_rage_auraa::CheckProc);
    }
};

class spell_action_devouring_rage : public SpellScript
{
    PrepareSpellScript(spell_action_devouring_rage);

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (caster->GetHealthPct() > 60)
        {
            return SPELL_FAILED_TRY_AGAIN;
        }
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_action_devouring_rage::CheckCast);
    }
};
class spell_action_wolf_bite : public SpellScript
{
    PrepareSpellScript(spell_action_wolf_bite);

    void Register() override
    {
        AfterCast += SpellCastFn(spell_action_wolf_bite::ForceAttack);
    }
};

void AddSC_action_spell_scripts()
{

    RegisterSpellScript(spell_action_potions);
    RegisterSpellScript(spell_action_quick_attack);
    RegisterSpellScript(spell_action_thrust_attack);
    RegisterSpellScript(spell_action_basic_attack);
    RegisterSpellAndAuraScriptPair(spell_action_deflect, spell_action_deflect_aura);
    RegisterSpellScript(spell_action_attack);
    RegisterSpellScript(spell_action_undeath_strike);
    RegisterSpellScript(spell_action_death_coil);
    RegisterSpellScript(spell_dk_death_coil_damage);
    RegisterSpellScript(spell_action_holy_shock);
    RegisterSpellScript(spell_action_crusader_strike);
    RegisterSpellAndAuraScriptPair(spell_action_devouring_rage, spell_action_devouring_rage_auraa);
    RegisterSpellScript(spell_action_wolf_bite);
}
