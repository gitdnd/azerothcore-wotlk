#include "action_include.h"

enum Events {
    STILL_IN_AIR,
};

class spell_action_thunder_clap : public SpellScript
{
    PrepareSpellScript(spell_action_thunder_clap);

    bool wasInAir = false;
    int  airTicks = 250;
    WorldLocation castingPoint;
    void StartCast()
    {
        auto  spell  = GetSpell();
        Unit* caster = GetCaster();
        if (caster->GetUnitMovementFlags() & MOVEMENTFLAG_FALLING)
        {
            caster->CastSpell(caster, SPELL_ACTION_SLAM_DUNK, true);
            wasInAir = true;
        }
        else
            castingPoint = caster->GetWorldLocation();

        
        spell->SetSpellTimer(250);
    }
    void SpellCast()
    {
        Unit* caster = GetCaster();

        auto spell = GetSpell();
        if (wasInAir)
        {
            if (caster->GetUnitMovementFlags() & MOVEMENTFLAG_FALLING)
            {
                airTicks -= spell->GetCastTime() - spell->GetSpellTimer();
                spell->ReSetTimer();

            }
            else
            {
                castingPoint = caster->GetWorldLocation();
                wasInAir     = false;
                spell->SetSpellTimer(std::max(airTicks, 0));
                spell->ModifySpellValue(SPELLVALUE_BASE_POINT0, spell->GetSpellValue()->EffectBasePoints[0] * ( 1 + (250 - airTicks) / 250));
            }
        }
        else
        {
            if (caster->GetWorldLocation() != castingPoint)
            {
                caster->CastStop(SPELL_ACTION_THUNDER_CLAP);
            }
        }
    }

    void Register() override
    {
        BeforeCastTime += SpellCastFn(spell_action_thunder_clap::StartCast);
        WhileCast += SpellCastFn(spell_action_thunder_clap::SpellCast);
    }
};

void SC_warrior_scripts() {
    RegisterSpellScript(spell_action_thunder_clap);
}
