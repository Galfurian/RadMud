/// @file   race.hpp
/// @brief  Define the race variables.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#pragma once

#include <map>
#include <string>
#include <vector>

#include "corpseModel.hpp"
#include "defines.hpp"
#include "faction.hpp"

class Material;

/// @brief Holds details about a race.
class Race
{
public:
    /// The race virtual number.
    int vnum;
    /// The article for the race.
    std::string article;
    /// The name of the race.
    std::string name;
    /// The description of the race.
    std::string description;
    /// The material of which are made the entity of this race.
    Material * material;
    /// Base race abilities.
    std::map<Ability, unsigned int> abilities;
    /// The factions that a player of this race can choose.
    std::vector<Faction *> availableFaction;
    /// The flag that indicates if the race is selectable by the player during
    ///  character creation.
    bool player_allow;
    /// TileSet of the icon.
    int tileSet;
    /// TileId of the icon.
    int tileId;
    /// Corpse model.
    CorpseModel corpse;

    /// @brief Constructor.
    Race();

    /// @brief Disable Copy Construct.
    Race(Race const &) = delete;

    /// @brief Disable Move construct.
    Race(Race &&) = delete;

    /// @brief Disable Copy assign.
    Race & operator=(Race const &) = delete;

    /// @brief Disable Move assign.
    Race & operator=(Race &&) = delete;

    /// @brief Destructor.
    ~Race();

    /// @brief Initialize the corpse specific of this race.
    void initializeCorpse(const std::string & corpseDescription);

    /// @brief Check the correctness of the race.
    /// @return <b>True</b> if the race has correct values,<br>
    ///         <b>False</b> otherwise.
    bool check();

    /// @brief Return the short description of this race.
    /// @param capital If true the return description starts with a capital.
    /// @return The short description.
    std::string getShortDescription(bool capital = false);

    /// @brief Given a source string, this function parse the string
    ///         and sets the race abilities.
    /// @param source Contains the list of abilities.
    /// @return <b>True</b> if the operation succeeded,<br>
    ///         <b>False</b> otherwise.
    bool setAbilities(std::string source);

    /// @brief Given a source string, this function parse the string
    ///         and sets the list of available factions for this race.
    /// @param source Contains the list of available factions.
    /// @return <b>True</b> if the operation succeeded,<br>
    ///         <b>False</b> otherwise.
    bool setAvailableFactions(const std::string & source);

    /// @brief Provides the value of the given ability.
    /// @param ability The ability to retrieve.
    /// @return The overall ability value.
    unsigned int getAbility(const Ability & ability) const;

    /// @brief Provides the value of the given ability IF the ability number is correct.
    /// @param abilityNumber The ability to retrieve.
    /// @return The overall ability value (if the number is not valid it returns 0).
    unsigned int getAbilityLua(const unsigned int & abilityNumber);

    /// @brief Given a faction vnum, answer True if it's compatible with this race.
    /// @param factionVnum The given faction vnum.
    /// @return If the faction is compatible.
    bool factionAllowed(int factionVnum);

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);

    /// @brief Returns the tile of the race.
    /// @return The string which contains the code of the race's tile.
    std::string getTile();
};
