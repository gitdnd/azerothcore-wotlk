/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Creature.h"
#include "Config.h"
#include "Pet.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "Unit.h"

inline bool _ModifyUInt32(bool apply, uint32& baseValue, int32& amount)
{
    // If amount is negative, change sign and value of apply.
    if (amount < 0)
    {
        apply = !apply;
        amount = -amount;
    }
    if (apply)
        baseValue += amount;
    else
    {
        // Make sure we do not get uint32 overflow.
        if (amount > int32(baseValue))
            amount = baseValue;
        baseValue -= amount;
    }
    return apply;
}

/*#######################################
########                         ########
########    UNIT STAT SYSTEM     ########
########                         ########
#######################################*/

void Unit::UpdateAllResistances()
{
    for (uint8 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateResistances(i);
}

void Unit::UpdateDamagePhysical(WeaponAttackType attType)
{
    float totalMin = 0.f;
    float totalMax = 0.f;

    float tmpMin, tmpMax;
    for (uint8 i = 0; i < MAX_ITEM_PROTO_DAMAGES; ++i)
    {
        CalculateMinMaxDamage(attType, false, true, tmpMin, tmpMax, i);
        totalMin += tmpMin;
        totalMax += tmpMax;
    }

    switch (attType)
    {
        case BASE_ATTACK:
        default:
            SetStatFloatValue(UNIT_FIELD_MINDAMAGE, totalMin);
            SetStatFloatValue(UNIT_FIELD_MAXDAMAGE, totalMax);
            break;
        case OFF_ATTACK:
            SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, totalMin);
            SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, totalMax);
            break;
        case RANGED_ATTACK:
            SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, totalMin);
            SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, totalMax);
            break;
    }
}

/*#######################################
########                         ########
########   PLAYERS STAT SYSTEM   ########
########                         ########
#######################################*/

bool Unit::UpdateStats(Stats stat)
{
    if (stat > STAT_SPIRIT)
        return false;

    // value = ((base_value * base_pct) + total_value) * total_pct
    float value  = GetTotalStatValue(stat);

    SetStat(stat, int32(value));

    switch (stat)
    {
        case STAT_STRENGTH:
            UpdateShieldBlockValue();
            break;
        case STAT_AGILITY:
            UpdateArmor();
            UpdateAllCritPercentages();
            UpdateDodgePercentage();
            break;
        case STAT_STAMINA:
            UpdateMaxPower(POWER_ENERGY);
            UpdateMaxHealth();
            break;
        case STAT_INTELLECT:
            UpdateMaxPower(POWER_MANA);
            UpdateAllSpellCritChances();
            UpdateArmor();                                  //SPELL_AURA_MOD_RESISTANCE_OF_INTELLECT_PERCENT, only armor currently
            break;
        default:
            UpdateMaxPower(POWER_RUNIC_POWER);
            break;
    }

    if (stat == STAT_STRENGTH)
    {
        UpdateAttackPowerAndDamage(false);
        if (HasAuraTypeWithMiscvalue(SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT, stat))
            UpdateAttackPowerAndDamage(true);
    }
    else if (stat == STAT_AGILITY)
    {
        UpdateAttackPowerAndDamage(false);
        UpdateAttackPowerAndDamage(true);
    }
    else
    {
        // Need update (exist AP from stat auras)
        if (HasAuraTypeWithMiscvalue(SPELL_AURA_MOD_ATTACK_POWER_OF_STAT_PERCENT, stat))
            UpdateAttackPowerAndDamage(false);
        if (HasAuraTypeWithMiscvalue(SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT, stat))
            UpdateAttackPowerAndDamage(true);
    }

    UpdateSpellDamageAndHealingBonus();
    UpdateManaRegen();

    // Update ratings in exist SPELL_AURA_MOD_RATING_FROM_STAT and only depends from stat
    uint32 mask = 0;
    AuraEffectList const& modRatingFromStat = GetAuraEffectsByType(SPELL_AURA_MOD_RATING_FROM_STAT);
    for (AuraEffectList::const_iterator i = modRatingFromStat.begin(); i != modRatingFromStat.end(); ++i)
        if (Stats((*i)->GetMiscValueB()) == stat)
            mask |= (*i)->GetMiscValue();
    if (mask)
    {
        for (uint32 rating = 0; rating < MAX_COMBAT_RATING; ++rating)
            if (mask & (1 << rating))
                ApplyRatingMod(CombatRating(rating), 0, true);
    }
    return true;
}

