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

// This is where scripts' loading functions should be declared:
void AddELKAIScripts();
void AddSC_elk_spell_scripts();
void AddSC_elk_dk_scripts();
void AddSC_elk_druid_scripts();
void AddSC_elk_hunter_scripts();
void AddSC_elk_mage_scripts();
void AddSC_elk_paladin_scripts();
void AddSC_elk_priest_scripts();
void AddSC_elk_rogue_scripts();
void AddSC_elk_shaman_scripts();
void AddSC_elk_warlock_scripts();
void AddSC_elk_warrior_scripts();

void AddSC_elk_creature_100_scripts();

void AddSC_elk_crafting();



// The name of this function should match:
// void Add${NameOfDirectory}Scripts()
void AddELKScripts()
{ 
    AddELKAIScripts();
    AddSC_elk_spell_scripts();
    AddSC_elk_dk_scripts();
    AddSC_elk_druid_scripts();
    AddSC_elk_hunter_scripts();
    AddSC_elk_mage_scripts();
    AddSC_elk_paladin_scripts();
    AddSC_elk_priest_scripts();
    AddSC_elk_rogue_scripts();
    AddSC_elk_shaman_scripts();
    AddSC_elk_warlock_scripts();
    AddSC_elk_warrior_scripts();

    AddSC_elk_creature_100_scripts();

    AddSC_elk_crafting();


}
