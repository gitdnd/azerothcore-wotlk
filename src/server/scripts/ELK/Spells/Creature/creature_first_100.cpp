#include "../spell_elk_include.h"


class spell_elk_creature_giga_thunder_clap : public SpellScript
{
    PrepareSpellScript(spell_elk_creature_giga_thunder_clap);

    void SpellCast()
    {
        GetCaster()->CastSpell(GetCaster(), SpellsC::GIGA_THUNDER_CLAP_HIT, false);
    }
    void Register() override
    {
        AfterFullChannel += SpellCastFn(spell_elk_creature_giga_thunder_clap::SpellCast);
    }
};

void AddSC_elk_creature_100_scripts()
{
    RegisterSpellScript(spell_elk_creature_giga_thunder_clap);
}