void Unit::ApplySpellPowerBonus(int32 amount, bool apply)
{
    apply = _ModifyUInt32(apply, m_baseSpellPower, amount);

    // For speed just update for client
    ApplyModUInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS, amount, apply);
    for (int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
        ApplyModInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, amount, apply);
}

void Unit::UpdateSpellDamageAndHealingBonus()
{
    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    // Get healing bonus for all schools
    SetStatInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS, SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_ALL));
    // Get damage bonus for all schools
    for (int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
        SetStatInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, SpellBaseDamageBonusDone(SpellSchoolMask(1 << i)));
}

bool Unit::UpdateAllStats()
{
    for (int8 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        float value = GetTotalStatValue(Stats(i));
        SetStat(Stats(i), int32(value));
    }

    UpdateArmor();
    // calls UpdateAttackPowerAndDamage() in UpdateArmor for SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR
    UpdateAttackPowerAndDamage(true);
    UpdateMaxHealth();

    for (uint8 i = POWER_MANA; i < MAX_POWERS; ++i)
        UpdateMaxPower(Powers(i));

    UpdateAllRatings();
    UpdateAllCritPercentages();
    UpdateAllSpellCritChances();
    UpdateDefenseBonusesMod();
    UpdateShieldBlockValue();
    UpdateSpellDamageAndHealingBonus();
    UpdateManaRegen();
    UpdateExpertise(BASE_ATTACK);
    UpdateExpertise(OFF_ATTACK);
    RecalculateRating(CR_ARMOR_PENETRATION);
    UpdateAllResistances();

    return true;
}

void Unit::ApplySpellPenetrationBonus(int32 amount, bool apply)
{
    ApplyModInt32Value(PLAYER_FIELD_MOD_TARGET_RESISTANCE, -amount, apply);
    m_spellPenetrationItemMod += apply ? amount : -amount;
}

void Unit::UpdateResistances(uint32 school)
{
    if (school > SPELL_SCHOOL_NORMAL)
    {
        // cant use GetTotalAuraModValue because of total pct multiplier :P
        float value = 0.0f;
        UnitMods unitMod = UnitMods(UNIT_MOD_RESISTANCE_START + school);

        value  = GetModifierValue(unitMod, BASE_VALUE);
        value *= GetModifierValue(unitMod, BASE_PCT);
        value += GetModifierValue(unitMod, TOTAL_VALUE);

        AuraEffectList const& mResbyIntellect = GetAuraEffectsByType(SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT);
        for(AuraEffectList::const_iterator i = mResbyIntellect.begin(); i != mResbyIntellect.end(); ++i)
        {
            if((*i)->GetMiscValue() & (1 << (school - 1)) )
                value += int32(GetStat(Stats((*i)->GetMiscValueB())) * (*i)->GetAmount() / 100.0f);
        }

        value *= GetModifierValue(unitMod, TOTAL_PCT);

        SetResistance(SpellSchools(school), int32(value));
    }
    else
        UpdateArmor();
}

void Unit::UpdateArmor()
{
    UnitMods unitMod = UNIT_MOD_ARMOR;

    float value = GetModifierValue(unitMod, BASE_VALUE);   // base armor (from items)
    value *= GetModifierValue(unitMod, BASE_PCT);           // armor percent from items
    value += GetStat(STAT_AGILITY) * 2.0f;                  // armor bonus from stats
    value += GetModifierValue(unitMod, TOTAL_VALUE);

    //add dynamic flat mods
    AuraEffectList const& mResbyIntellect = GetAuraEffectsByType(SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT);
    for (AuraEffectList::const_iterator i = mResbyIntellect.begin(); i != mResbyIntellect.end(); ++i)
    {
        if ((*i)->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL)
            value += CalculatePct(GetStat(Stats((*i)->GetMiscValueB())), (*i)->GetAmount());
    }

    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetArmor(int32(value));

    UpdateAttackPowerAndDamage();                           // armor dependent auras update for SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR
}

