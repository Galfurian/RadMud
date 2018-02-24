/// @file   mobileModel.hpp
/// @brief  Define the model of a Mobile OBject.
/// @author Enrico Fraccaroli
/// @date   24 02 2018
/// @copyright
/// Copyright (c) 2018 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///     The above copyright notice and this permission notice shall be included
///     in all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.

#pragma once

#include "character.hpp"

/// @brief Holds details about a Mobile OBject.
class MobileModel
{
public:
    /// The virtual number of the mobile object.
    unsigned int vnum;
    /// The name of the mobile.
    std::string propnoun;
    /// List of keys associated with the mobile.
    std::vector<std::string> keys;
    /// Short description.
    std::string shortdesc;
    /// Static description.
    std::string staticdesc;
    /// The complete description.
    std::string description;
    /// The original race.
    Race * race;
    /// The original faction.
    Faction * faction;
    /// The original gender.
    GenderType gender;
    /// The original weight.
    double weight;
    /// The original behaviours of the mobile.
    std::vector<std::string> actions;
    /// The original flags.
    unsigned int flags;
    /// The original level.
    unsigned int level;
    /// The original abilities.
    std::map<Ability, unsigned int> abilities;
    /// The original file that contains the behaviour of this mobile.
    std::string lua_script;

    MobileModel() :
        vnum(),
        propnoun(),
        keys(),
        shortdesc(),
        staticdesc(),
        race(),
        faction(),
        gender(),
        weight(),
        actions(),
        flags(),
        level(),
        abilities(),
        lua_script()
    {
        // Nothing to do.
    }

    /// @brief Creates a mobile using this model.
    /// @return A pointer to the newly create mobile.
    Mobile * spawn(Room * spawnRoom, unsigned int mobileVnum)
    {
        (void) spawnRoom;
        (void) mobileVnum;
        if (spawnRoom == nullptr) return nullptr;
        return nullptr;
#if 0
        // Initialize the mobile.
        auto mob = new Mobile();
        mob->id = result->getNextString();
        mob->respawnRoom = Mud::instance().findRoom(
            result->getNextUnsignedInteger());
        mob->room = mob->respawnRoom;
        mob->name = result->getNextString();
        mob->keys = GetWords(result->getNextString());
        mob->shortdesc = result->getNextString();
        mob->staticdesc = result->getNextString();
        mob->description = result->getNextString();
        mob->race =
            Mud::instance().findRace(result->getNextUnsignedInteger());
        mob->faction = Mud::instance().findFaction(
            result->getNextUnsignedInteger());
        mob->gender = static_cast<GenderType>(result->getNextInteger());
        mob->weight = result->getNextDouble();
        mob->actions = GetWords(result->getNextString());
        mob->flags = result->getNextUnsignedInteger();
        mob->level = result->getNextUnsignedInteger();
        if (!mob->setAbilities(result->getNextString()))
        {
            delete (mob);
            throw SQLiteException("Wrong characteristics.");
        }
        mob->lua_script = result->getNextString();
        mob->setHealth(mob->getMaxHealth(), true);
        mob->setStamina(mob->getMaxStamina(), true);
        // Translate new_line.
        FindAndReplace(&mob->description, "%r", "\n");
        // Check the correctness.
        if (!mob->check())
        {
            delete (mob);
            throw SQLiteException("Error during error checking.");
        }
        if (!Mud::instance().addMobile(mob))
        {
            delete (mob);
            throw SQLiteException("Error during mob insertion.");
        }
        // Respawn the mob.
        mob->respawn();
#endif
    }
};