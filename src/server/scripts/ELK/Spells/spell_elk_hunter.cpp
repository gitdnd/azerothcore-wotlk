#include "spell_elk_include.h"




class spell_elk_pin_down_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_pin_down_aura);

    Position pos = Position(0, 0, 0, 0);
    void OnPin(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        pos = GetTarget()->GetPosition();
        Periodic(nullptr);
    }
    void Periodic(AuraEffect const* aurEff)
    {
        if (GetTarget()->GetExactDist(pos) > 4.f)
            GetAura()->GetEffect(EFFECT_1)->ChangeAmount(-90);
        else
            GetAura()->GetEffect(EFFECT_1)->ChangeAmount(0);

        GetTarget()->CastSpell(pos.m_positionX, pos.m_positionY, pos.m_positionZ, 1700002, true);
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_elk_pin_down_aura::OnPin, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elk_pin_down_aura::Periodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};


void AddSC_elk_hunter_scripts()
{
    RegisterSpellScript(spell_elk_pin_down_aura);
}
