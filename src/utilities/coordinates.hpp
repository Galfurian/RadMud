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

#ifndef COORDINATES_HPP
#define COORDINATES_HPP

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "../luabridge/LuaBridge.h"
#include "../utils.hpp"

/// Used to create and manage a tridimensional coordinates.
template<typename CoordType>
class Coordinates
{
    public:
        /// Coordinate on the x axis.
        CoordType x;
        /// Coordinate on the y axis.
        CoordType y;
        /// Coordinate on the z axis.
        CoordType z;

        Coordinates() :
            x(),
            y(),
            z()
        {
            // Nothing to do.
        }

        /// @brief It the constructor, needed to create a set of Coordinates.
        /// @param _x Coordinate on width axis.
        /// @param _y Coordinate on heigth axis.
        /// @param _z Coordinate on altitude axis.
        Coordinates(const CoordType & _x, const CoordType & _y, const CoordType & _z) :
            x(_x),
            y(_y),
            z(_z)
        {
            // Nothing to do.
        }

        /// @brief Define operator ==, equal.
        /// @param right The right parameter.
        /// @return True if left Coordinates are equal to the right Coordinates.
        bool operator==(const Coordinates<CoordType> & right) const
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

        /// @brief Define operator <, less than.
        /// @param right The right parameter.
        /// @return True if left Coordinates are less than right Coordinates.
        bool operator<(const Coordinates<CoordType> & right)
        {
            if (x < right.x)
            {
                return true;
            }
            if (y < right.y)
            {
                return true;
            }
            if (z < right.z)
            {
                return true;
            }
            return false;
        }

        /// @brief Add to the current Coordinates the right operand.
        /// @param right The right parameter.
        /// @return The new Coordinates.
        Coordinates<CoordType> operator+(const Coordinates<CoordType> & right)
        {
            Coordinates<CoordType> coord;
            coord.x = x + right.x;
            coord.y = y + right.y;
            coord.z = z + right.z;
            return coord;
        }

        /// @brief Provides the string version of the coordinates.
        /// @return A string representing the coordinates.
        std::string toString() const
        {
            return "[X:" + ToString(x) + "; Y:" + ToString(y) + "; Z:" + ToString(z) + "]";
        }

        /// @brief Function used to register inside the lua environment the class.
        /// @param L The lua environment.
        static void luaRegister(lua_State * L)
        {
            luabridge::getGlobalNamespace(L) //
            .beginClass<Coordinates<CoordType>>("Coordinates") //
            .addData("x", &Coordinates<CoordType>::x, false) //
            .addData("y", &Coordinates<CoordType>::y, false) //
            .addData("z", &Coordinates<CoordType>::z, false) //
            .endClass();
        }
};

#endif
