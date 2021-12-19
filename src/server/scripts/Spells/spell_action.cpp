
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




enum ActionSpells
{
    SPELL_ACTION_piercing_ATTACK                = 89998,
    SPELL_ACTION_BASIC_ATTACK                   = 89998,
    SPELL_ACTION_DEFLECT                        = 89999,
    SPELL_ACTION_ATTACK                         = 90000,
    SPELL_ACTION_UNDEATH_STRIKE                 = 90001,
    SPELL_ACTION_CRUSADER_STRIKE                = 91003,
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
        Unit* tempUnit = GetCaster();

        tempUnit->SendPlaySpellVisual(17002);


        switch (GetSpell()->GetTriggerDummy())
        {
            case SPELL_ACTION_CRUSADER_STRIKE:
                tempUnit->CastSpell(tempUnit, 12043, true);
                break;
        }
    }
    void Register() override
    {
        AfterCast += SpellCastFn(spell_action_basic_attack::SpellFinish);
    }
};
class spell_action_deflect_aura : public AuraScript
{
    PrepareAuraScript(spell_action_deflect_aura);

    void SpellFinish()
    {
        Unit* tempCaster = GetCaster();
        tempCaster->HandleEmoteCommand(EMOTE_ONESHOT_PARRY2H);
        tempCaster->SetSheath(SHEATH_STATE_MELEE);
    }
    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* tempCaster = GetTarget();
        Unit* tempVictim = dmgInfo.GetAttacker();

        tempCaster->PlayDirectSound(18023);
        dmgInfo.ModifyDamage(-1 * dmgInfo.GetDamage());
        tempCaster->RemoveSpellCooldown(SPELL_ACTION_DEFLECT, true);
        if (tempVictim)
        {
            tempVictim->ModifyPower(POWER_ENERGY, -10);

            Position tempPoint  = tempCaster->GetPosition();
            Position tempPoint2 = tempVictim->GetPosition();
            Position tempPoint3;
            tempPoint3;
            float tempO = tempPoint2.GetAbsoluteAngle(tempPoint);
            tempPoint3 = tempCaster->GetFirstCollisionPosition(0.6, tempO);
            tempPoint3.SetOrientation(0);
            float tempX = tempPoint3.GetPositionX();
            float tempY = tempPoint3.GetPositionY();
            float tempZ = tempPoint3.GetPositionZ();

            Movement::MoveSplineInit init(tempCaster);
            init.MoveTo(tempX, tempY, tempZ);
            init.SetVelocity(5);
            init.SetOrientationInversed();
            init.Launch();

            tempCaster->HandleEmoteCommand(EMOTE_ONESHOT_PARRY2H);
            tempCaster->SetSheath(SHEATH_STATE_MELEE);
        }
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
        Unit* tempCaster = GetCaster();
        tempCaster->HandleEmoteCommand(EMOTE_ONESHOT_PARRY2H);
        tempCaster->SetSheath(SHEATH_STATE_MELEE);
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

class spell_action_crusader_strike : public SpellScript
{
    PrepareSpellScript(spell_action_crusader_strike);

    void Register() override
    {
        AfterCast += SpellCastFn(spell_action_crusader_strike::ForceAttack);
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


void AddSC_action_spell_scripts()
{
    RegisterSpellScript(spell_action_piercing_attack);
    RegisterSpellScript(spell_action_basic_attack);
    RegisterSpellAndAuraScriptPair(spell_action_deflect, spell_action_deflect_aura);
    RegisterSpellScript(spell_action_attack);
    RegisterSpellScript(spell_action_undeath_strike);
    RegisterSpellScript(spell_action_crusader_strike);
    //RegisterSpellAndAuraScriptPair(spell_dk_death_and_decay, spell_dk_death_and_decay_aura);
}
