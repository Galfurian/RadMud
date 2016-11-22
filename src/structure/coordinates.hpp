/// @file    coordinates.hpp
/// @brief   Define the class Coordinates.
/// @details This class allows to hadle localization by means of the three main coordinates,
///           x;y;z, and also define a series of operators useful for coordinates manipulation
///           and comparison.
/// @author  Enrico Fraccaroli
/// @date   Sep 8 2015
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

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "luaBridge.hpp"

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

    /// @brief Define operator !=, equal.
    /// @param right The right parameter.
    /// @return True if left Coordinates are different to the right Coordinates.
    bool operator!=(const Coordinates & right) const;

    /// @brief Define operator <, less than.
    /// @param right The right parameter.
    /// @return True if left Coordinates are less than right Coordinates.
    bool operator<(const Coordinates & right) const;

    /// @brief Add to the current Coordinates the right operand.
    /// @param right The right parameter.
    /// @return The new Coordinates.
    Coordinates operator+(const Coordinates & right) const;

    /// @brief Evaluates the square.
    /// @return The square.
    int square() const;

    /// @brief Provides the string version of the coordinates.
    /// @return A string representing the coordinates.
    std::string toString() const;

    /// @brief Evaluates the round of the given coordinates.
    /// @param x The coordinate on the x axis.
    /// @param y The coordinate on the y axis.
    /// @param z The coordinate on the z axis.
    /// @return The interger version of the coordinates.
    static Coordinates round(double x, double y, double z);

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);
};
