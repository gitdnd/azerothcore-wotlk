#include "creature_base.h"

enum EversongDialogue : uint32
{ 
    FELENDREN_INTRO = 200192,
    FELENDREN_TALK_1 = 200193,
    FELENDREN_TALK_2 = 200194,
    FELENDREN_TALK_3 = 200195,
    FELENDREN_TALK_4 = 200196,
    FELENDREN_TALK_5 = 200197,
    FELENDREN_TALK_6 = 200198,
    FELENDREN_PUT_DOWN = 200199,
    FELENDREN_EMBRACE_1 = 200200,
    FELENDREN_EMBRACE_2 = 200201,
    FELENDREN_EMBRACE_3 = 200202,
    FELENDREN_EMBRACE_4 = 200202,
    FELENDREN_TALK_7 = 200204,
    FELENDREN_TALK_8 = 200205,
    FELENDREN_PUT_DOWN_RESET = 200206,
    FELENDREN_INTRO_MET = 200207,

    SOLANIANS_ORB = 200208,
    SCOURGE_SCROLL_INTRO = 200210,
    SCOURGE_SCROLL_READ_1 = 200211,
    SCOURGE_SCROLL_READ_2 = 200212,
    SCOURGE_SCROLL_READ_3 = 200213,
    SCOURGE_SCROLL_READ_4 = 200214,
    DOTDOTDOT = 200209,
};
enum EversongDialogueOptions : uint16
{
    FELENDREN_LEAVE_DIALOGUE,
    FELENDREN_INTRO_DIALOGUE,
    FELENDREN_DIALOGUE_ASSAULTED,
    FELENDREN_DIALOGUE_1,
    FELENDREN_DIALOGUE_2,
    FELENDREN_DIALOGUE_3,
    FELENDREN_DIALOGUE_4,
    FELENDREN_DIALOGUE_5,
    FELENDREN_DIALOGUE_6,
    FELENDREN_DIALOGUE_PUT_DOWN,
    FELENDREN_DIALOGUE_PUT_DOWN_ATTACK,
    FELENDREN_DIALOGUE_EMBRACE,
    FELENDREN_DIALOGUE_EMBRACE_1,
    FELENDREN_DIALOGUE_EMBRACE_2,
    FELENDREN_DIALOGUE_EMBRACE_3,
    FELENDREN_DIALOGUE_7,
    FELENDREN_DIALOGUE_8,

    SOLANIANS_ORB_TAKE,
    SOLANIANS_ORB_SHATTER,
    SOLANIANS_ORB_STARE,
    SCOURGE_SCROLL_TAKE,
    SCOURGE_SCROLL_BURN,
    SCOURGE_SCROLL_READ_DIALOGUE_1,
    SCOURGE_SCROLL_READ_DIALOGUE_2,
    SCOURGE_SCROLL_READ_DIALOGUE_3,
    SCOURGE_SCROLL_READ_DIALOGUE_4,
    SOLANIANS_JOURNAL_TAKE,
    SOLANIANS_JOURNAL_SHATTER,
    SOLANIANS_JOURNAL_STARE,
};

