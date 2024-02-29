#include <fstream>
#include <filesystem>

#include <iostream>
#include <ELK/nlohmann/json.hpp>

#include "Chat.h"
#include "Language.h"
#include "Player.h"

#include "creature_base.h"

#define DoComboIfAvailable(x) if(EasyCast(x)) return

#define ExtractJson(val, json, index) if (json.find(index) != json.end())	\
{																			\
	val = json[index];														\
}


namespace fs = std::filesystem;
using json = nlohmann::json;


void ReadELKAIArray(std::string name, const json& js, ELKCreatureScript* script)
{
    ELKCCombo combo;
    combo.name = name;
    ExtractJson(combo.probability, js, "PROBABILITY");
    script->CombosTotal += combo.probability;


    if (js.find("PROBABILITY_AURA") != js.end())
        for (const auto& [k, v] : js["PROBABILITY_AURA"].items())
        {
            combo.probabilityAura.push_back({ atoi(k.c_str()), atoi(k.c_str()) });
            script->AuraProbabilityTotal[atoi(k.c_str())] += atoi(k.c_str());
        }

    for (const auto& moves : js["SEQUENCE"]) {
        std::vector<uint8> sequence = {};
        sequence = moves.get<std::vector<uint8>>();
        ELKCSequence cSequence;
        cSequence.actions = sequence;
        for (uint8 i = 0; i < sequence.size(); i++)
            if (sequence[i] < script->Actions.size())
            {
                auto action = script->Actions[sequence[i]];
                if (action.runeCost > 0)
                {
                    cSequence.runeCost += script->Actions[i].runeCost;
                }
                if (action.cooldown > 0 && cSequence.cooldown < action.cooldown)
                    cSequence.cooldown = action.cooldown;
            }
            else
                continue;
        combo.sequences.push_back(cSequence);
    }
    script->Combos.push_back(combo);
}
void AddELKAICreatureJson()
{
    const fs::path directory = "\Creature";
    if (fs::is_directory(directory)) {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (!entry.path().string().ends_with(".json"))
                continue;
            std::ifstream file(entry.path());
            if (file.is_open()) {
                try {

                    json data;
                    file >> data;

                    for (const auto& creature : data)
                    {
                        std::string name = "";
                        ExtractJson(name, creature, "name");

                        if (auto it = ELKCreatureScript::ELKCreatureScripts.find(name); it != ELKCreatureScript::ELKCreatureScripts.end())
                        {
                            ExtractJson(it->second->reinforcementCall, creature, "reinforcementCall");
                            ExtractJson(it->second->chanceAtk, creature, "chanceAtk");
                            ExtractJson(it->second->chanceDef, creature, "chanceDef");
                            ExtractJson(it->second->chanceSpell, creature, "chanceSpell");
                            ExtractJson(it->second->SpellPowerBonus, creature, "SpellPowerBonus");

                            ExtractJson(it->second->regularCheck, creature, "regularCheck");
                            ExtractJson(it->second->regularCheckHP, creature, "regularCheckHP");

                            ExtractJson(it->second->dynamicMovement.dist, creature, "DMdist");
                            ExtractJson(it->second->dynamicMovement.angleRaw, creature, "DMangleRaw");
                            ExtractJson(it->second->dynamicMovement.angleBase, creature, "DMangleBase");
                            ExtractJson(it->second->dynamicMovement.moveTime, creature, "DMmoveTime");
                            ExtractJson(it->second->dynamicMovementOdds, creature, "DModds");

                            ExtractJson(it->second->dynamicMovement.sprintPre, creature, "DMsprintPre");
                            ExtractJson(it->second->dynamicMovement.sprintPost, creature, "DMsprintPost");

                            uint8 next_mutate = 0;
                            it->second->Actions.clear();
                            if (creature.find("ACTIONS") != creature.end())
                                for (const auto& [k, v] : creature["ACTIONS"].items())
                                {

                                    ELKCAction action;
                                    std::string tempId = "";
                                    ExtractJson(tempId, v, "id");
                                    if (tempId == "JUMP")
                                    {
                                        action.type = ELKCAType::JUMP;
                                    }
                                    else
                                    {
                                        action.id = std::stoi(tempId);
                                        action.type = ELKCAType::SPELL;
                                        const SpellInfo* spell = sSpellMgr->GetSpellInfo(action.id);

                                        if (!spell)
                                            continue;
                                        SpellRuneCostEntry const* runeCostData = sSpellRuneCostStore.LookupEntry(spell->RuneCostID);
                                        if (runeCostData && !(runeCostData->NoRuneCost()))
                                        {
                                            for (uint8 i = 0; i < RUNE_DEATH; ++i)
                                            {
                                                action.runeCost += runeCostData->RuneCost[i];
                                            }
                                        }
                                    }
                                    std::string tempTarget = "";
                                    ExtractJson(tempTarget, v, "target");
                                    if (tempTarget == "VICTIM")
                                    {
                                        action.target = ELKCATarget::VICTIM;
                                    }
                                    else if (tempTarget == "VICTIMAWAY")
                                    {
                                        action.target = ELKCATarget::VICTIMAWAY;
                                    }
                                    ExtractJson(action.delay, v, "delay");
                                    ExtractJson(action.max_dist, v, "max_dist");
                                    ExtractJson(action.angle, v, "angle");
                                    ExtractJson(action.speedXY, v, "speedXY");
                                    ExtractJson(action.speedZ, v, "speedZ");
                                    ExtractJson(action.min_dist, v, "min_dist");
                                    ExtractJson(action.distance, v, "distance");
                                    ExtractJson(action.cooldown, v, "cooldown");
                                    ExtractJson(action.min_hp, v, "min_hp");
                                    ExtractJson(action.max_hp, v, "max_hp");
                                    ExtractJson(next_mutate, v, "next_mutate");

                                    if (next_mutate)
                                        action.on_mutate = true;

                                    int16 index = atoi(k.c_str());
                                    if (it->second->Actions.size() <= index)
                                        it->second->Actions.resize(index + 1);
                                    it->second->Actions[index] = action;
                                }

                            it->second->Combos.clear();

                            if (creature.find("COMBOS") != creature.end())
                                for (const auto& [k, v] : creature["COMBOS"].items())
                                {
                                    ReadELKAIArray(k, v, it->second);
                                }
                        }
                    }
                    file.close();
                }
                catch (const std::exception& e) {
                    std::cerr << "Error reading JSON from file " << entry.path() << ": " << e.what() << std::endl;
                }
            }

        }
    }
}
void AddELKDialogueJson()
{
    const fs::path directory = "\Dialogue";
    if (fs::is_directory(directory)) {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (!entry.path().string().ends_with(".json"))
                continue;
            std::ifstream file(entry.path());
            if (file.is_open()) {
                try {

                    json data;
                    file >> data;

                    for (const auto& creature : data)
                    {
                        std::string name = "";
                        ExtractJson(name, creature, "name");

                        if (auto it = ELKCreatureScript::ELKCreatureScripts.find(name); it != ELKCreatureScript::ELKCreatureScripts.end())
                        {
                            ExtractJson(it->second->reinforcementCall, creature, "reinforcementCall");

                            uint8 next_mutate = 0;
                            it->second->Actions.clear();
                            if (creature.find("ACTIONS") != creature.end())
                                for (const auto& itA : creature["ACTIONS"])
                                {

                                    ELKCAction action;
                                    std::string tempId = "";
                                    ExtractJson(tempId, itA, "id");
                                    if (tempId == "JUMP")
                                    {
                                        action.type = ELKCAType::JUMP;
                                    }
                                    else
                                    {
                                        action.id = std::stoi(tempId);
                                        action.type = ELKCAType::SPELL;
                                        const SpellInfo* spell = sSpellMgr->GetSpellInfo(action.id);

                                        if (!spell)
                                            continue;
                                        SpellRuneCostEntry const* runeCostData = sSpellRuneCostStore.LookupEntry(spell->RuneCostID);
                                        if (runeCostData && !(runeCostData->NoRuneCost()))
                                        {
                                            for (uint8 i = 0; i < RUNE_DEATH; ++i)
                                            {
                                                action.runeCost += runeCostData->RuneCost[i];
                                            }
                                        }
                                    }
                                    std::string tempTarget = "";
                                    ExtractJson(tempTarget, itA, "target");
                                    if (tempTarget == "VICTIM")
                                    {
                                        action.target = ELKCATarget::VICTIM;
                                    }
                                    else if (tempTarget == "VICTIMAWAY")
                                    {
                                        action.target = ELKCATarget::VICTIMAWAY;
                                    }
                                    ExtractJson(action.delay, itA, "delay");
                                    ExtractJson(next_mutate, itA, "next_mutate");
                                    if (next_mutate)
                                        action.on_mutate = true;
                                    uint8 index = 0;
                                    ExtractJson(index, itA, "index");
                                    it->second->Actions[index] = action;
                                }
                        }
                    }
                    file.close();
                }
                catch (const std::exception& e) {
                    std::cerr << "Error reading JSON from file " << entry.path() << ": " << e.what() << std::endl;
                }
            }

        }
    }
}
using namespace Acore::ChatCommands;
class elk_commandscript : public CommandScript
{
public:
    elk_commandscript() : CommandScript("elk_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable elkCommandTable =
        {
            { "ai",             HandleELKAICommand,         SEC_ADMINISTRATOR, Console::Yes }
        };
        static ChatCommandTable commandTable =
        {
            { "elkjson", elkCommandTable}
        };

        return commandTable;
    }


    static bool HandleELKAICommand(ChatHandler* handler)
    {
        AddELKAICreatureJson();
        return true;
    }
};

void AddSC_elk_durotar_mobs();
void AddSC_elk_elwynn_forest_mobs();
void AddSC_elk_eversong_woods_mobs();

void AddELKAIScripts()
{
    AddSC_elk_durotar_mobs();
    AddSC_elk_elwynn_forest_mobs();
    AddSC_elk_eversong_woods_mobs();

    new elk_commandscript();
}
