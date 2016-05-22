/// @file   coordinates.cpp
/// @brief  Implements the methods of the class Coordinates.
/// @author Enrico Fraccaroli
/// @date   Sep 8 2015
/// @copyright
/// Copyright (c) 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "coordinates.hpp"
#include "../luabridge/LuaBridge.h"
#include "../lua/lua_script.hpp"

Coordinates::Coordinates()
{
    x = 0;
    y = 0;
    z = 0;
}

Coordinates::Coordinates(const int _x, const int _y, const int _z) :
        x(_x),
        y(_y),
        z(_z)
{
}

Coordinates Coordinates::operator+(const Coordinates &right) const
{
    Coordinates coord;
    coord.x = x + right.x;
    coord.y = y + right.y;
    coord.z = z + right.z;
    return coord;
}

bool Coordinates::operator<(const Coordinates &right) const
{
    if (x < right.x)
    {
        return true;
    }
    else
    {
        return false;
    }
    if (y < right.y)
    {
        return true;
    }
    else
    {
        return false;
    }
    if (z < right.z)
    {
        return true;
    }
    else
    {
        return false;
    }

    return true;
}

bool Coordinates::operator==(const Coordinates &right) const
{
    if (x != right.x)
    {
        return false;
    }
    if (y != right.y)
    {
        return false;
    }
    if (z != right.z)
    {
        return false;
    }
    return true;
}

void Coordinates::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass<Coordinates>("Coordinates") //
    .addData("x", &Coordinates::x, false) //
    .addData("y", &Coordinates::y, false) //
    .addData("z", &Coordinates::z, false) //
    .endClass();
}