class amani_berserker : public ELKCreatureScript
{
public:
    amani_berserker() : ELKCreatureScript("amani_berserker") {}
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new amani_berserkerAI(creature, this);
    }
    struct amani_berserkerAI : public ELKAI
    {
        amani_berserkerAI(Creature* creature, const ELKCreatureScript* script) : ELKAI(creature, script)
        {
            reinforcementCall = 0;

            me->SetFloatValue(PLAYER_CRIT_PERCENTAGE, 100);
            me->ApplySpellPowerBonus(300, true);
        };
        void EnterCombatCustom(Unit* /*who*/) override
        {
            regularCheck = 3500;
            events.ScheduleEvent(REGULAR_CHECK, regularCheck);
        }
        void ResetExtra() override
        {

            lives = 10;
            spellHits = 0;
            damagedAmount = 0;
            ResetCC();
        }
        void UpdateAI(uint32 diff) override
        {
            ELKCUpdateAI();

            if (currentCombo.type == ELKCActionType_NONE)
                if (Aura* aura = me->GetAura(uint32(ELKSpells::COMBO_COUNT)); aura && aura->GetStackAmount() > 4)
                    if (EasyCast(uint32(SpellsC::CRUSHING_WAVE)))
                        return;

            if (me->IsWithinMeleeRange(me->GetVictim()))
                chanceAtk = script->chanceAtk;
            else
                chanceAtk = 0;
            DoEvents();
        }

        uint32 spellHits = 0;
        void SpellHitExtra(Unit* caster, SpellInfo const* spell) override
        {
            if (caster == me)
                return;
            spellHits++;
            if (rand() % spellHits > 10)
            {
                me->CastSpell(me, 150032, true);
                spellHits = 0;
            }
        } // its deflect. also therees a bug, at start of atk combos does 1 extra normal atk.

        uint8 lives = 10;
        uint16 damagedAmount = 0;

        void DamageTakenExtra(Unit* doneby, uint32& damage, DamageEffectType type, SpellSchoolMask school) override
        {
            if (me->IsAlive() == false)
                return;
            if (damage >= me->GetHealth() && lives > rand() % 10)
            {
                damage = 0;
                me->SetHealth(300);
                lives--;
                chanceDef = 0;
            }
            if (me->HasAura(150033))
                return;
            damagedAmount += damage;
            if (damagedAmount > 30)
                damagedAmount -= 30;
            else
                damagedAmount = 0;
            if (damagedAmount > 300)
            {
                damagedAmount = 0;
                doneby->CastSpell(me, 150033, true);
            }

        }
    };
};




void AddSC_elk_eversong_woods_mobs()
{

    new amani_berserker();
    /*
    new mana_wyrm();
    new wretched();
    new tainted_arcane_wraith();
    new felendren();

    new solanians_orb();
    new scourge_scroll();

    new amani_shadowpriest();

    new amani_axe_thrower(); // jumps and throws axes in 3s. hes an imp basically. doesnt even deflect, dies fast
    new spearcrafter_otembe(); // gets +10 deflect chance after being hit every few attacks, lots of dmg, throws spears and bladestorms in place, after using lives system starts summoning serpent wards

    new shadowpine_ripper(); // small fodder guy that just attacks then equips a weapon, either a sword, or axes and becomes like berserker. if he kills you he casts cannibalize on you.
    new shadowpine_witch(); // summons spiders, zombie mummies, revives allies: KEY, drains mana, uses lightning that reduces armor, shrinks you. if you get to low HP he just casts finger of death for 100% his curr mana and laughs.
    new shadowpine_oracle(); // spirit links allies splitting damage, restores their mana to full after a 10s cast, gives shields that blow up after 5s and thunderclap, stuns them but makes them immune while channeling if and only if theres multiple enemies (which there always is). if completed, heals to full.
    new shadowpine_headhunter(); // replacement for axe thrower, throws spears at a location you are constantly forcing you to move in 5s.
    new shadowpine_shadowcaster(); // better shadow priest, the same but more spells.
    new shadowpine_catlord(); // big; leaves flamestrikes for 30s, jumps a lot, summons cat when hit a lot while deflecting. deflects when not doing anything. will get away to cast a 5s cast time regen spell that if interrupted disarms and silences them for 10s.
    new shadowpine_hexxer(); // berserk AI mixing in all of the spells previously used besides catlord. also shrinks you
    new kelgash(); //

    new amanshi_berserker() // amani berserkers have a 3% chance to turn into one of these on death.
    new amanshi_warbringer() // 2 of them outside the instance somewhere
    // replace savages, lookouts and scouts with all the custom guys I've made. make elder lynxes wander outside
    */
}





