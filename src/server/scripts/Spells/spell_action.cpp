
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
class spell_action_deflect : public SpellScript
{
    PrepareSpellScript(spell_action_deflect);

    SpellCastResult PlayAnimation()
    {
        Unit* caster = GetCaster();

        return SPELL_CAST_OK;
    }
    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_action_deflect::PlayAnimation);
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