float Unit::GetHealthBonusFromStamina() // not used, stamina doesn't give HP here.
{
    float stamina = GetStat(STAT_STAMINA);

    float baseStam = stamina < 20 ? stamina : 20;
    float moreStam = stamina - baseStam;

    return baseStam + (moreStam * 10.0f);
}

float Unit::GetManaBonusFromIntellect()
{
    return GetStat(STAT_INTELLECT) * 10;
}

void Unit::UpdateMaxHealth()
{
    UnitMods unitMod = UNIT_MOD_HEALTH;

    float value = GetModifierValue(unitMod, BASE_VALUE) + GetCreateHealth();
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE) + GetArmor();
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    sScriptMgr->OnAfterUpdateMaxHealth(this, value);
    SetMaxHealth((uint32)value);
}

void Unit::UpdateMaxPower(Powers power)
{
    UnitMods unitMod = UnitMods(static_cast<uint16>(UNIT_MOD_POWER_START) + power);

    float bonusPower = (power == POWER_MANA && GetCreatePowers(power) > 0) ? GetManaBonusFromIntellect() : 0;

    float value = GetModifierValue(unitMod, BASE_VALUE) + GetCreatePowers(power);
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE) +  bonusPower;
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    sScriptMgr->OnAfterUpdateMaxPower(this, power, value);
    SetMaxPower(power, uint32(value));
}

void Unit::ApplyFeralAPBonus(int32 amount, bool apply)
{
    _ModifyUInt32(apply, m_baseFeralAP, amount);
    UpdateAttackPowerAndDamage();
}

void Unit::UpdateAttackPowerAndDamage(bool ranged)
{
    float val2 = 0.0f;
    float level = float(GetLevel());

    sScriptMgr->OnBeforeUpdateAttackPowerAndDamage(this, level, val2, ranged);

    UnitMods unitMod = ranged ? UNIT_MOD_ATTACK_POWER_RANGED : UNIT_MOD_ATTACK_POWER;

    uint16 index = UNIT_FIELD_ATTACK_POWER;
    uint16 index_mod = UNIT_FIELD_ATTACK_POWER_MODS;
    uint16 index_mult = UNIT_FIELD_ATTACK_POWER_MULTIPLIER;

    if (ranged)
    {
        index = UNIT_FIELD_RANGED_ATTACK_POWER;
        index_mod = UNIT_FIELD_RANGED_ATTACK_POWER_MODS;
        index_mult = UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER;

        val2 = GetStat(STAT_AGILITY) * 2;
    }
    else
    {
        val2 = GetStat(STAT_AGILITY) + GetStat(STAT_STRENGTH) * 2.0f;
    }

    SetModifierValue(unitMod, BASE_VALUE, val2);

    float base_attPower  = GetModifierValue(unitMod, BASE_VALUE) * GetModifierValue(unitMod, BASE_PCT);
    float attPowerMod = GetModifierValue(unitMod, TOTAL_VALUE);

    //add dynamic flat mods
    if (ranged)
    {
        AuraEffectList const& mRAPbyStat = GetAuraEffectsByType(SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT);
        for (AuraEffectList::const_iterator i = mRAPbyStat.begin(); i != mRAPbyStat.end(); ++i)
            attPowerMod += CalculatePct(GetStat(Stats((*i)->GetMiscValue())), (*i)->GetAmount());
    }
    else
    {
        AuraEffectList const& mAPbyStat = GetAuraEffectsByType(SPELL_AURA_MOD_ATTACK_POWER_OF_STAT_PERCENT);
        for (AuraEffectList::const_iterator i = mAPbyStat.begin(); i != mAPbyStat.end(); ++i)
            attPowerMod += CalculatePct(GetStat(Stats((*i)->GetMiscValue())), (*i)->GetAmount());

        AuraEffectList const& mAPbyArmor = GetAuraEffectsByType(SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR);
        for (AuraEffectList::const_iterator iter = mAPbyArmor.begin(); iter != mAPbyArmor.end(); ++iter)
            // always: ((*i)->GetModifier()->m_miscvalue == 1 == SPELL_SCHOOL_MASK_NORMAL)
            attPowerMod += int32(GetArmor() / (*iter)->GetAmount());
    }

    float attPowerMultiplier = GetModifierValue(unitMod, TOTAL_PCT) - 1.0f;

    sScriptMgr->OnAfterUpdateAttackPowerAndDamage(this, level, base_attPower, attPowerMod, attPowerMultiplier, ranged);
    SetInt32Value(index, (uint32)base_attPower);            //UNIT_FIELD_(RANGED)_ATTACK_POWER field
    SetInt32Value(index_mod, (uint32)attPowerMod);          //UNIT_FIELD_(RANGED)_ATTACK_POWER_MODS field
    SetFloatValue(index_mult, attPowerMultiplier);          //UNIT_FIELD_(RANGED)_ATTACK_POWER_MULTIPLIER field

    //automatically update weapon damage after attack power modification
    if (ranged)
    {
        UpdateDamagePhysical(RANGED_ATTACK);
    }
    else
    {
        UpdateDamagePhysical(BASE_ATTACK);
        UpdateSpellDamageAndHealingBonus();
        if (CanDualWield() && haveOffhandWeapon())           //allow update offhand damage only if player knows DualWield Spec and has equipped offhand weapon
            UpdateDamagePhysical(OFF_ATTACK);

    }
}

