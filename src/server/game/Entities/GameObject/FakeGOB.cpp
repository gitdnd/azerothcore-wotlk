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

#include "AccountMgr.h"
#include "BattlegroundAV.h"
#include "CellImpl.h"
#include "CreatureAISelector.h"
#include "DisableMgr.h"
#include "DynamicTree.h"
#include "GameObjectAI.h"
#include "GameObjectModel.h"
#include "GameTime.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "GroupMgr.h"
#include "ObjectMgr.h"
#include "OutdoorPvPMgr.h"
#include "PoolMgr.h"
#include "ScriptMgr.h"
#include "SpellMgr.h"
#include "Transport.h"
#include "UpdateFieldFlags.h"
#include "World.h"
#include <G3D/Box.h>
#include <G3D/CoordinateFrame.h>
#include <G3D/Quat.h>
/*
FakeGOB::FakeGOB() : WorldObject(false), MovableMapObject(),
m_model(nullptr)
{
    m_objectType |= TYPEMASK_GAMEOBJECT;
    m_objectTypeId = TYPEID_GAMEOBJECT;

    m_updateFlag = (UPDATEFLAG_LOWGUID | UPDATEFLAG_STATIONARY_POSITION | UPDATEFLAG_POSITION | UPDATEFLAG_ROTATION);

    m_valuesCount = GAMEOBJECT_END;

}

FakeGOB::~FakeGOB()
{
    delete m_model;
}

void FakeGOB::BuildValuesUpdate(uint8 updateType, ByteBuffer* data, Player* target) const
{
    if (!target)
        return;

    bool targetIsGM = target->IsGameMaster() && AccountMgr::IsGMAccount(target->GetSession()->GetSecurity());

    ByteBuffer fieldBuffer;

    UpdateMask updateMask;
    updateMask.SetCount(m_valuesCount);

    uint32* flags = GameObjectUpdateFieldFlags;
    uint32 visibleFlag = UF_FLAG_PUBLIC;
    if (GetOwnerGUID() == target->GetGUID())
        visibleFlag |= UF_FLAG_OWNER;

    *data << uint8(updateMask.GetBlockCount());
    updateMask.AppendToPacket(data);
    data->append(fieldBuffer);
}

void FakeGOB::AddToWorld()
{
    ///- Register the gameobject for guid lookup
    if (!IsInWorld())
    {
        if (m_zoneScript)
            m_zoneScript->OnGameObjectCreate(this);

        GetMap()->GetObjectsStore().Insert<GameObject>(GetGUID(), this);
        if (m_spawnId)
            GetMap()->GetGameObjectBySpawnIdStore().insert(std::make_pair(m_spawnId, this));

        if (m_model)
        {
            m_model->UpdatePosition();
            GetMap()->InsertGameObjectModel(*m_model);
        }

        EnableCollision(GetGoState() == GO_STATE_READY || IsTransport()); // pussywizard: this startOpen is unneeded here, collision depends entirely on GOState

        WorldObject::AddToWorld();

        loot.sourceWorldObjectGUID = GetGUID();

        sScriptMgr->OnGameObjectAddWorld(this);
    }
}

void FakeGOB::RemoveFromWorld()
{
    ///- Remove the gameobject from the accessor
    if (IsInWorld())
    {
        sScriptMgr->OnGameObjectRemoveWorld(this);

        if (m_zoneScript)
            m_zoneScript->OnGameObjectRemove(this);

        RemoveFromOwner();

        if (m_model)
            if (GetMap()->ContainsGameObjectModel(*m_model))
                GetMap()->RemoveGameObjectModel(*m_model);

        if (Transport* transport = GetTransport())
            transport->RemovePassenger(this, true);

        // If linked trap exists, despawn it
        if (GameObject* linkedTrap = GetLinkedTrap())
        {
            linkedTrap->Delete();
        }

        WorldObject::RemoveFromWorld();

        if (m_spawnId)
            Acore::Containers::MultimapErasePair(GetMap()->GetGameObjectBySpawnIdStore(), m_spawnId, this);
        GetMap()->GetObjectsStore().Remove<GameObject>(GetGUID());
    }
}

void FakeGOB::CleanupsBeforeDelete(bool finalCleanup)
{
    if (GetTransport() && !ToTransport())
    {
        GetTransport()->RemovePassenger(this);
        SetTransport(nullptr);
        m_movementInfo.transport.Reset();
        m_movementInfo.RemoveMovementFlag(MOVEMENTFLAG_ONTRANSPORT);
    }

    if (IsInWorld())
        RemoveFromWorld();

    if (m_uint32Values)                                      // field array can be not exist if GameOBject not loaded
        RemoveFromOwner();
}

bool FakeGOB::Create(ObjectGuid::LowType guidlow, uint32 name_id, Map* map, uint32 phaseMask, float x, float y, float z, float ang, G3D::Quat const& rotation, uint32 animprogress, GOState go_state, uint32 artKit)
{
    ASSERT(map);
    SetMap(map);

    Relocate(x, y, z, ang);
    m_stationaryPosition.Relocate(x, y, z, ang);
    if (!IsPositionValid())
    {
        LOG_ERROR("entities.gameobject", "Gameobject (GUID: {} Entry: {}) not created. Suggested coordinates isn't valid (X: {} Y: {})", guidlow, name_id, x, y);
        return false;
    }

    SetPhaseMask(phaseMask, false);

    UpdatePositionData();

    SetZoneScript();
    if (m_zoneScript)
    {
        name_id = m_zoneScript->GetGameObjectEntry(guidlow, name_id);
        if (!name_id)
            return false;
    }

    GameObjectTemplate const* goinfo = sObjectMgr->GetGameObjectTemplate(name_id);
    if (!goinfo)
    {
        LOG_ERROR("sql.sql", "Gameobject (GUID: {} Entry: {}) not created: non-existing entry in `gameobject_template`. Map: {} (X: {} Y: {} Z: {})", guidlow, name_id, map->GetId(), x, y, z);
        return false;
    }

    Object::_Create(guidlow, goinfo->entry, HighGuid::GameObject);

    m_goInfo = goinfo;

    if (goinfo->type >= MAX_GAMEOBJECT_TYPE)
    {
        LOG_ERROR("sql.sql", "Gameobject (GUID: {} Entry: {}) not created: non-existing GO type '{}' in `gameobject_template`. It will crash client if created.", guidlow, name_id, goinfo->type);
        return false;
    }

    GameObjectAddon const* addon = sObjectMgr->GetGameObjectAddon(GetSpawnId());

    // hackfix for the hackfix down below
    switch (goinfo->entry)
    {
        // excluded ids from the hackfix below
        // used switch since there should be more
    case 181233: // maexxna portal effect
    case 181575: // maexxna portal
    case 20992: // theramore black shield
    case 21042: // theramore guard badge
        SetLocalRotation(rotation);
        break;
    default:
        // xinef: hackfix - but make it possible to use original WorldRotation (using special gameobject addon data)
        // pussywizard: temporarily calculate WorldRotation from orientation, do so until values in db are correct
        if (addon && addon->invisibilityType == INVISIBILITY_GENERAL && addon->InvisibilityValue == 0)
        {
            SetLocalRotation(rotation);
        }
        else
        {
            SetLocalRotationAngles(NormalizeOrientation(GetOrientation()), 0.0f, 0.0f);
        }
        break;
    }

    // pussywizard: no PathRotation for normal gameobjects
    SetTransportPathRotation(0.0f, 0.0f, 0.0f, 1.0f);

    SetObjectScale(goinfo->size);

    if (GameObjectTemplateAddon const* templateAddon = GetTemplateAddon())
    {
        SetUInt32Value(GAMEOBJECT_FACTION, templateAddon->faction);
        ReplaceAllGameObjectFlags((GameObjectFlags)templateAddon->flags);
    }

    SetEntry(goinfo->entry);

    // set name for logs usage, doesn't affect anything ingame
    SetName(goinfo->name);

    // GAMEOBJECT_BYTES_1, index at 0, 1, 2 and 3
    SetGoType(GameobjectTypes(goinfo->type));

    if (IsInstanceGameobject())
    {
        switch (GetStateSavedOnInstance())
        {
        case 0:
            SetGoState(GO_STATE_READY);
            SwitchDoorOrButton(true);
            break;
        case 1:
            SetGoState(GO_STATE_READY);
            break;
        case 2:
            SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            break;
        default:
            SetGoState(go_state);
            break;
        }
    }
    else
    {
        SetGoState(go_state);
    }

    SetGoArtKit(artKit);

    SetDisplayId(goinfo->displayId);

    if (!m_model)
        m_model = CreateModel();

    switch (goinfo->type)
    {
    case GAMEOBJECT_TYPE_FISHINGHOLE:
        SetGoAnimProgress(animprogress);
        m_goValue.FishingHole.MaxOpens = urand(GetGOInfo()->fishinghole.minSuccessOpens, GetGOInfo()->fishinghole.maxSuccessOpens);
        break;
    case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
        m_goValue.Building.Health = goinfo->building.intactNumHits + goinfo->building.damagedNumHits;
        m_goValue.Building.MaxHealth = m_goValue.Building.Health;
        SetGoAnimProgress(255);
        break;
    case GAMEOBJECT_TYPE_FISHINGNODE:
        SetGoAnimProgress(0);
        break;
    case GAMEOBJECT_TYPE_TRAP:
        if (GetGOInfo()->trap.stealthed)
        {
            m_stealth.AddFlag(STEALTH_TRAP);
            m_stealth.AddValue(STEALTH_TRAP, 70);
        }

        if (GetGOInfo()->trap.invisible)
        {
            m_invisibility.AddFlag(INVISIBILITY_TRAP);
            m_invisibility.AddValue(INVISIBILITY_TRAP, 300);
        }
        break;
    default:
        SetGoAnimProgress(animprogress);
        break;
    }

    if (addon)
    {
        if (addon->InvisibilityValue)
        {
            m_invisibility.AddFlag(addon->invisibilityType);
            m_invisibility.AddValue(addon->invisibilityType, addon->InvisibilityValue);
        }
    }

    LastUsedScriptID = GetGOInfo()->ScriptId;
    AIM_Initialize();

    if (uint32 linkedEntry = GetGOInfo()->GetLinkedGameObjectEntry())
    {
        GameObject* linkedGO = new GameObject();
        if (linkedGO->Create(map->GenerateLowGuid<HighGuid::GameObject>(), linkedEntry, map, phaseMask, x, y, z, ang, rotation, 255, GO_STATE_READY))
        {
            SetLinkedTrap(linkedGO);
            map->AddToMap(linkedGO);
        }
        else
        {
            delete linkedGO;
        }
    }

    // Check if GameObject is Large
    if (goinfo->IsLargeGameObject())
    {
        SetVisibilityDistanceOverride(VisibilityDistanceType::Large);
    }

    // Check if GameObject is Infinite
    if (goinfo->IsInfiniteGameObject())
    {
        SetVisibilityDistanceOverride(VisibilityDistanceType::Infinite);
    }

    return true;
}

void FakeGOB::Update(uint32 diff)
{
    if (AI())
        AI()->UpdateAI(diff);
    else if (!AIM_Initialize())
        LOG_ERROR("entities.gameobject", "Could not initialize GameObjectAI");

    if (m_despawnDelay)
    {
        if (m_despawnDelay > diff)
        {
            m_despawnDelay -= diff;
        }
        else
        {
            m_despawnDelay = 0;
            DespawnOrUnsummon(0ms, m_despawnRespawnTime);
        }
    }

    for (std::unordered_map<ObjectGuid, int32>::iterator itr = m_SkillupList.begin(); itr != m_SkillupList.end();)
    {
        if (itr->second > 0)
        {
            if (itr->second > static_cast<int32>(diff))
            {
                itr->second -= static_cast<int32>(diff);
                ++itr;
            }
            else
            {
                itr = m_SkillupList.erase(itr);
            }
        }
        else
        {
            ++itr;
        }
    }

    switch (m_lootState)
    {
    case GO_NOT_READY:
    {
        switch (GetGoType())
        {
        case GAMEOBJECT_TYPE_TRAP:
        {
            // Arming Time for GAMEOBJECT_TYPE_TRAP (6)
            GameObjectTemplate const* goInfo = GetGOInfo();
            // Bombs
            if (goInfo->trap.type == 2)
                m_cooldownTime = GameTime::GetGameTimeMS().count() + 10 * IN_MILLISECONDS; // Hardcoded tooltip value
            else if (GetOwner())
                m_cooldownTime = GameTime::GetGameTimeMS().count() + goInfo->trap.startDelay * IN_MILLISECONDS;

            m_lootState = GO_READY;
            break;
        }
        case GAMEOBJECT_TYPE_FISHINGNODE:
        {
            // fishing code (bobber ready)
            if (GameTime::GetGameTime().count() > m_respawnTime - FISHING_BOBBER_READY_TIME)
            {
                // splash bobber (bobber ready now)
                Unit* caster = GetOwner();
                if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                {
                    SetGoState(GO_STATE_ACTIVE);
                    ReplaceAllGameObjectFlags(GO_FLAG_NODESPAWN);

                    UpdateData udata;
                    WorldPacket packet;
                    BuildValuesUpdateBlockForPlayer(&udata, caster->ToPlayer());
                    udata.BuildPacket(&packet);
                    caster->ToPlayer()->GetSession()->SendPacket(&packet);

                    SendCustomAnim(GetGoAnimProgress());
                }

                m_lootState = GO_READY;                 // can be successfully open with some chance
            }
            return;
        }
        case GAMEOBJECT_TYPE_SUMMONING_RITUAL:
        {
            if (GameTime::GetGameTimeMS().count() < m_cooldownTime)
                return;
            GameObjectTemplate const* info = GetGOInfo();
            if (info->summoningRitual.animSpell)
            {
                // xinef: if ritual requires animation, ensure that all users performs channel
                CheckRitualList();
            }
            if (GetUniqueUseCount() < info->summoningRitual.reqParticipants)
            {
                SetLootState(GO_READY);
                return;
            }

            bool triggered = info->summoningRitual.animSpell;
            Unit* owner = GetOwner();
            Unit* spellCaster = owner ? owner : ObjectAccessor::GetPlayer(*this, m_ritualOwnerGUID);
            if (!spellCaster)
            {
                SetLootState(GO_JUST_DEACTIVATED);
                return;
            }

            uint32 spellId = info->summoningRitual.spellId;

            if (spellId == 62330)                       // GO store nonexistent spell, replace by expected
            {
                // spell have reagent and mana cost but it not expected use its
                // it triggered spell in fact casted at currently channeled GO
                spellId = 61993;
                triggered = true;
            }

            // Cast casterTargetSpell at a random GO user
            // on the current DB there is only one gameobject that uses this (Ritual of Doom)
            // and its required target number is 1 (outter for loop will run once)
            if (info->summoningRitual.casterTargetSpell && info->summoningRitual.casterTargetSpell != 1) // No idea why this field is a bool in some cases
                for (uint32 i = 0; i < info->summoningRitual.casterTargetSpellTargets; i++)
                    // m_unique_users can contain only player GUIDs
                    if (Player* target = ObjectAccessor::GetPlayer(*this, Acore::Containers::SelectRandomContainerElement(m_unique_users)))
                        spellCaster->CastSpell(target, info->summoningRitual.casterTargetSpell, true);

            // finish owners spell
            // xinef: properly process event cooldowns
            if (owner)
            {
                if (Spell* spell = owner->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                {
                    spell->SendChannelUpdate(0);
                    spell->finish(false);
                }
            }

            // can be deleted now
            if (!info->summoningRitual.ritualPersistent)
                SetLootState(GO_JUST_DEACTIVATED);
            else
                SetLootState(GO_READY);

            ClearRitualList();
            spellCaster->CastSpell(spellCaster, spellId, triggered);
            return;
        }
        case GAMEOBJECT_TYPE_CHEST:
            if (m_restockTime > GameTime::GetGameTime())
            {
                return;
            }
            // If there is no restock timer, or if the restock timer passed, the chest becomes ready to loot
            m_restockTime = 0s;
            m_lootState = GO_READY;
            AddToObjectUpdateIfNeeded();
            break;
        default:
            m_lootState = GO_READY;                         // for other GOis same switched without delay to GO_READY
            break;
        }
        [[fallthrough]];
    }
    case GO_READY:
    {
        if (m_respawnTime > 0)                          // timer on
        {
            time_t now = GameTime::GetGameTime().count();
            if (m_respawnTime <= now)            // timer expired
            {
                ObjectGuid dbtableHighGuid = ObjectGuid::Create<HighGuid::GameObject>(GetEntry(), m_spawnId);
                time_t linkedRespawntime = GetMap()->GetLinkedRespawnTime(dbtableHighGuid);
                if (linkedRespawntime)             // Can't respawn, the master is dead
                {
                    ObjectGuid targetGuid = sObjectMgr->GetLinkedRespawnGuid(dbtableHighGuid);
                    if (targetGuid == dbtableHighGuid) // if linking self, never respawn (check delayed to next day)
                        SetRespawnTime(DAY);
                    else
                        m_respawnTime = (now > linkedRespawntime ? now : linkedRespawntime) + urand(5, MINUTE); // else copy time from master and add a little
                    SaveRespawnTime(); // also save to DB immediately
                    return;
                }

                m_respawnTime = 0;
                m_SkillupList.clear();
                m_usetimes = 0;

                switch (GetGoType())
                {
                case GAMEOBJECT_TYPE_FISHINGNODE:   //  can't fish now
                {
                    Unit* caster = GetOwner();
                    if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        caster->ToPlayer()->RemoveGameObject(this, false);

                        WorldPacket data(SMSG_FISH_ESCAPED, 0);
                        caster->ToPlayer()->GetSession()->SendPacket(&data);
                    }
                    // can be delete
                    m_lootState = GO_JUST_DEACTIVATED;
                    return;
                }
                case GAMEOBJECT_TYPE_DOOR:
                case GAMEOBJECT_TYPE_BUTTON:
                    //we need to open doors if they are closed (add there another condition if this code breaks some usage, but it need to be here for battlegrounds)
                    if (GetGoState() != GO_STATE_READY)
                        ResetDoorOrButton();
                    break;
                case GAMEOBJECT_TYPE_FISHINGHOLE:
                    // Initialize a new max fish count on respawn
                    m_goValue.FishingHole.MaxOpens = urand(GetGOInfo()->fishinghole.minSuccessOpens, GetGOInfo()->fishinghole.maxSuccessOpens);
                    break;
                default:
                    break;
                }

                if (!m_spawnedByDefault)        // despawn timer
                {
                    // can be despawned or destroyed
                    SetLootState(GO_JUST_DEACTIVATED);
                    return;
                }

                // Xinef: Call AI Reset (required for example in SmartAI to clear one time events)
                if (AI())
                    AI()->Reset();

                // respawn timer
                uint32 poolid = m_spawnId ? sPoolMgr->IsPartOfAPool<GameObject>(m_spawnId) : 0;
                if (poolid)
                    sPoolMgr->UpdatePool<GameObject>(poolid, m_spawnId);
                else
                    GetMap()->AddToMap(this);
            }
        }

        if (isSpawned())
        {
            // traps can have time and can not have
            GameObjectTemplate const* goInfo = GetGOInfo();
            if (goInfo->type == GAMEOBJECT_TYPE_TRAP)
            {
                if (GameTime::GetGameTimeMS().count() < m_cooldownTime)
                    break;

                // Type 2 - Bomb (will go away after casting it's spell)
                if (goInfo->trap.type == 2)
                {
                    SetLootState(GO_ACTIVATED);
                    break;
                }

                /// @todo This is activation radius. Casting radius must be selected from spell data.
                /// @todo Move activated state code to GO_ACTIVATED, in this place just check for activation and set state.
                float radius = float(goInfo->trap.diameter) * 0.5f;
                if (!goInfo->trap.diameter)
                {
                    // Cast in other case (at some triggering/linked go/etc explicit call)
                    if (goInfo->trap.cooldown != 3 || m_respawnTime > 0)
                    {
                        break;
                    }

                    radius = 3.f;
                }

                // Type 0 and 1 - trap (type 0 will not get removed after casting a spell)
                Unit* owner = GetOwner();
                Unit* target = nullptr;                            // pointer to appropriate target if found any

                // Note: this hack with search required until GO casting not implemented
                // search unfriendly creature
                if (owner && goInfo->trap.autoCloseTime != -1) // hunter trap
                {
                    Acore::NearestAttackableNoTotemUnitInObjectRangeCheck checker(this, owner, radius);
                    Acore::UnitSearcher<Acore::NearestAttackableNoTotemUnitInObjectRangeCheck> searcher(this, target, checker);
                    Cell::VisitAllObjects(this, searcher, radius);
                }
                else                                        // environmental trap
                {
                    // environmental damage spells already have around enemies targeting but this not help in case not existed GO casting support
                    // affect only players
                    Player* player = nullptr;
                    Acore::AnyPlayerInObjectRangeCheck checker(this, radius, true, true);
                    Acore::PlayerSearcher<Acore::AnyPlayerInObjectRangeCheck> searcher(this, player, checker);
                    Cell::VisitWorldObjects(this, searcher, radius);
                    target = player;
                }

                if (target)
                {
                    SetLootState(GO_ACTIVATED, target);
                }
            }
            else if (uint32 max_charges = goInfo->GetCharges())
            {
                if (m_usetimes >= max_charges)
                {
                    m_usetimes = 0;
                    SetLootState(GO_JUST_DEACTIVATED);      // can be despawned or destroyed
                }
            }
        }

        break;
    }
    case GO_ACTIVATED:
    {
        switch (GetGoType())
        {
        case GAMEOBJECT_TYPE_DOOR:
        case GAMEOBJECT_TYPE_BUTTON:
            if (GetGOInfo()->GetAutoCloseTime() && GameTime::GetGameTimeMS().count() >= m_cooldownTime)
                ResetDoorOrButton();
            break;
        case GAMEOBJECT_TYPE_GOOBER:
            if (GameTime::GetGameTimeMS().count() >= m_cooldownTime)
            {
                RemoveGameObjectFlag(GO_FLAG_IN_USE);

                SetLootState(GO_JUST_DEACTIVATED);
            }
            break;
        case GAMEOBJECT_TYPE_CHEST:
            if (m_groupLootTimer)
            {
                if (m_groupLootTimer <= diff)
                {
                    Group* group = sGroupMgr->GetGroupByGUID(lootingGroupLowGUID);
                    if (group)
                        group->EndRoll(&loot, GetMap());
                    m_groupLootTimer = 0;
                    lootingGroupLowGUID = 0;
                }
                else
                {
                    m_groupLootTimer -= diff;
                }
            }

            // Non-consumable chest was partially looted and restock time passed, restock all loot now
            if (GetGOInfo()->chest.consumable == 0 && GameTime::GetGameTime() >= m_restockTime)
            {
                m_restockTime = 0s;
                m_lootState = GO_READY;
                AddToObjectUpdateIfNeeded();
            }
            break;
        case GAMEOBJECT_TYPE_TRAP:
        {
            GameObjectTemplate const* goInfo = GetGOInfo();
            if (goInfo->trap.type == 2)
            {
                if (goInfo->trap.spellId)
                    CastSpell(nullptr, goInfo->trap.spellId);  // FIXME: null target won't work for target type 1
                SetLootState(GO_JUST_DEACTIVATED);
            }
            else if (Unit* target = ObjectAccessor::GetUnit(*this, _lootStateUnitGUID))
            {
                if (goInfo->trap.spellId)
                    CastSpell(target, goInfo->trap.spellId);

                m_cooldownTime = GameTime::GetGameTimeMS().count() + (goInfo->trap.cooldown ? goInfo->trap.cooldown : uint32(4)) * IN_MILLISECONDS; // template or 4 seconds

                if (goInfo->trap.type == 1)
                    SetLootState(GO_JUST_DEACTIVATED);
                else if (!goInfo->trap.type)
                    SetLootState(GO_READY);

                // Battleground gameobjects have data2 == 0 && data5 == 3
                if (!goInfo->trap.diameter && goInfo->trap.cooldown == 3)
                    if (Player* player = target->ToPlayer())
                        if (Battleground* bg = player->GetBattleground())
                            bg->HandleTriggerBuff(this);
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case GO_JUST_DEACTIVATED:
    {
        // If nearby linked trap exists, despawn it
        if (GameObject* linkedTrap = GetLinkedTrap())
        {
            linkedTrap->DespawnOrUnsummon();
        }

        //if Gameobject should cast spell, then this, but some GOs (type = 10) should be destroyed
        if (GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        {
            SetGoState(GO_STATE_READY);

            //any return here in case battleground traps
            // Xinef: Do not return here for summoned gos that should be deleted few lines below
            // Xinef: Battleground objects are treated as spawned by default
            if (GameObjectTemplateAddon const* addon = GetTemplateAddon())
                if ((addon->flags & GO_FLAG_NODESPAWN) && isSpawnedByDefault())
                    return;
        }

        loot.clear();

        // Do not delete chests or goobers that are not consumed on loot, while still allowing them to despawn when they expire if summoned
        bool isSummonedAndExpired = (GetOwner() || GetSpellId()) && m_respawnTime == 0;
        if ((GetGoType() == GAMEOBJECT_TYPE_CHEST || GetGoType() == GAMEOBJECT_TYPE_GOOBER) && !GetGOInfo()->IsDespawnAtAction() && !isSummonedAndExpired)
        {
            if (GetGoType() == GAMEOBJECT_TYPE_CHEST && GetGOInfo()->chest.chestRestockTime > 0)
            {
                // Start restock timer when the chest is fully looted
                m_restockTime = GameTime::GetGameTime() + Seconds(GetGOInfo()->chest.chestRestockTime);
                SetLootState(GO_NOT_READY);
                AddToObjectUpdateIfNeeded();
            }
            else
            {
                SetLootState(GO_READY);
            }

            UpdateObjectVisibility();
            return;
        }
        else if (GetOwnerGUID() || GetSpellId())
        {
            SetRespawnTime(0);
            Delete();
            return;
        }

        SetLootState(GO_READY);

        //burning flags in some battlegrounds, if you find better condition, just add it
        if (GetGOInfo()->IsDespawnAtAction() || GetGoAnimProgress() > 0)
        {
            SendObjectDeSpawnAnim(GetGUID());
            //reset flags
            if (GameObjectTemplateAddon const* addon = GetTemplateAddon())
                ReplaceAllGameObjectFlags((GameObjectFlags)addon->flags);
        }

        if (!m_respawnDelayTime)
            return;

        if (!m_spawnedByDefault)
        {
            m_respawnTime = 0;
            DestroyForNearbyPlayers(); // xinef: old UpdateObjectVisibility();
            return;
        }

        m_respawnTime = GameTime::GetGameTime().count() + m_respawnDelayTime;

        // if option not set then object will be saved at grid unload
        if (GetMap()->IsDungeon())
            SaveRespawnTime();

        DestroyForNearbyPlayers(); // xinef: old UpdateObjectVisibility();
        break;
    }
    }

    sScriptMgr->OnGameObjectUpdate(this, diff);
}

void FakeGOB::SetLocalRotationAngles(float z_rot, float y_rot, float x_rot)
{
    SetLocalRotation(G3D::Quat(G3D::Matrix3::fromEulerAnglesZYX(z_rot, y_rot, x_rot)));
}

void FakeGOB::SetLocalRotation(G3D::Quat const& rot)
{
    G3D::Quat rotation;
    // Temporary solution for gameobjects that have no rotation data in DB:
    if (G3D::fuzzyEq(rot.z, 0.f) && G3D::fuzzyEq(rot.w, 0.f))
        rotation = G3D::Quat::fromAxisAngleRotation(G3D::Vector3::unitZ(), GetOrientation());
    else
        rotation = rot;

    rotation.unitize();
    m_localRotation = rotation;
    UpdatePackedRotation();
}

void FakeGOB::SetTransportPathRotation(float qx, float qy, float qz, float qw)
{
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 0, qx);
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 1, qy);
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 2, qz);
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 3, qw);
}

void FakeGOB::SetTransportPathRotation(float qx, float qy, float qz, float qw)
{
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 0, qx);
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 1, qy);
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 2, qz);
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 3, qw);
}

G3D::Quat FakeGOB::GetWorldRotation() const
{
    G3D::Quat localRotation = GetLocalRotation();
    if (Transport* transport = GetTransport())
    {
        G3D::Quat worldRotation = transport->GetWorldRotation();

        G3D::Quat worldRotationQuat(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w);
        G3D::Quat localRotationQuat(localRotation.x, localRotation.y, localRotation.z, localRotation.w);

        G3D::Quat resultRotation = localRotationQuat * worldRotationQuat;

        return G3D::Quat(resultRotation.x, resultRotation.y, resultRotation.z, resultRotation.w);
    }
    return localRotation;
}

void FakeGOB::Refresh()
{
    // not refresh despawned not casted GO (despawned casted GO destroyed in all cases anyway)
    if (m_respawnTime > 0 && m_spawnedByDefault)
        return;

    if (isSpawned())
        GetMap()->AddToMap(this);
}
void FakeGOB::DespawnOrUnsummon(Milliseconds delay, Seconds forceRespawnTime)
{
    if (delay > 0ms)
    {
        if (!m_despawnDelay || m_despawnDelay > delay.count())
        {
            m_despawnDelay = delay.count();
            m_despawnRespawnTime = forceRespawnTime;
        }
    }
    else
    {
        if (m_goData)
        {
            int32 const respawnDelay = (forceRespawnTime > 0s) ? forceRespawnTime.count() : m_goData->spawntimesecs;
            SetRespawnTime(respawnDelay);
        }

        // Respawn is handled by the gameobject itself.
        // If we delete it from world, it simply never respawns...
        // Uncomment this and remove the following lines if dynamic spawn is implemented.
        // Delete();
        {
            SetLootState(GO_JUST_DEACTIVATED);
            SendObjectDeSpawnAnim(GetGUID());
            SetGoState(GO_STATE_READY);

            if (GameObject* trap = GetLinkedTrap())
            {
                trap->DespawnOrUnsummon();
            }

            if (GameObjectTemplateAddon const* addon = GetTemplateAddon())
            {
                ReplaceAllGameObjectFlags((GameObjectFlags)addon->flags);
            }

            uint32 poolid = m_spawnId ? sPoolMgr->IsPartOfAPool<GameObject>(m_spawnId) : 0;
            if (poolid)
            {
                sPoolMgr->UpdatePool<GameObject>(poolid, m_spawnId);
            }
        }
    }
}

void FakeGOB::Delete()
{
    SetLootState(GO_NOT_READY);
    RemoveFromOwner();

    SendObjectDeSpawnAnim(GetGUID());

    SetGoState(GO_STATE_READY);

    if (GameObjectTemplateAddon const* addon = GetTemplateAddon())
        ReplaceAllGameObjectFlags((GameObjectFlags)addon->flags);

    // Xinef: if ritual gameobject is removed, clear anim spells
    if (GetGOInfo()->type == GAMEOBJECT_TYPE_SUMMONING_RITUAL)
        ClearRitualList();

    uint32 poolid = m_spawnId ? sPoolMgr->IsPartOfAPool<GameObject>(m_spawnId) : 0;
    if (poolid)
        sPoolMgr->UpdatePool<GameObject>(poolid, m_spawnId);
    else
        AddObjectToRemoveList();
}

void FakeGOB::SetGoState(GOState state)
{
    SetByteValue(GAMEOBJECT_BYTES_1, 0, state);

    sScriptMgr->OnGameObjectStateChanged(this, state);

    if (m_model)
    {
        if (!IsInWorld())
            return;

        // pussywizard: this startOpen is unneeded here, collision depends entirely on current GOState
        EnableCollision(state == GO_STATE_READY || IsTransport());
        // pussywizard: commented out everything below


    }
    if (IsInstanceGameobject() && IsAbleToSaveOnDb())
    {
        // Save the gameobject state on the Database
        if (!FindStateSavedOnInstance())
        {
            SaveInstanceData(GameobjectStateToInt(&state));
        }
        else
        {
            UpdateInstanceData(GameobjectStateToInt(&state));
        }
    }
}

void FakeGOB::SetGoArtKit(uint8 kit)
{
    SetByteValue(GAMEOBJECT_BYTES_1, 2, kit);
    GameObjectData* data = const_cast<GameObjectData*>(sObjectMgr->GetGameObjectData(m_spawnId));
    if (data)
        data->artKit = kit;
}

void FakeGOB::SetGoArtKit(uint8 artkit, GameObject* go, ObjectGuid::LowType lowguid)
{
    const GameObjectData* data = nullptr;
    if (go)
    {
        go->SetGoArtKit(artkit);
        data = go->GetGameObjectData();
    }
    else if (lowguid)
        data = sObjectMgr->GetGameObjectData(lowguid);

    if (data)
        const_cast<GameObjectData*>(data)->artKit = artkit;
}
void FakeGOB::SendCustomAnim(uint32 anim)
{
    WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM, 8 + 4);
    data << GetGUID();
    data << uint32(anim);
    SendMessageToSet(&data, true);
}

bool FakeGOB::IsInRange(float x, float y, float z, float radius) const
{
    GameObjectDisplayInfoEntry const* info = sGameObjectDisplayInfoStore.LookupEntry(m_goInfo->displayId);
    if (!info)
        return IsWithinDist3d(x, y, z, radius);

    float sinA = std::sin(GetOrientation());
    float cosA = cos(GetOrientation());
    float dx = x - GetPositionX();
    float dy = y - GetPositionY();
    float dz = z - GetPositionZ();
    float dist = std::sqrt(dx * dx + dy * dy);
    //! Check if the distance between the 2 objects is 0, can happen if both objects are on the same position.
    //! The code below this check wont crash if dist is 0 because 0/0 in float operations is valid, and returns infinite
    if (G3D::fuzzyEq(dist, 0.0f))
        return true;

    float scale = GetObjectScale();
    float sinB = dx / dist;
    float cosB = dy / dist;
    dx = dist * (cosA * cosB + sinA * sinB);
    dy = dist * (cosA * sinB - sinA * cosB);
    return dx < (info->maxX * scale) + radius && dx >(info->minX * scale) - radius
        && dy < (info->maxY * scale) + radius && dy >(info->minY * scale) - radius
        && dz < (info->maxZ * scale) + radius && dz >(info->minZ * scale) - radius;
}

void FakeGOB::SendMessageToSetInRange(WorldPacket const* data, float dist, bool self, bool includeMargin, Player const* skipped_rcvr) const
{
    dist += GetObjectSize();
    if (includeMargin)
        dist += VISIBILITY_COMPENSATION * 2.0f; // pussywizard: to ensure everyone receives all important packets
    Acore::MessageDistDeliverer notifier(this, data, dist, false, skipped_rcvr);
    Cell::VisitWorldObjects(this, notifier, dist);
}
void FakeGOB::EventInform(uint32 eventId)
{
    if (!eventId)
        return;

    if (AI())
        AI()->EventInform(eventId);

    if (m_zoneScript)
        m_zoneScript->ProcessEvent(this, eventId);
}

void FakeGOB::SetDisplayId(uint32 displayid)
{
    SetUInt32Value(GAMEOBJECT_DISPLAYID, displayid);
    UpdateModel();
}

void FakeGOB::SetPosition(float x, float y, float z, float o)
{
    // pussywizard: do not call for MotionTransport and other gobjects not in grid

    if (!Acore::IsValidMapCoord(x, y, z, o))
        return;

    GetMap()->GameObjectRelocation(this, x, y, z, o);
}

void FakeGOB::UpdateModelPosition()
{
    if (!m_model)
        return;

    if (GetMap()->ContainsGameObjectModel(*m_model))
    {
        GetMap()->RemoveGameObjectModel(*m_model);
        m_model->UpdatePosition();
        GetMap()->InsertGameObjectModel(*m_model);
    }
}

GameObjectModel* FakeGOB::CreateModel()
{
    return GameObjectModel::Create(std::make_unique<GameObjectModelOwnerImpl>(this), sWorld->GetDataPath());
}

void FakeGOB::UpdateModel()
{
    if (!IsInWorld())
        return;
    if (m_model)
        if (GetMap()->ContainsGameObjectModel(*m_model))
            GetMap()->RemoveGameObjectModel(*m_model);
    delete m_model;
    m_model = CreateModel();
    if (m_model)
        GetMap()->InsertGameObjectModel(*m_model);
}
*/
