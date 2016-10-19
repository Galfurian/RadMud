/// @file   faction.hpp
/// @brief  Define the faction variables.
/// @author Enrico Fraccaroli
/// @date   Sep 30 2014
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

#include <list>
#include <string>
#include <vector>
#include <map>

#include "lua/lua_script.hpp"
#include "utilities/table.hpp"

class CurrencyModel;

/// @brief Holds details about a faction.
class Faction
{
public:
    /// The faction identification number.
    int vnum;
    /// The name of the faction.
    std::string name;
    /// The description of the faction.
    std::string description;
    /// The currency used by this faction.
    CurrencyModel * currency;

    /// @brief Contructor.
    Faction();

    /// @brief Disable Copy Construct.
    Faction(Faction const &) = delete;

    /// @brief Disable Move construct.
    Faction(Faction &&) = delete;

    /// @brief Disable Copy assign.
    Faction & operator=(Faction const &) = delete;

    /// @brief Disable Move assign.
    Faction & operator=(Faction &&) = delete;

    /// @brief Destructor.
    ~Faction();

    /// @brief Check the correctness of the faction.
    /// @return <b>True</b> if the faction has correct values,<br>
    ///         <b>False</b> otherwise.
    bool check();

    /// @brief Fills the provided table with the information concerning the faction.
    /// @param sheet The table that has to be filled.
    void getSheet(Table & sheet) const;

    /// @brief Return the name of the faction.
    /// @return The name of the faction.
    std::string getName();

    /// @brief Return the name of the faction with the first letter capitalized.
    /// @return The name of the faction.
    std::string getNameCapital();

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);
};
