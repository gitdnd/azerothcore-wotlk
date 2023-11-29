#include "spell_elk_include.h"


class spell_elk_reflect : public AuraScript
{
    PrepareAuraScript(spell_elk_reflect);

    void FlashFocus(ProcEventInfo& eventInfo)
    {
        GetAura()->Remove();
    }
    void Register() override
    {
        DoPrepareProc += AuraProcFn(spell_elk_reflect::FlashFocus);
    }
};

class spell_elk_pyroblast : public ELKSpellScript
{
    PrepareAuraScript(spell_elk_pyroblast);

    void Play()
    {
    }
};
void AddSC_elk_mage_scripts()
{
    RegisterSpellScript(spell_elk_reflect);
    AddExtension(1100001, nullptr, 30, true, UNIT_MOD_STAT_INTELLECT, 0.5f);
}
