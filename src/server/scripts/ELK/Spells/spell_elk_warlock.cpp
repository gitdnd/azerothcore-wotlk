#include "spell_elk_include.h"


class spell_elk_summon_imps : public SpellScript
{
    PrepareSpellScript(spell_elk_summon_imps);

    uint32 channelTime = 0;
    static const uint32 impId = 416;
    void Periodic()
    {
        if(GetSpell()->GetChannelTime() > 0)
            channelTime = 8000 - GetSpell()->GetSpellTimer();
    }
    void Summon()
    {
        Unit* player = GetCaster();
        std::list<Creature*> m = {};
        player->GetAllMinionsByEntry(m, impId);
        uint8 imps = channelTime / 1000 + 1;
        if (m.size() + imps > 8)
            imps = 8 - m.size();
        for (uint8 i = 0; i < imps; i++)
        {
            TempSummon* summon = player->SummonCreature(impId, player->GetPosition(), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 100000, 0, sSummonPropertiesStore.LookupEntry(impId));
            summon->SetFaction(player->GetFaction());
            player->SetMinion(summon, true);
        }
    }
    void Register() override
    {
        AfterFullChannel += SpellCastFn(spell_elk_summon_imps::Summon);
        WhileCast += SpellCastFn(spell_elk_summon_imps::Periodic);
    }
};
void AddSC_elk_warlock_scripts()
{
    RegisterSpellScript(spell_elk_summon_imps);
}
