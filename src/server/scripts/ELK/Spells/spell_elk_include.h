#pragma once

#include "Group.h"
#include "PetDefines.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Totem.h"
#include "UnitAI.h"
#include <MoveSplineInit.h>

enum class MapDummy : uint8;

enum OutsideSpells
{

};
 enum Spells : uint32
{
    ATTACK_HIT =                100002,
    ATTACK =                    100003,
    ATTACK_SLOW_DEBUFF =        100004,
    ANIMATION_RESET =           100005,
    SPELL_DEFLECT =             100006,
    COMBO_COUNT =               100007,
    SPELL_DEFLECT_SHORT =       100008,
    CRITICAL_ATTACK_HIT =       100009,
    CRITICAL_ATTACK =           100010,
    SPELL_DOUBLE_JUMP_BACK =    100011,
    SPELL_FEL_RUSH_BUFF =       100012,
    DISENGAGE_MINI_HOP =        100014,
    ATTACK1H =                  100025,
    ATTACK2HL =                 100026,
    ATTACK1HP =                 100027,
    ATTACKOFF =                 100028,
    ATTACKOFFP =                100029,
    ATTACKUNARMED =             100030,
    ATTACKSHIELD =              100031,
    CRITICAL_ATTACK1H =         100032,
    CRITICAL_ATTACKUNARMED =    100033,
    CRITICAL_ATTACKMUTILATE =   100034,
    CRITICAL_ATTACK2H =         100035,
    DEFLECT1H =                 100036,
    DEFLECTUNARMED =            100037,
    DEFLECTSHEILDBLOCK =        100038,
    DEFLECT2HL =                100039,
};
enum SpellsC // Creature Spells.
{
    MIND_FLAY =                 150001,
    REJUVINATION =              150002,
    FIREBALL =                  150003,
    THROW =                     150004,
    FRENZY =                    150005,
    SHADOW_WORD_PAIN =          150006,
    ARCANE_BOLT =               150007,
    FEATHER_BURST =             150008,
    SUNDER_ARMOR =              150009,
    MIND_BLAST =                150010,
    WITHERED_TOUCH =            150011,
    EVASION =                   150012,
    HEAD_CRACK =                150013,
    POISON =                    150014,
    SLOW =                      150015,
    DECAYED_AGILITY =           150016,
    FAERIE_FIRE =               150017,
    MOONFIRE =                  150018,
    ARCANE_RESIDUE =            150019,
    CHAIN_LIGHTNING =           150020,
    FROST_SHOCK =               150021,
    HEALING_WAVE =              150022,
    WITHER_STRIKE =             150023,
    FLASH_HEAL =                150024,
    LIGHTNING_BOLT =            150025,
    WRATH =                     150026,
    FEAR =                      150027,
    ARCANE_MISSILES =           150028,
    ARCANE_TORRENT =            150029,
};
enum SpellsStrike : uint32
{
    SPELL_PLAGUE_SCOURGE_STRIKE =       200001,
    SPELL_BLOOD_DEATH_STRIKE =          200002,
    SPELL_FROST_STRIKE_OBLITERATE =     200003,

    SPELL_CRUSADER_STRIKE =             210001,
};

enum SpellsELKPaladin : uint32
{
    SPELL_RETRIBUTION_AURA = 210002,
    SPELL_RETRIBUTION_AURA_2 = 210003,
    SPELL_HOLY_POWER = 210004,

    SUBSPELL_LONG_RETRIBUTION = 219999,
};
class ELKAuraScript : public AuraScript
{
protected:
};
class ELKSpellScript : public SpellScript
{
protected:
    void AttackBegin()
    {
        GetSpell()->SetBonusRange(GetCaster()->GetCombatReach());
    }
    void AttackHit()
    {
        Unit* victim = GetHitUnit();

        CalcDamageInfo damageInfo;
        GetCaster()->CalculateMeleeDamage(victim, 0, &damageInfo);
        damageInfo.HitInfo |= HITINFO_NO_ANIMATION;
        Unit::DealDamageMods(victim, damageInfo.damage, &damageInfo.absorb);
        GetCaster()->PlayDistanceSound(129);

        GetCaster()->DealMeleeDamage(&damageInfo, true);

        DamageInfo dmgInfo(damageInfo);
        dmgInfo.SetSpellInfo(GetSpellInfo());
        GetCaster()->DoOnAttackHitScripts(victim, dmgInfo);
        GetCaster()->ProcDamageAndSpell(damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damage,
            damageInfo.attackType, nullptr, nullptr, -1, nullptr, &dmgInfo);
    }
    void AfterAttack()
    {
        GetCaster()->DoAfterAttackScripts();
    }
    void QueSpell(Unit* caster)
    {
        Player* player = caster->ToPlayer();
        if (!player)
            return;
        player->SetQuedSpell(GetSpellInfo()->Id); 
    }
};

class spell_elk_strike_aura : public AuraScript
{

    PrepareAuraScript(spell_elk_strike_aura);

