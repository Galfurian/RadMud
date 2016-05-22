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

#ifndef RACE_HPP
#define RACE_HPP

#include <vector>
#include <string>
#include <list>
#include <map>
#include "lua/lua_script.hpp"
#include "material.hpp"

/// @brief Holds details about a race.
class Race
{
    public:
        /// The race virtual number.
        int vnum;
        /// The name of the race.
        std::string name;
        /// The description of the race.
        std::string description;
        /// The material of which are made the entity of this race.
        Material * material;
        /// The base strength value of the race.
        int strength;
        /// The base agility value of the race.
        int agility;
        /// The base perception value of the race.
        int perception;
        /// The base constitution value of the race.
        int constitution;
        /// The base intelligence value of the race.
        int intelligence;
        /// The factions that a player of this race can choose.
        std::vector<int> available_faction;
        /// The flag that indicates if the race is selectable by the player during
        ///  character creation.
        bool player_allow;
        /// TileSet of the icon.
        int tileSet;
        /// TileId of the icon.
        int tileId;
        /// The corpse description.
        std::string corpseDescription;

        /// @brief Constructor.
        Race();

        /// @brief Copy Constructor.
        Race(const Race & source);

        /// @brief Destructor.
        ~Race();

        /// @brief Check the correctness of the race.
        /// @return <b>True</b> if the race has correct values,<br>
        ///         <b>False</b> otherwise.
        bool check();

        /// @brief Given a source string, this function parse the string
        ///         and sets the rate characteristics.
        /// @param source Contains the list of characteristics.
        /// @return <b>True</b> if the operation succeeded,<br>
        ///         <b>False</b> otherwise.
        bool setCharacteristic(std::string source);

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

/// Race list handler.
typedef std::vector<Race> RaceList;

/// Race map handler.
typedef std::map<int, Race> RaceMap;

#endif
