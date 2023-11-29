#include "spell_elk_include.h"


class spell_elk_static_link : public AuraScript
{
    PrepareAuraScript(spell_elk_static_link);

    void PeriodicDistanceCheck(AuraEffect const* aurEff)
    {
        std::list<Unit*> target = {};
        aurEff->GetTargetList(target);
        for (Unit* unit : target)
        {
            if (unit->IsAlive() && unit->GetExactDist(GetCaster()) <= GetSpellInfo()->GetMaxRange())
                return;
        }
        Aura* aura = GetAura();
        if (aura)
             aura->Remove();
        aura = GetCaster()->GetAura(1200001);
        if (aura)
            aura->Remove();
    }
    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elk_static_link::PeriodicDistanceCheck, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};
void AddSC_elk_shaman_scripts()
{
    RegisterSpellScript(spell_elk_static_link);
}