void Unit::UpdateShieldBlockValue()
{
    SetUInt32Value(PLAYER_SHIELD_BLOCK, GetShieldBlockValue());
}

void Player::CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, bool addTotalPct, float& minDamage, float& maxDamage, uint8 damageIndex)
{
    // Only proto damage, not affected by any mods
    if (damageIndex != 0)
    {
        minDamage = 0.0f;
        maxDamage = 0.0f;

        if (!IsInFeralForm() && CanUseAttackType(attType))
        {
            minDamage = GetWeaponDamageRange(attType, MINDAMAGE, damageIndex);
            maxDamage = GetWeaponDamageRange(attType, MAXDAMAGE, damageIndex);
        }

        return;
    }

    UnitMods unitMod;

    switch (attType)
    {
        case BASE_ATTACK:
        default:
            unitMod = UNIT_MOD_DAMAGE_MAINHAND;
            break;
        case OFF_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_OFFHAND;
            break;
        case RANGED_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_RANGED;
            break;
    }

    float attackSpeedMod = GetAPMultiplier(attType, normalized);

    float baseValue  = GetModifierValue(unitMod, BASE_VALUE) + GetTotalAttackPowerValue(attType) / 14.0f * attackSpeedMod;
    float basePct    = GetModifierValue(unitMod, BASE_PCT);
    float totalValue = GetModifierValue(unitMod, TOTAL_VALUE);
    float totalPct   = addTotalPct ? GetModifierValue(unitMod, TOTAL_PCT) : 1.0f;

    float weaponMinDamage = GetWeaponDamageRange(attType, MINDAMAGE);
    float weaponMaxDamage = GetWeaponDamageRange(attType, MAXDAMAGE);

    if (IsInFeralForm()) // check if player is druid and in cat or bear forms
    {
        uint8 lvl = GetLevel();
        if (lvl > 60)
            lvl = 60;

        weaponMinDamage = lvl * 0.85f * attackSpeedMod;
        weaponMaxDamage = lvl * 1.25f * attackSpeedMod;
    }
    else if (!CanUseAttackType(attType)) // check if player not in form but still can't use (disarm case)
    {
        // cannot use ranged/off attack, set values to 0
        if (attType != BASE_ATTACK)
        {
            minDamage = 0.0f;
            maxDamage = 0.0f;
            return;
        }
        weaponMinDamage = BASE_MINDAMAGE;
        weaponMaxDamage = BASE_MAXDAMAGE;
    }
    else if (attType == RANGED_ATTACK) // add ammo DPS to ranged damage
    {
        if (IsPlayer())
        {
            Player* player = dynamic_cast<Player*>(this);
            weaponMinDamage += player->GetAmmoDPS() * attackSpeedMod;
            weaponMaxDamage += player->GetAmmoDPS() * attackSpeedMod;
        }
    }

    minDamage = ((weaponMinDamage + baseValue) * basePct + totalValue) * totalPct;
    maxDamage = ((weaponMaxDamage + baseValue) * basePct + totalValue) * totalPct;

    // pussywizard: crashfix (casting negative to uint => min > max => assertion in urand)
    if (minDamage < 0.0f || minDamage > 1000000000.0f)
        minDamage = 0.0f;
    if (maxDamage < 0.0f || maxDamage > 1000000000.0f)
        maxDamage = 0.0f;
    if (minDamage > maxDamage)
        minDamage = maxDamage;
}

