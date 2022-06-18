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
enum Spells
{
    ATTACK_HIT =            100002,
    ATTACK =                100003,
    ATTACK_SLOW_DEBUFF =    100004,
    ANIMATION_RESET =       100005,
    SPELL_DEFLECT =         100006,
    DEFLECT_FATIGUE =       100007,
    SPELL_DEFLECT_SHORT =   100008,
    SPELL_CRUSADER_STRIKE,
    SPELL_UNDEATH_STRIKE
};
enum SpellsC // Creature Spells.
{
    MIND_FLAY =             150001,
    REJUVINATION =          150002,
    FIREBALL =              150003,
    THROW =                 150004,
    FRENZY =                150005,
    SHADOW_WORD_PAIN =      150006,
    ARCANE_BOLT =           150007,
    FEATHER_BURST =         150008,
    SUNDER_ARMOR =          150009,
    MIND_BLAST =            150010,
    WITHERED_TOUCH =        150011,
    EVASION =               150012,
    HEAD_CRACK =            150013,
    POISON =                150014,
    SLOW =                  150015,
    DECAYED_AGILITY =       150016,
    FAERIE_FIRE =           150017,
    MOONFIRE =              150018,
    ARCANE_RESIDUE =        150019,
    CHAIN_LIGHTNING =       150020,
    FROST_SHOCK =           150021,
    HEALING_WAVE =          150022,
    WITHER_STRIKE =         150023,
    FLASH_HEAL =            150024,
    LIGHTNING_BOLT =        150025,
    WRATH =                 150026,
    FEAR =                  150027,
    ARCANE_MISSILES =       150028,
    ARCANE_TORRENT =        150029,
};



class spell_elk_attack : public SpellScript
{
    PrepareSpellScript(spell_elk_attack);
    Unit* caster;
    void SpellBegin()
    {
        caster = GetCaster();
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
        BeforeCast += SpellCastFn(spell_elk_attack::SpellBegin);
        AfterFullChannel += SpellCastFn(spell_elk_attack::AttackHit);
    }

};
