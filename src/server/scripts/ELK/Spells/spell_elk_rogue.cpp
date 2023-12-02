#include "spell_elk_include.h"

#include <string>

#include <ModelInstance.h>


class spell_elk_ninja_feint_proc_aura : public AuraScript
{
    PrepareSpellScript(spell_elk_ninja_feint_proc_aura);

    void Summon()
    {
    }
    void Register() override
    {

    }
};

void AddSC_elk_rogue_scripts()
{
    RegisterSpellScript(spell_elk_ninja_feint_proc_aura);
}
