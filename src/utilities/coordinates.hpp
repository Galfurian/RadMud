/// @file    coordinates.hpp
/// @brief   Define the class Coordinates.
/// @details This class allows to hadle localization by means of the three main coordinates,
///           x;y;z, and also define a series of operators useful for coordinates manipulation
///           and comparison.
/// @author  Enrico Fraccaroli
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

#pragma once

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "../lua/luabridge/LuaBridge.h"

/// Used to create and manage a tridimensional coordinates.
class Coordinates
{
public:
    /// Coordinate on the x axis.
    int x;
    /// Coordinate on the y axis.
    int y;
    /// Coordinate on the z axis.
    int z;

    Coordinates();

    /// @brief It the constructor, needed to create a set of Coordinates.
    /// @param _x Coordinate on width axis.
    /// @param _y Coordinate on heigth axis.
    /// @param _z Coordinate on altitude axis.
    Coordinates(const int & _x, const int & _y, const int & _z);

    /// @brief Define operator ==, equal.
    /// @param right The right parameter.
    /// @return True if left Coordinates are equal to the right Coordinates.
    bool operator==(const Coordinates & right) const;

    /// @brief Define operator <, less than.
    /// @param right The right parameter.
    /// @return True if left Coordinates are less than right Coordinates.
    bool operator<(const Coordinates & right);

    /// @brief Add to the current Coordinates the right operand.
    /// @param right The right parameter.
    /// @return The new Coordinates.
    Coordinates operator+(const Coordinates & right);

    /// @brief Provides the string version of the coordinates.
    /// @return A string representing the coordinates.
    std::string toString() const;

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);
};
