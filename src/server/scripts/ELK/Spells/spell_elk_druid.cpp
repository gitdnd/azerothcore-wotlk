#include "spell_elk_include.h"

#include <string>

#include <ModelInstance.h>


class spell_elk_force_of_nature : public SpellScript
{
    PrepareSpellScript(spell_elk_force_of_nature);

    void Summon()
    {
        Unit* unit = GetCaster();
        float x = -unit->GetPositionX();
        float y = -unit->GetPositionY();
        float z = -unit->GetPositionZ();
        VMAP::ModelInstance* tree = unit->GetMap()->FindCollisionModel(
            GetSpell()->m_targets.GetDstPos()->GetPositionX(),
            GetSpell()->m_targets.GetDstPos()->GetPositionY(),
            GetSpell()->m_targets.GetDstPos()->GetPositionZ(),
            x, y, z);
        if (tree)
        {
            std::string name = tree->name.c_str();
            transform(name.begin(), name.end(), name.begin(), ::tolower);
            if (name.find("tree") == std::string::npos) {
                return;
            }
            std::list<Creature*> m = {};
            unit->GetAllMinionsByEntry(m, 36070);
            uint8 trees = uint8((tree->iBound.high().z - tree->iBound.low().z) / 10.f) + 1;
            if (m.size() + trees > 8)
                trees = 8 - m.size();
            for (uint8 i = 0; i < trees; i++)
            {
                TempSummon* summon = unit->SummonCreature(36070, unit->GetPosition(), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 100000, 0, sSummonPropertiesStore.LookupEntry(36070));
                summon->SetFaction(unit->GetFaction());
                unit->SetMinion(summon, true);
            }
        }

    }
    void Register() override
    {
        BeforeCastTime += SpellCastFn(spell_elk_force_of_nature::Summon);
    }
};
void AddSC_elk_druid_scripts(){
    RegisterSpellScript(spell_elk_force_of_nature);
}
