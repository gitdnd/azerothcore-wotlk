#include "../ELK/Spells/spell_elk_include.h"

class spell_extension_system : public AuraScript
{
    PrepareAuraScript(spell_extension_system);

    struct ExtensionObj
    {
        ExtensionObj(void(spell_extension_system::* Function)(Spell*), int32 Cost, bool Unique) : Function(Function), Cost(Cost), Unique(Unique) {};
        void(spell_extension_system::* Function)(Spell*);
        int32 Cost;
        bool Unique;
    };
    std::vector<const spell_extension_system::ExtensionObj*> extensions = {};
    
    void SpellCast(Spell* spell)
    {
        const uint32 id = spell->GetSpellInfo()->Id;
        auto it = Extensions.find(id);
        if (it == Extensions.end())
            return;
        const ExtensionObj* extension = &(it->second);
        if (extension->Unique == true)
        {
            for (const ExtensionObj* obj : extensions)
            {
                if (obj->Function == extension->Function)
                    return;
            }
        }
        extensions.emplace_back(extension);
        int32 mana = GetUnitOwner()->GetCreateMana();
        if (!mana)
            return;
        mana = int32(CalculatePct(mana, extension->Cost));
        int32 currMana = GetUnitOwner()->GetPower(POWER_MANA);
        if (currMana < mana)
            return;
        GetUnitOwner()->SetPower(POWER_MANA, currMana - mana);
        (this->*extension->Function)((spell));
    }
    void IsCasting(AuraEffect const* aurEff)
    {
        if (GetUnitOwner()->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            return;
        GetAura()->SetDuration(GetAura()->GetDuration() + 100);
    }


    /// Hardcoded Effects


    void CrusaderStrike(Spell* spell)
    {
        Aura* aura = GetUnitOwner()->GetAura(1000007);
        if (!aura)
            return;
        float value = (1.f + (float)GetUnitOwner()->SpellBasePowerBonusDone(GetSpellInfo()->GetSchoolMask(), 0, true));
        aura->ModSpellPowerBonus(value * 0.04f);
        aura->SetDuration(aura->GetDuration() - 400);
    }
    void DivineStorm(Spell* spell)
    {
        for (auto* aura : GetUnitOwner()->GetSingleCastAuras())
        {
            if (aura->GetSpellInfo()->GetSchoolMask() & SpellSchoolMask::SPELL_SCHOOL_MASK_HOLY)
                aura->SetDuration(aura->GetDuration() + 1000);
        }
    }

    void AvengingWrath(Spell* spell)
    {
        Aura* aura = GetUnitOwner()->GetAura(1000005);
        if (!aura)
            return;
        aura->SetDuration(aura->GetDuration() + 1000);
    }


    void Consecration(Spell* spell)
    {
        Player* player = GetUnitOwner()->ToPlayer();
        if (!player)
            return;
        player->RemoveSpellCooldown(1000001);
        
    }



    void SealOfSilver(Spell* spell)
    {
        Aura* aura = GetUnitOwner()->GetAura(1000007);
        if (!aura)
            return;
        float value = (1.f + (float)GetUnitOwner()->SpellBasePowerBonusDone(GetSpellInfo()->GetSchoolMask(), 0, true));
        aura->ModSpellPowerBonus(value * 0.1f);
    }


    void Register() override
    {
        OnSpellCast += OnSpellCastFn(spell_extension_system::SpellCast);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_extension_system::IsCasting, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
    inline static const std::map<uint32, const ExtensionObj> Extensions = {
        {1000001, ExtensionObj(&spell_extension_system::CrusaderStrike, 4, false)},
        {1000004, ExtensionObj(&spell_extension_system::DivineStorm, 12, true)},
        {1000005, ExtensionObj(&spell_extension_system::AvengingWrath, 10, true)},
        {1000006, ExtensionObj(&spell_extension_system::Consecration, 30, true)},
        {1000007, ExtensionObj(&spell_extension_system::SealOfSilver, -15, true)}
    };
};


class spell_ext_crusader_strike : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_crusader_strike);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_crusader_strike::TriggerExtension);
    }
};
class spell_ext_divine_storm : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_divine_storm);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_divine_storm::TriggerExtension);
    }
};
class spell_ext_avenging_wrath : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_avenging_wrath);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_avenging_wrath::TriggerExtension);
    }
};
class spell_ext_consecration : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_consecration);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_consecration::TriggerExtension);
    }
};

class spell_ext_seal_of_silver : public ELKSpellScript
{
    PrepareSpellScript(spell_ext_seal_of_silver);


    void Register() override
    {
        BeforeCast += SpellCastFn(spell_ext_seal_of_silver::TriggerExtension);
    }
};

void AddSC_combosystem()
{
    RegisterSpellScript(spell_extension_system);
    RegisterSpellScript(spell_ext_crusader_strike);
    RegisterSpellScript(spell_ext_divine_storm);
    RegisterSpellScript(spell_ext_avenging_wrath);
    RegisterSpellScript(spell_ext_consecration);
    RegisterSpellScript(spell_ext_seal_of_silver);
}

