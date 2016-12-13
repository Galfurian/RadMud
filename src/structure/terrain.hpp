/// @file   terrain.hpp
/// @brief  
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
    /// If there is natural light on this terrain.
    bool inside;
    /// If there is natural light on this terrain.
    bool light;
    /// The lua_State associated with this terrain.
    lua_State * L;

    /// @brief Constructor.
    Terrain();

    /// @brief Constructor.
    Terrain(unsigned int _vnum, std::string _name, unsigned int _flags, unsigned int _space, bool _inside, bool _light);

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);
};