void Unit::UpdateDefenseBonusesMod()
{
    UpdateBlockPercentage();
    UpdateParryPercentage();
    UpdateDodgePercentage();
}

void Unit::UpdateBlockPercentage()
{
    // No block
    float value = 0.0f;
    // Modify value from defense skill
    value += (int32(GetDefenseSkillValue()) - int32(GetMaxSkillValueForLevel())) * 0.04f;
    // Increase from SPELL_AURA_MOD_BLOCK_PERCENT aura
    value += GetTotalAuraModifier(SPELL_AURA_MOD_BLOCK_PERCENT);
    // Increase from rating
    value += GetRatingBonusValue(CR_BLOCK);

    if (sConfigMgr->GetOption<bool>("Stats.Limits.Enable", false))
    {
        value = value > sConfigMgr->GetOption<float>("Stats.Limits.Block", 95.0f) ? sConfigMgr->GetOption<float>("Stats.Limits.Block", 95.0f) : value;
    }

    value = value < 0.0f ? 0.0f : value;
    SetStatFloatValue(PLAYER_BLOCK_PERCENTAGE, value);
}

void Unit::UpdateCritPercentage(WeaponAttackType attType)
{
    BaseModGroup modGroup;
    uint16 index;
    CombatRating cr;

    switch (attType)
    {
        case OFF_ATTACK:
            modGroup = OFFHAND_CRIT_PERCENTAGE;
            index = PLAYER_OFFHAND_CRIT_PERCENTAGE;
            cr = CR_CRIT_MELEE;
            break;
        case RANGED_ATTACK:
            modGroup = RANGED_CRIT_PERCENTAGE;
            index = PLAYER_RANGED_CRIT_PERCENTAGE;
            cr = CR_CRIT_RANGED;
            break;
        case BASE_ATTACK:
        default:
            modGroup = CRIT_PERCENTAGE;
            index = PLAYER_CRIT_PERCENTAGE;
            cr = CR_CRIT_MELEE;
            break;
    }

    float value = GetTotalPercentageModValue(modGroup) + GetRatingBonusValue(cr);
    // Modify crit from weapon skill and maximized defense skill of same level victim difference
    value += (int32(GetWeaponSkillValue(attType)) - int32(GetMaxSkillValueForLevel())) * 0.04f;

    if (sConfigMgr->GetOption<bool>("Stats.Limits.Enable", false))
    {
        value = value > sConfigMgr->GetOption<float>("Stats.Limits.Crit", 95.0f) ? sConfigMgr->GetOption<float>("Stats.Limits.Crit", 95.0f) : value;
    }

    value = value < 0.0f ? 0.0f : value;
    SetStatFloatValue(index, value);
}

void Unit::UpdateAllCritPercentages()
{
    float value = GetMeleeCritFromAgility();

    SetBaseModValue(CRIT_PERCENTAGE, PCT_MOD, value);
    SetBaseModValue(OFFHAND_CRIT_PERCENTAGE, PCT_MOD, value);
    SetBaseModValue(RANGED_CRIT_PERCENTAGE, PCT_MOD, value);

    UpdateCritPercentage(BASE_ATTACK);
    UpdateCritPercentage(OFF_ATTACK);
    UpdateCritPercentage(RANGED_ATTACK);
}

const float m_diminishing_k[MAX_CLASSES] =
{
    1.0f,  // Warrior
    1.0f,  // Paladin
    1.0f,  // Hunter
    1.0f,  // Rogue
    1.0f,  // Priest
    1.0f,  // DK
    1.0f,  // Shaman
    1.0f,  // Mage
    1.0f,  // Warlock
    1.0f,   // ??
    1.0f   // Druid
};

float Unit::GetMissPercentageFromDefence() const
{
    return 0;
}

