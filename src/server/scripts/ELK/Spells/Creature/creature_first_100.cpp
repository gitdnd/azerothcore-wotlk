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

class spell_elk_dark_wave_aura : public AuraScript
{
    friend class spell_elk_dark_wave;

    PrepareAuraScript(spell_elk_dark_wave_aura);

    struct WaveTargets
    {
        WaveTargets(Position pos, uint8 ticks) : pos(pos), ticks(ticks) {};
        Position pos;
        uint8 ticks;
    };
    std::vector< WaveTargets> targets;
    void OnAdd(Unit* unit)
    {
        targets.push_back(WaveTargets(GetOwner()->GetPosition(), 0));
    }
    void Periodic(AuraEffect const* aurEff)
    {
        for (std::vector<WaveTargets>::iterator target = targets.begin(); target != targets.end(); )
        {
            target->ticks++;
            Position pos2 = target->pos;
            GetUnitOwner()->MovePositionToFirstCollision(pos2, 1, -1 * GetOwner()->GetOrientation() + pos2.m_orientation);
            pos2.m_orientation = target->pos.m_orientation;
            float dist = target->pos.GetExactDist(pos2);
            if (dist < 0.8 || dist > 1.2)
            {
                target = targets.erase(target);
            }
            else
            {
                target->pos = pos2;
                GetUnitOwner()->CastSpell(pos2.m_positionX, pos2.m_positionY, pos2.m_positionZ, 150047, true);
                ++target;
            }
        }
    }
    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elk_dark_wave_aura::Periodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

class spell_elk_dark_wave : public SpellScript
{
    PrepareSpellScript(spell_elk_dark_wave);

    void Hit()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;
        Aura* aura = caster->GetAura(150057);
        if (!aura)
            return;
        spell_elk_dark_wave_aura* script = dynamic_cast<spell_elk_dark_wave_aura*>(aura->GetScriptByName("spell_elk_dark_wave"));
        if (script)
            script->OnAdd(GetHitUnit());
    }
    void Register() override
    {
        AfterHit += SpellHitFn(spell_elk_dark_wave::Hit);
    }
};


class spell_elk_black_wave_aura : public AuraScript
{
    friend class spell_elk_black_wave;
    PrepareAuraScript(spell_elk_black_wave_aura);

    struct WaveTargets
    {
        WaveTargets(Position pos, uint8 ticks, Unit* target) : pos(pos), ticks(ticks), target(target) {};
        Position pos;
        uint8 ticks;
        Unit* target;
    };
    std::vector< WaveTargets> targets;
    void OnAdd(Unit* unit)
    {
        targets.push_back(WaveTargets(GetOwner()->GetPosition(), 0, unit));
    }
    void Periodic(AuraEffect const* aurEff)
    {
        for (std::vector<WaveTargets>::iterator target = targets.begin(); target != targets.end(); )
        {
            target->ticks++;
            Position pos2 = target->pos;
            GetUnitOwner()->MovePositionToFirstCollision(pos2, 1, 0);
            float dist = target->pos.GetExactDist(pos2);
            if (dist < 0.8 || dist > 1.2)
            {
                target = targets.erase(target);
            }
            else
            {
                target->pos = pos2;
                GetUnitOwner()->CastSpell(pos2.m_positionX, pos2.m_positionY, pos2.m_positionZ, 150049, true);
                ++target;
            }
        }
    }
    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elk_black_wave_aura::Periodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

class spell_elk_black_wave : public SpellScript
{
    PrepareSpellScript(spell_elk_black_wave);

    void Hit()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;
        Aura* aura = caster->GetAura(150058);
        if (!aura)
            return;
        spell_elk_black_wave_aura* script = dynamic_cast<spell_elk_black_wave_aura*>(aura->GetScriptByName("spell_elk_black_wave"));
        if (script)
            script->OnAdd(GetHitUnit());
    }
    void Register() override
    {
        AfterHit += SpellHitFn(spell_elk_black_wave::Hit);
    }
};

class spell_elk_black_space_aura : public AuraScript
{
    friend class spell_elk_black_space;
    PrepareAuraScript(spell_elk_black_space_aura);

    struct WaveTargets
    {
        WaveTargets(Position pos, uint8 ticks, Unit* target) : pos(pos), ticks(ticks), target(target) {};
        Position pos;
        uint8 ticks;
        Unit* target;
    };
    std::vector< WaveTargets> targets;
    void OnAdd(Unit* unit)
    {
        targets.push_back(WaveTargets(GetOwner()->GetPosition(), 0, unit));
    }
    void Periodic(AuraEffect const* aurEff)
    {
        for (std::vector<WaveTargets>::iterator target = targets.begin(); target != targets.end(); )
        {
            target->ticks++;
            Position pos2 = target->pos;
            GetUnitOwner()->MovePositionToFirstCollision(pos2, 1, 0);
            float dist = target->pos.GetExactDist(pos2);
            if (dist < 0.8 || dist > 1.2)
            {
                target = targets.erase(target);
            }
            else
            {
                target->pos = pos2;
                GetUnitOwner()->CastSpell(pos2.m_positionX, pos2.m_positionY, pos2.m_positionZ, 150049, true);
                ++target;
            }
        }
        for (std::vector<WaveTargets>::iterator target = targets.begin(); target != targets.end(); )
        {
            target->ticks++;
            Position pos2 = target->pos;

            GetUnitOwner()->MovePositionToFirstCollision(pos2, 1, 0);
            float dist = target->pos.GetExactDist(pos2);
            if (dist < 0.8 || dist > 1.2)
            {
                target = targets.erase(target);
            }
            else
            {
                target->pos = pos2;
                GetUnitOwner()->CastSpell(pos2.m_positionX, pos2.m_positionY, pos2.m_positionZ, 150049, true);
                ++target;
            }
        }
    }
    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elk_black_space_aura::Periodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

class spell_elk_black_space : public SpellScript
{
    PrepareSpellScript(spell_elk_black_space);

