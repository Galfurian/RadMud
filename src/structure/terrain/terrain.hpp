/// @file   terrain.hpp
/// @brief  Declaration of Terrain class.
/// @author Enrico Fraccaroli
/// @date   08/12/2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include <string>
#include <lua.hpp>

namespace terrain
{

/// Used to determine the flag of the terrain.
using TerrainFlag = enum class TerrainFlags
{
    None = 0,           ///< No flag.
    Indoor = 1,         ///< The terrain is indoor.
    NaturalLight = 2,   ///< There is natural light on this terrain.
    LiquidSource = 4,   ///< The terrain contains a source of liquid.
    LiquidRiver = 8     ///< The terrain contains a river of liquid.
};

///// Used to determine the generation flag of the terrain.
//using TerrainGenerationFlag = enum class TerrainGenerationFlags
//{
//    None = 0,                   ///< No flag.
//    CanHostLiquidSource = 2,    ///< The terrain can host a liquid source.
//    CanHostForest = 4,          ///< The terrain can host a forest.
//    //8
//};

/// @brief Holds information about a type of terrain.
class Terrain
{
public:
    /// The vnum.
    unsigned int vnum;
    /// The unique name.
    std::string name;
    /// The flags of the terrain.
    unsigned int flags;
    /// The available space inside the terrain.
    unsigned int space;
    /// The lua_State associated with this terrain.
    lua_State * L;

    /// @brief Constructor.
    Terrain();

    /// @brief Constructor.
    Terrain(unsigned int _vnum,
            std::string _name,
            unsigned int _flags,
            unsigned int _space);

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);
};

}
