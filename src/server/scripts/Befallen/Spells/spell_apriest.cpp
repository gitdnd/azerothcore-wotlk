#include "action_include.h"



class spell_action_heal_flash_heal : public SpellScript
{
    PrepareSpellScript(spell_action_heal_flash_heal);

    void  Recasting()
    {
        Unit* caster = GetCaster();
        auto spell = caster->FindCurrentSpellBySpellId(SPELL_ACTION_HEAL);
        if (spell)
        {
            int castTime = spell->GetCastTime() - spell->GetSpellTimer();
            Unit* target   = spell->m_targets.GetUnitTarget();
            spell->cancel();
            caster->CastSpell(target, SPELL_ACTION_FLASH_HEAL, false);
            auto spell2 = caster->FindCurrentSpellBySpellId(SPELL_ACTION_FLASH_HEAL);
            spell2->ModifySpellTimer(-1 * castTime);
        }
    }
    void Register() override
    {
        BeforeCastTime += SpellCastFn(spell_action_heal_flash_heal::Recasting);
    }
};

void SC_priest_scripts()
{
    RegisterSpellScript(spell_action_heal_flash_heal);
}