    void SpellApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        Unit* target= GetTarget();
        if (!target)
            return;
        target->SetStrikeAura(GetSpellInfo()->Id);
    }
    void SpellRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        Unit* target = GetTarget();
        if (!target)
            return;
        target->SetStrikeAura(0);
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_elk_strike_aura::SpellApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_elk_strike_aura::SpellRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_elk_attack : public ELKSpellScript
{
    PrepareSpellScript(spell_elk_attack);
    Unit* caster;
    Spell* spell;

    uint8 comboLength = 0;
    bool isCrit = false;



    void SpellClick()
    {
        spell = GetSpell();
        caster = GetCaster();
        Spell* curAtk = caster->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == ATTACK)
            {
                if (curAtk->GetSpellTimer() < 50)
                {
                    spell->skip = true;
                    QueSpell(caster);
                    return;
                }
                else
                    if (caster->GetCritTempo() > 100)
                    {
                        isCrit = true;
                        caster->ModCritTempo(-100);
                        curAtk->cancel();
                    }
                    else
                    {
                        spell->skip = true;
                        return;
                    }
            }
        }
        else
        {
            curAtk = caster->GetCurrentSpell(CURRENT_GENERIC_SPELL);
            if (curAtk)
            {
                if (curAtk->m_spellInfo->Id == ATTACK_HIT && curAtk->GetSpellTimer() > 0)
                {
                    spell->skip = true;
                    if (caster->IsPlayer())
                    {
                        Player* player = caster->ToPlayer();
                        player->SetQuedSpell(ATTACK);
                    }
                    return;
                }
            }
        }
        if (caster->IsPlayer())
        {
            Player* player = caster->ToPlayer();
            if (player)
            {
                int weps = 0;
                int anim = RAND(1, 4);
                int animSpell = ATTACKUNARMED;
                auto wep1 = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND); 
                auto wep2 = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
                std::vector<const ItemTemplate*> weapons = {nullptr, nullptr};
                if (wep1)
                    weapons[0] = wep1->GetTemplate();
                if (wep2)
                    weapons[1] = wep2->GetTemplate();
                if (isCrit)
                {
                    animSpell = CRITICAL_ATTACKUNARMED;
                    if (weapons[0])
                    {
                        weps += 1;

                        switch (weapons[0]->GetSkill())
                        {
                        case SKILL_AXES:
                        case SKILL_SWORDS:
                        case SKILL_MACES:
                            animSpell = CRITICAL_ATTACK1H;
                            break;
                        case SKILL_FISHING:
                        case SKILL_2H_AXES:
                        case SKILL_2H_SWORDS:
                        case SKILL_2H_MACES:
                            animSpell = CRITICAL_ATTACK2H;
                            break;
                        case SKILL_POLEARMS:
                        case SKILL_STAVES:
                            animSpell = CRITICAL_ATTACK2H;
                            break;
                        case SKILL_DAGGERS:
                            animSpell = CRITICAL_ATTACKMUTILATE;
                            break;
                        case SKILL_SHIELD:
                            animSpell = CRITICAL_ATTACK1H;
                            break;
                        case SKILL_FIST_WEAPONS:
                        default:
                            animSpell = CRITICAL_ATTACKUNARMED;
                            break;
                        }

                    }
                }
                else
                {
                    if (weapons[0])
                    {
                        weps += 1;

                        switch (weapons[0]->GetSkill())
                        {
                        case SKILL_AXES:
                        case SKILL_SWORDS:
                        case SKILL_MACES:
                            animSpell = ATTACK1H;
                            break;
                        case SKILL_FISHING:
                        case SKILL_2H_AXES:
                        case SKILL_2H_SWORDS:
                        case SKILL_2H_MACES:
                            animSpell = ATTACK;
                            break;
                        case SKILL_POLEARMS:
                        case SKILL_STAVES:
                            animSpell = ATTACK2HL;
                            break;
                        case SKILL_DAGGERS:
                            animSpell = ATTACK1HP;
                            break;
                        case SKILL_SHIELD:
                            animSpell = ATTACKSHIELD;
                            break;
                        case SKILL_FIST_WEAPONS:
                        default:
                            animSpell = ATTACKUNARMED;
                            break;
                        }

                    }

                    if (weapons[1])
                    {
                        weps += 2;

                        switch (weapons[1]->GetSkill())
                        {
                        case SKILL_AXES:
                        case SKILL_SWORDS:
                        case SKILL_MACES:
                            animSpell = ATTACKOFF;
                            break;
                        case SKILL_FISHING:
                        case SKILL_2H_AXES:
                        case SKILL_2H_SWORDS:
                        case SKILL_2H_MACES:
                            animSpell = ATTACKOFF;
                            break;
                        case SKILL_POLEARMS:
                        case SKILL_STAVES:
                            animSpell = ATTACKOFFP;
                            break;
                        case SKILL_DAGGERS:
                            animSpell = ATTACKOFFP;
                            break;
                        case SKILL_SHIELD:
                            animSpell = ATTACKSHIELD;
                            break;
                        case SKILL_FIST_WEAPONS:
                        default:
                            animSpell = ATTACKUNARMED;
                            break;
                        }
                    }
                }
                if (weps == 0)
                    spell->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(ATTACKUNARMED));
                else
                    spell->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(animSpell));

            }
        }
        else
        {
            Creature* creature = caster->ToCreature();
            if (creature)
            {

                int weps = 0;
                int anim = RAND(1, 4);
                int animSpell = ATTACKUNARMED;
                auto weapons = creature->GetWeaponEquippedEntry();

                if (isCrit)
                {
                    animSpell = CRITICAL_ATTACKUNARMED;
                    if (weapons[0])
                    {
                        weps += 1;

                        switch (weapons[0]->GetSkill())
                        {
                        case SKILL_AXES:
                        case SKILL_SWORDS:
                        case SKILL_MACES:
                            animSpell = CRITICAL_ATTACK1H;
                            break;
                        case SKILL_FISHING:
                        case SKILL_2H_AXES:
                        case SKILL_2H_SWORDS:
                        case SKILL_2H_MACES:
                            animSpell = CRITICAL_ATTACK2H;
                            break;
                        case SKILL_POLEARMS:
                        case SKILL_STAVES:
                            animSpell = CRITICAL_ATTACK2H;
                            break;
                        case SKILL_DAGGERS:
                            animSpell = CRITICAL_ATTACKMUTILATE;
                            break;
                        case SKILL_SHIELD:
                            animSpell = CRITICAL_ATTACK1H;
                            break;
                        case SKILL_FIST_WEAPONS:
                        default:
                            animSpell = CRITICAL_ATTACKUNARMED;
                            break;
                        }

                    }
                }
                else
                {
                    if (weapons[0])
                    {
                        weps += 1;

                        switch (weapons[0]->GetSkill())
                        {
                        case SKILL_AXES:
                        case SKILL_SWORDS:
                        case SKILL_MACES:
                            animSpell = ATTACK1H;
                        case SKILL_FISHING:
                        case SKILL_2H_AXES:
                        case SKILL_2H_SWORDS:
                        case SKILL_2H_MACES:
                            animSpell = ATTACK;
                        case SKILL_POLEARMS:
                        case SKILL_STAVES:
                            animSpell = ATTACK2HL;
                        case SKILL_DAGGERS:
                            animSpell = ATTACK1HP;
                        case SKILL_SHIELD:
                            animSpell = ATTACKSHIELD;
                        case SKILL_FIST_WEAPONS:
                        default:
                            animSpell = ATTACKUNARMED;
                        }

                    }

                    if (weapons[1])
                    {
                        weps += 2;

                        switch (weapons[1]->GetSkill())
                        {
                        case SKILL_AXES:
                        case SKILL_SWORDS:
                        case SKILL_MACES:
                            animSpell = ATTACKOFF;
                        case SKILL_FISHING:
                        case SKILL_2H_AXES:
                        case SKILL_2H_SWORDS:
                        case SKILL_2H_MACES:
                            animSpell = ATTACKOFF;
                        case SKILL_POLEARMS:
                        case SKILL_STAVES:
                            animSpell = ATTACKOFFP;
                        case SKILL_DAGGERS:
                            animSpell = ATTACKOFFP;
                        case SKILL_SHIELD:
                            animSpell = ATTACKSHIELD;
                        case SKILL_FIST_WEAPONS:
                        default:
                            animSpell = ATTACKUNARMED;
                        }
                    }
                } 
                spell->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(animSpell));

            }
        }
        uint16 cd = 0;
        if(caster->CanUseAttackType(BASE_ATTACK))
            cd += caster->GetAttackTime(BASE_ATTACK);
        if (caster->CanUseAttackType(OFF_ATTACK))
            cd += caster->GetAttackTime(OFF_ATTACK);
        spell->SetRuneCooldown(cd);
        spell->SetRuneCost(1);

        if (auto aura = caster->GetAura(COMBO_COUNT))
            comboLength = aura->GetStackAmount();
    }
    void SpellBegin()
    {
    }
    virtual void AttackUnique()
    {

    }
    void AttackHit()
    {
        uint32 hitSpell;
        if (isCrit)
            hitSpell = CRITICAL_ATTACK_HIT;
        else
            hitSpell = ATTACK_HIT;
        caster->CastSpell(caster, hitSpell, false);
        AttackUnique();

        auto curAtk = caster->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == hitSpell)
            {
                curAtk->GetTriggerDummy()[MapDummy::ComboLength] = comboLength;
                return;
            }
        }
    }
    virtual void Register()
    {
        BeforeSpellLoad += SpellCastFn(spell_elk_attack::SpellClick);
        BeforeCast += SpellCastFn(spell_elk_attack::SpellBegin);
        AfterFullChannel += SpellCastFn(spell_elk_attack::AttackHit);
    }

};
