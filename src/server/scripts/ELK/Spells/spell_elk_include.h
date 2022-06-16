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
enum ELKSpells
{
    ATTACK_HIT =            100002,
    ATTACK =                100003,
    ATTACK_SLOW_DEBUFF =    100004,
    ANIMATION_RESET =       100005,
    SPELL_DEFLECT =         100006,
    SPELL_CRUSADER_STRIKE,
    SPELL_UNDEATH_STRIKE
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

 
void SC_elk_include_scripts()
{
    RegisterSpellScript(spell_elk_attack); 
}
