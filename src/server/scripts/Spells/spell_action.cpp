
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




enum ActionSpells {
    SPELL_ACTION_DEFLECT                        = 89999,
    SPELL_ACTION_ATTACK                         = 90000,
    SPELL_ACTION_UNDEATH_STRIKE                 = 90001,
    SPELL_ACTION_CRUSADER_STRIKE                = 91003,
};



class spell_action_basic_attack : public SpellScript
{
    PrepareSpellScript(spell_action_basic_attack);

    void AttackFX()
    {
        GetCaster()->SendPlaySpellVisual(17002);
    }
    void Register() override
    {
        AfterCast += SpellCastFn(spell_action_basic_attack::AttackFX);
    }
};
class spell_action_deflect : public AuraScript
{
    PrepareAuraScript(spell_action_deflect);

    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* tempCaster = GetTarget();
        Unit* tempVictim = dmgInfo.GetAttacker();

        tempVictim->ModifyPower(POWER_ENERGY, -10);
        tempCaster->PlayDirectSound();
    }


    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_action_deflect::Absorb, EFFECT_0);
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


void AddSC_action_spell_scripts()
{
    RegisterSpellScript(spell_action_deflect);
    RegisterSpellScript(spell_action_attack);
    RegisterSpellScript(spell_action_crusader_strike);
    //RegisterSpellAndAuraScriptPair(spell_dk_death_and_decay, spell_dk_death_and_decay_aura);
}