void Unit::UpdateParryPercentage()
{

    // No parry
    float value = 0.0f;
    m_realParry = 0.0f;
    uint32 pclass = getClass() - 1;
    float nondiminishing  = 5.0f;
    // Parry from rating
    float diminishing = GetRatingBonusValue(CR_PARRY);
    // Parry from SPELL_AURA_MOD_PARRY_PERCENT aura
    nondiminishing += GetTotalAuraModifier(SPELL_AURA_MOD_PARRY_PERCENT);
    // apply diminishing formula to diminishing parry chance
    m_realParry = nondiminishing + diminishing;
    m_realParry = m_realParry < 0.0f ? 0.0f : m_realParry;

    value = std::max(diminishing + nondiminishing, 0.0f);

    if (sConfigMgr->GetOption<bool>("Stats.Limits.Enable", false))
    {
        value = value > sConfigMgr->GetOption<float>("Stats.Limits.Parry", 95.0f) ? sConfigMgr->GetOption<float>("Stats.Limits.Parry", 95.0f) : value;
    }

    SetStatFloatValue(PLAYER_PARRY_PERCENTAGE, value);
}

void Unit::UpdateDodgePercentage()
{
    float diminishing = 0.0f, nondiminishing = 0.0f;
    GetDodgeFromAgility(diminishing, nondiminishing);
    // Dodge from SPELL_AURA_MOD_DODGE_PERCENT aura
    nondiminishing += GetTotalAuraModifier(SPELL_AURA_MOD_DODGE_PERCENT);
    // Dodge from rating
    diminishing += GetRatingBonusValue(CR_DODGE);
    m_realDodge = nondiminishing + (diminishing );

    m_realDodge = m_realDodge < 0.0f ? 0.0f : m_realDodge;
    float value = std::max(diminishing + nondiminishing, 0.0f);

    if (sConfigMgr->GetOption<bool>("Stats.Limits.Enable", false))
    {
        value = value > sConfigMgr->GetOption<float>("Stats.Limits.Dodge", 95.0f) ? sConfigMgr->GetOption<float>("Stats.Limits.Dodge", 95.0f) : value;
    }

    SetStatFloatValue(PLAYER_DODGE_PERCENTAGE, value);
}

void Unit::UpdateSpellCritChance(uint32 school)
{
    // For normal school set zero crit chance
    if (school == SPELL_SCHOOL_NORMAL)
    {
        SetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1, 0.0f);
        return;
    }
    // For others recalculate it from:
    float crit = 0.0f;
    // Crit from Intellect
    crit += GetSpellCritFromIntellect();
    // Increase crit from SPELL_AURA_MOD_SPELL_CRIT_CHANCE
    crit += GetTotalAuraModifierAreaExclusive(SPELL_AURA_MOD_SPELL_CRIT_CHANCE);
    // Increase crit from SPELL_AURA_MOD_CRIT_PCT
    crit += GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_PCT);
    // Increase crit by school from SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL
    crit += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL, 1 << school);
    // Increase crit from spell crit ratings
    crit += GetRatingBonusValue(CR_CRIT_SPELL);

    // Store crit value
    SetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1 + school, crit);
}

void Unit::UpdateArmorPenetration(int32 amount)
{
    // Store Rating Value
    SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_1 + static_cast<uint16>(CR_ARMOR_PENETRATION), amount);
}

void Unit::UpdateMeleeHitChances()
{
    m_modMeleeHitChance = (float)GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE);
    m_modMeleeHitChance += GetRatingBonusValue(CR_HIT_MELEE);
}

void Unit::UpdateRangedHitChances()
{
    m_modRangedHitChance = (float)GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE);
    m_modRangedHitChance += GetRatingBonusValue(CR_HIT_RANGED);
}

void Unit::UpdateSpellHitChances()
{
    m_modSpellHitChance = (float)GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_HIT_CHANCE);
    m_modSpellHitChance += GetRatingBonusValue(CR_HIT_SPELL);
}

void Unit::UpdateAllSpellCritChances()
{
    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateSpellCritChance(i);
}

