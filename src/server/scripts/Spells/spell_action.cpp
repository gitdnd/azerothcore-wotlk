
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

    SPELL_DK_DEATH_COIL_DAMAGE = 47632,
    SPELL_DK_DEATH_COIL_HEAL   = 47633,
};
enum ActionSpells
{
    SPELL_ACITON_QUICK_FOLLOW_UP      = 89993,
    SPELL_ACITON_QUICK_ATTACK     = 89994,
    SPELL_ACITON_ATTACK_SLOW                    = 89995,
    SPELL_ACTION_PIERCING_ATTACK                = 89998,
    SPELL_ACTION_BASIC_ATTACK                   = 89998,
    SPELL_ACTION_DEFLECT                        = 89999,
    SPELL_ACTION_ATTACK                         = 90000,
    SPELL_ACTION_UNDEATH_STRIKE                 = 90001,
    SPELL_ACTION_RAISE_GHOUL                    = 90002,
    SPELL_ACTION_DEATH_COIL                     = 90004,
    SPELL_ACTION_FESTERING_PLAGUE               = 90005,
    SPELL_ACTION_DEATH_COIL_KILLS = 90006,
    SPELL_ACTION_CRUSADER_STRIKE  = 91003,
    SPELL_ACTION_BLOOD_HOWL       = 100003,
    SPELL_WOLF_FORM               = 100004,
    SPELL_ACTION_WOLF_BITE  = 100005,
};




class spell_action_quick_attack : public SpellScript
{
    PrepareSpellScript(spell_action_quick_attack);

    void SpellStart()
    {

        Unit*            unitCaster = GetCaster();
        Position         CasterPnt  = unitCaster->GetWorldLocation();
        const SpellInfo* spellInfo  = GetSpellInfo();

        unitCaster->CastSpell(unitCaster, 89995, true);
        unitCaster->CastSpell(unitCaster, 89993, false);

        Spell* spell = unitCaster->FindCurrentSpellBySpellId(89993);
        if (spell)
        {
            uint32 spellId = (GetSpellInfo()->Id);
            spell->SetTriggerDummy(std::vector<std::any> {spellId});
        }

        int32 atkTime = unitCaster->GetAttackTime(BASE_ATTACK);
        atkTime *= unitCaster->m_modAttackSpeedPct[BASE_ATTACK];
        int32 CD;
        if (unitCaster->haveOffhandWeapon() == true)
        {
            int32 atkTime2 = unitCaster->GetAttackTime(OFF_ATTACK);
            atkTime2 *= unitCaster->m_modAttackSpeedPct[OFF_ATTACK];
            CD = (atkTime + atkTime2) / 2;
        }
        else
        {
            CD = atkTime;
        }
        uint32 category = spellInfo->GetCategory();

        unitCaster->_AddSpellCooldown(spellInfo->Id, category, 0, CD, true, true);
        if (unitCaster->GetTypeId() == TYPEID_PLAYER)
        {
            WorldPacket data;
            Player*     tempPlayer = dynamic_cast<Player*>(unitCaster);

            tempPlayer->BuildCooldownPacket(data, SPELL_COOLDOWN_FLAG_NONE, spellInfo->Id, CD);
            tempPlayer->SendDirectMessage(&data);
        }

        SpellCategoryStore::const_iterator i_scstore = sSpellsByCategoryStore.find(category);
        if (i_scstore != sSpellsByCategoryStore.end())
        {
            for (SpellCategorySet::const_iterator i_scset = i_scstore->second.begin(); i_scset != i_scstore->second.end(); ++i_scset)
            {
                if (i_scset->second == spellInfo->Id) // skip main spell, already handled above
                {
                    continue;
                }

                // Only within the same spellfamily
                SpellInfo const* categorySpellInfo = sSpellMgr->GetSpellInfo(i_scset->second);
                if (!categorySpellInfo || categorySpellInfo->SpellFamilyName != spellInfo->SpellFamilyName)
                {
                    continue;
                }

                unitCaster->_AddSpellCooldown(i_scset->second, category, 0, CD, true);
            }
        }
    }
    void Register() override
    {
        BeforeCast += SpellCastFn(spell_action_quick_attack::SpellStart);
    }
};

class spell_action_piercing_attack : public SpellScript
{
    PrepareSpellScript(spell_action_piercing_attack);

    void SpellStart()
    {
    }
    void Register() override
    {
        BeforeCast += SpellCastFn(spell_action_piercing_attack::SpellStart);
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
                caster->CastSpell(caster, 12043, true);
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
        if (!(caster && unitHit) && GetHitDamage() <= 0)
            return;
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
        OnHit += SpellHitFn(spell_action_basic_attack::SpellHit);
    }
};
class spell_action_deflect_aura : public AuraScript
{
    PrepareAuraScript(spell_action_deflect_aura);

    void SpellFinish()
    {
        Unit* unitCast = GetCaster();
        unitCast->HandleEmoteCommand(EMOTE_ONESHOT_PARRY2H);
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
            unitAttacker->ModifyPower(POWER_ENERGY, -10);

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
                caster->CastCustomSpell(target, SPELL_DK_DEATH_COIL_HEAL, &damage, nullptr, nullptr, true);
            }
            else
            {
                caster->CastCustomSpell(target, SPELL_DK_DEATH_COIL_DAMAGE, &damage, nullptr, nullptr, true);
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

    RegisterSpellScript(spell_action_quick_attack);
    RegisterSpellScript(spell_action_piercing_attack);
    RegisterSpellScript(spell_action_basic_attack);
    RegisterSpellAndAuraScriptPair(spell_action_deflect, spell_action_deflect_aura);
    RegisterSpellScript(spell_action_attack);
    RegisterSpellScript(spell_action_undeath_strike);
    RegisterSpellScript(spell_action_death_coil);
    RegisterSpellScript(spell_dk_death_coil_damage);
    RegisterSpellScript(spell_action_crusader_strike);
    RegisterSpellScript(spell_action_wolf_bite);
}
