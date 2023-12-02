#include "spell_elk_include.h"




class spell_elk_healing_reach_aura : public AuraScript
{
    PrepareAuraScript(spell_elk_healing_reach_aura);

    Position pos = Position(0, 0, 0, 0);
    void OnStart(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        pos = GetTarget()->GetPosition();
        pos.m_orientation = GetCaster()->GetOrientation();
        Periodic(nullptr);
    }
    void Periodic(AuraEffect const* aurEff)
    {
        pos.RelocatePolarOffset(0.f, 1.f);
    }
    void End(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        GetCaster()->CastSpell(pos.m_positionX, pos.m_positionY, pos.m_positionZ, 1800002, true);
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_elk_healing_reach_aura::OnStart, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elk_healing_reach_aura::Periodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectRemove += AuraEffectRemoveFn(spell_elk_healing_reach_aura::End, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_elk_priest_scripts()
{
    RegisterSpellScript(spell_elk_healing_reach_aura);
}
