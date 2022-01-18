#include "action_include.h"





class spell_action_blood_howl : public SpellScript
{
    PrepareSpellScript(spell_action_blood_howl);

    void Cast()
    {
        auto caster = GetCaster();
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
            if (caster)
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

void SC_enemy_scripts()
{
    RegisterSpellScript(spell_action_blood_howl);
    RegisterSpellAndAuraScriptPair(spell_action_devouring_rage, spell_action_devouring_rage_auraa);
    RegisterSpellScript(spell_action_wolf_bite);
}
