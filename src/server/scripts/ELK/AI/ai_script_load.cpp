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


void ReadELKAIArray(std::string category, const json& js, ELKCreatureScript* script)
{
    ELKActionType type;
    if (category == "ATTACK")
        type = ELKActionType::ATTACK;
    else if (category == "DEFEND")
        type = ELKActionType::DEFEND;
    else if (category == "SPELL")
        type = ELKActionType::SPELL;

    script->Combos[type].clear();
    std::vector<uint8> combo = {};
    for (const auto& moves : js[category]) {
        combo = moves.get<std::vector<uint8>>();
        ELKCCombo cCombo;
        cCombo.actions = combo;
        for (uint8 i = 0; i < combo.size(); i++)
            if (script->Actions[i].runeCost > 0)
                cCombo.runeCost += script->Actions[i].runeCost;
        script->Combos[type].push_back(cCombo);
    }
}
void AddELKAIJson()
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
                                    ExtractJson(action.max_dist, itA, "max_dist");
                                    ExtractJson(action.angle, itA, "angle");
                                    ExtractJson(action.speedXY, itA, "speedXY");
                                    ExtractJson(action.speedZ, itA, "speedZ");
                                    ExtractJson(action.min_dist, itA, "min_dist");
                                    ExtractJson(action.distance, itA, "distance");
                                    ExtractJson(next_mutate, itA, "next_mutate");
                                    if (next_mutate)
                                        action.on_mutate = true;
                                    uint8 index = 0;
                                    ExtractJson(index, itA, "index");
                                    it->second->Actions[index] = action;
                                }

                            if (creature.find("ATTACK") != creature.end())
                                ReadELKAIArray("ATTACK", creature, it->second);
                            if (creature.find("DEFEND") != creature.end())
                                ReadELKAIArray("DEFEND", creature, it->second);
                            if (creature.find("SPELL") != creature.end())
                                ReadELKAIArray("SPELL", creature, it->second);
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
        AddELKAIJson();
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
