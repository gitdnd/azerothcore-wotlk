#include "action_include.h"



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

class spell_action_lich_touch : public SpellScript
{
    PrepareSpellScript(spell_action_lich_touch);
};
class spell_action_death_and_decay : public SpellScript
{
    PrepareSpellScript(spell_action_death_and_decay);
};

class spell_dk_death_coil_damage : public SpellScript
{
    PrepareSpellScript(spell_dk_death_coil_damage);

    void SpellHit()
    {
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


void SC_dk_scripts()
{
    RegisterSpellScript(spell_action_undeath_strike);
    RegisterSpellScript(spell_action_death_coil);
    RegisterSpellScript(spell_dk_death_coil_damage);
}
