/// @file   terrain.cpp
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

#include "terrain.hpp"
#include "LuaBridge.hpp"

Terrain::Terrain() :
    vnum(),
    name(),
    flags(),
    space(),
    inside(),
    light()
{
    // Nothing to do.
}

Terrain::Terrain(unsigned int _vnum,
                 std::string _name,
                 unsigned int _flags,
                 unsigned int _space,
                 bool _inside,
                 bool _light) :
    vnum(_vnum),
    name(_name),
    flags(_flags),
    space(_space),
    inside(_inside),
    light(_light)
{
    // Nothing to do.
}

void Terrain::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L)
        .beginClass<Terrain>("Terrain")
        .addData("vnum", &Terrain::vnum, false)
        .addData("name", &Terrain::name, false)
        .addData("flags", &Terrain::flags, false)
        .addData("space", &Terrain::space, false)
        .addData("light", &Terrain::light, false)
        .endClass();
}