    void Hit()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;
        Aura* aura = caster->GetAura(150059);
        if (!aura)
            return;
        spell_elk_black_space_aura* script = dynamic_cast<spell_elk_black_space_aura*>(aura->GetScriptByName("spell_elk_black_space"));
        if (script)
            script->OnAdd(GetHitUnit());
    }
    void Register() override
    {
        AfterHit += SpellHitFn(spell_elk_black_space::Hit);
    }
};


class spell_elk_way_to_hellfire_aura : public AuraScript
{
    friend class spell_elk_way_to_hellfire;
    PrepareAuraScript(spell_elk_way_to_hellfire_aura);

    struct WaveTargets
    {
        WaveTargets() {};
        Position pos;
        uint8 ticks = 0;
        float normX;
        float normY;
        float normZ;
        float normO;
    };
    std::vector< WaveTargets> targets;

    void OnAdd(Unit* target)
    {
        WaveTargets wt;
        wt.pos = GetOwner()->GetPosition();
        wt.pos.m_positionZ += GetUnitOwner()->GetCollisionHeight();
        Position pos2 = target->GetPosition();
        float dist = wt.pos.GetExactDist(pos2);
        wt.normX = (wt.pos.m_positionX - pos2.m_positionX) / dist;
        wt.normY = (wt.pos.m_positionY - pos2.m_positionY) / dist;
        wt.normZ = (wt.pos.m_positionZ - (pos2.m_positionZ + target->GetCollisionHeight()/2)) / dist;
    }
    void Periodic(AuraEffect const* aurEff)
    {
        for (std::vector<WaveTargets>::iterator target = targets.begin(); target != targets.end(); )
        {
            target->ticks++;
            if (target->ticks < 5)
                return;
            Position pos2 = target->pos;
            pos2.m_positionX -= target->normX * target->ticks;
            pos2.m_positionY -= target->normY * target->ticks;
            pos2.m_positionZ -= target->normZ * target->ticks;

            if (!GetUnitOwner()->GetMap()->isInLineOfSight(target->pos.m_positionX, target->pos.m_positionY, target->pos.m_positionZ, pos2.m_positionX, pos2.m_positionY, pos2.m_positionZ, GetUnitOwner()->GetPhaseMask(), LINEOFSIGHT_ALL_CHECKS, VMAP::ModelIgnoreFlags::Nothing))
            {
                if (target->ticks < 10)
                    target = targets.erase(target);
                else
                {
                    if (GetUnitOwner()->GetVictim())
                    {
                        target->ticks = 0;
                        pos2.m_positionX += target->normX * target->ticks;
                        pos2.m_positionY += target->normY * target->ticks;
                        pos2.m_positionZ += target->normZ * target->ticks;
                        target->pos = pos2;
                        pos2 = GetUnitOwner()->GetVictim()->GetPosition();
                        float dist = target->pos.GetExactDist(pos2);
                        target->normX = (target->pos.m_positionX - pos2.m_positionX) / dist;
                        target->normY = (target->pos.m_positionY - pos2.m_positionY) / dist;
                        target->normZ = (target->pos.m_positionZ - (pos2.m_positionZ + GetUnitOwner()->GetVictim()->GetCollisionHeight() / 2)) / dist;
                        ++target;
                    }
                    else
                        target = targets.erase(target);
                }
            }
            GetUnitOwner()->CastSpell(pos2.m_positionX, pos2.m_positionY, pos2.m_positionZ, 150047, true);
        }
    }
    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_elk_way_to_hellfire_aura::Periodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

class spell_elk_way_to_hellfire : public SpellScript
{
    PrepareSpellScript(spell_elk_way_to_hellfire);

    void Hit()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;
        Aura* aura = caster->GetAura(150060);
        if (!aura)
            return;
        spell_elk_way_to_hellfire_aura* script = dynamic_cast<spell_elk_way_to_hellfire_aura*>(aura->GetScriptByName("spell_elk_way_to_hellfire"));
        if (script)
            script->OnAdd(GetHitUnit());
    }
    void Register() override
    {
        AfterHit += SpellHitFn(spell_elk_way_to_hellfire::Hit);
    }
};

void AddSC_elk_creature_100_scripts()
{
    RegisterSpellScript(spell_elk_creature_giga_thunder_clap);
    RegisterSpellAndAuraScriptPair(spell_elk_dark_wave, spell_elk_dark_wave_aura);
    RegisterSpellAndAuraScriptPair(spell_elk_black_wave, spell_elk_black_wave_aura);
    RegisterSpellAndAuraScriptPair(spell_elk_black_space, spell_elk_black_space_aura);
    RegisterSpellAndAuraScriptPair(spell_elk_way_to_hellfire, spell_elk_way_to_hellfire_aura);
}
