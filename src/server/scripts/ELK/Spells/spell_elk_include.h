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
    DEFLECT_FATIGUE =           100007,
    SPELL_DEFLECT_SHORT =       100008,
    CRITICAL_ATTACK =           100010,
    SPELL_DOUBLE_JUMP_BACK =    100011,
    SPELL_FEL_RUSH_BUFF =       100012,
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
    DEFLECT2HL =                100035,
    DEFLECT1H =                 100036,
    DEFLECTUNARMED =            100037,
    DEFLECTSHEILDBLOCK =        100038,
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
enum SpellClass : uint32
{
    SPELL_CRUSADER_STRIKE = 999999,
    SPELL_UNDEATH_STRIKE = 999998,
};



class spell_elk_attack : public SpellScript
{
    PrepareSpellScript(spell_elk_attack);
    Unit* caster;
    Spell* spell;





    void SpellClick()
    {
        spell = GetSpell();
        caster = GetCaster();
        Spell* curAtk = caster->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (curAtk)
        {
            if (curAtk->m_spellInfo->Id == ATTACK)
            {
                spell->skip = true;
                return;
            }
        }
        else
        {
            curAtk = caster->GetCurrentSpell(CURRENT_GENERIC_SPELL);
            if (curAtk)
            {
                if (curAtk->m_spellInfo->Id == ATTACK_HIT)
                {
                    spell->skip = true;
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
                int animSpell = ATTACK;
                auto wep1 = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND); 
                auto wep2 = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
                std::vector<const ItemTemplate*> weapons = {nullptr, nullptr};
                if (wep1)
                    weapons[0] = wep1->GetTemplate();
                if (wep2)
                    weapons[1] = wep2->GetTemplate();
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
                int animSpell = ATTACK;
                auto weapons = creature->GetWeaponEquippedEntry();
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
                if (weps == 0)
                    spell->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(ATTACKUNARMED));
                else
                    spell->SetSpellInfo(SpellMgr::instance()->AssertSpellInfo(animSpell));

            }
        }
    }
    void SpellBegin()
    {
    }
    virtual void AttackUnique()
    {

    }
    void AttackHit()
    {
        caster->CastSpell(caster, ATTACK_HIT, false);
        AttackUnique();
    }
    virtual void Register()
    {
        BeforeSpellLoad += SpellCastFn(spell_elk_attack::SpellClick);
        BeforeCast += SpellCastFn(spell_elk_attack::SpellBegin);
        AfterFullChannel += SpellCastFn(spell_elk_attack::AttackHit);
    }

};
