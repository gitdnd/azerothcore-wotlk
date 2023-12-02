#include "spell_elk_include.h"


/*
class spell_elk_spin_fury : public SpellScript
{
    PrepareSpellScript(spell_elk_spin_fury);

    uint8 procs = 0;
    void Periodic()
    {
        procs++;
    }
    void Summon()
    {
        if (procs == 10)
            GetCaster()->CastSpell(GetCaster(), 1600002, true);
    }
    void Register() override
    {
        AfterFullChannel += SpellCastFn(spell_elk_summon_imps::Summon);
        WhileCast += SpellCastFn(spell_elk_summon_imps::Periodic);
    }
};

*/
void AddSC_elk_warrior_scripts()
{
}