void Unit::UpdateExpertise(WeaponAttackType attack)
{
    if (attack == RANGED_ATTACK)
        return;

    int32 expertise = int32(GetRatingBonusValue(CR_EXPERTISE));


    AuraEffectList const& expAuras = GetAuraEffectsByType(SPELL_AURA_MOD_EXPERTISE);
    for (AuraEffectList::const_iterator itr = expAuras.begin(); itr != expAuras.end(); ++itr)
    {
        // item neutral spell
        if ((*itr)->GetSpellInfo()->EquippedItemClass == -1)
            expertise += (*itr)->GetAmount();
        // item dependent spell
        else if (IsPlayer())
        {
            Player* player = dynamic_cast<Player*>(this);
            Item* weapon = player->GetWeaponForAttack(attack, true);
            if (weapon && weapon->IsFitToSpellRequirements((*itr)->GetSpellInfo()))
                expertise += (*itr)->GetAmount();
        }
    }

    if (expertise < 0)
        expertise = 0;

    switch (attack)
    {
        case BASE_ATTACK:
            SetUInt32Value(PLAYER_EXPERTISE, expertise);
            break;
        case OFF_ATTACK:
            SetUInt32Value(PLAYER_OFFHAND_EXPERTISE, expertise);
            break;
        default:
            break;
    }
}

void Unit::ApplyManaRegenBonus(int32 amount, bool apply)
{
    _ModifyUInt32(apply, m_baseManaRegen, amount);
    UpdateManaRegen();
}

void Unit::ApplyHealthRegenBonus(int32 amount, bool apply)
{
    _ModifyUInt32(apply, m_baseHealthRegen, amount);
}

void Unit::UpdateManaRegen()
{
    if( HasAuraTypeWithMiscvalue(SPELL_AURA_PREVENT_REGENERATE_POWER, POWER_MANA + 1) )
    {
        SetStatFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER, 0);
        SetStatFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER, 0);
        return;
    }

    float Intellect = GetStat(STAT_INTELLECT);
    // Mana regen from spirit and intellect
    float power_regen = std::sqrt(Intellect) * OCTRegenMPPerSpirit();
    // Apply PCT bonus from SPELL_AURA_MOD_POWER_REGEN_PERCENT aura on spirit base regen
    power_regen *= GetTotalAuraMultiplierByMiscValue(SPELL_AURA_MOD_POWER_REGEN_PERCENT, POWER_MANA);

    // Mana regen from SPELL_AURA_MOD_POWER_REGEN aura
    float power_regen_mp5 = (GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_POWER_REGEN, POWER_MANA) + m_baseManaRegen) / 5.0f;

    // Get bonus from SPELL_AURA_MOD_MANA_REGEN_FROM_STAT aura
    AuraEffectList const& regenAura = GetAuraEffectsByType(SPELL_AURA_MOD_MANA_REGEN_FROM_STAT);
    for (AuraEffectList::const_iterator i = regenAura.begin(); i != regenAura.end(); ++i)
    {
        power_regen_mp5 += GetStat(Stats((*i)->GetMiscValue())) * (*i)->GetAmount() / 500.0f;
    }

    // Set regen rate in cast state apply only on spirit based regen
    int32 modManaRegenInterrupt = GetTotalAuraModifier(SPELL_AURA_MOD_MANA_REGEN_INTERRUPT);
    if (modManaRegenInterrupt > 100)
        modManaRegenInterrupt = 100;
    SetStatFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER, power_regen_mp5 + CalculatePct(power_regen, modManaRegenInterrupt));

    SetStatFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER, power_regen_mp5 + power_regen);
}

void Player::UpdateRuneRegen(RuneType rune)
{
    if (rune >= NUM_RUNE_TYPES)
        return;

    uint32 cooldown = 0;

    for (uint32 i = 0; i < MAX_RUNES; ++i)
        if (GetBaseRune(i) == rune)
        {
            cooldown = GetRuneBaseCooldown(i, true);
            break;
        }

    if (cooldown <= 0)
        return;

    float regen = float(1 * IN_MILLISECONDS) / float(cooldown);
    SetFloatValue(PLAYER_RUNE_REGEN_1 + uint8(rune), regen);
}

void Unit::_ApplyAllStatBonuses()
{
    SetCanModifyStats(false);

    _ApplyAllAuraStatMods();
    if (IsPlayer())
    {
        Player* player = dynamic_cast<Player*>(this);
        player->_ApplyAllItemMods();
    }
    SetCanModifyStats(true);

    UpdateAllStats();
}

void Unit::_RemoveAllStatBonuses()
{
    SetCanModifyStats(false);

    if (IsPlayer())
    {
        Player* player = dynamic_cast<Player*>(this);
        player->_RemoveAllItemMods();
    }
    _RemoveAllAuraStatMods();

    SetCanModifyStats(true);

    UpdateAllStats();
}

