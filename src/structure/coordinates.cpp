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
#include "../utilities/logger.hpp"

Coordinates::Coordinates() :
    x(),
    y(),
    z()
{
    // Nothing to do.
}

Coordinates::Coordinates(const int & _x, const int & _y, const int & _z) :
    x(_x),
    y(_y),
    z(_z)
{
    // Nothing to do.
}

bool Coordinates::operator==(const Coordinates & right) const
{
    return (x == right.x) && (y == right.y) && (z == right.z);
}

bool Coordinates::operator<(const Coordinates & right) const
{
    return (x < right.x) && (y < right.y) && (z < right.z);
}

Coordinates Coordinates::operator+(const Coordinates & right) const
{
    Coordinates coord;
    coord.x = x + right.x;
    coord.y = y + right.y;
    coord.z = z + right.z;
    return coord;
}

int Coordinates::square() const
{
    return (x * x) + (y * y);
}

std::string Coordinates::toString() const
{
    return "[X:" + ToString(x) + "; Y:" + ToString(y) + "; Z:" + ToString(z) + "]";
}

void Coordinates::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L)
        .beginClass<Coordinates>("Coordinates")
        .addData("x", &Coordinates::x, false)
        .addData("y", &Coordinates::y, false)
        .addData("z", &Coordinates::z, false)
        .endClass();
}

Coordinates Coordinates::round(double x, double y, double z)
{
    float rx = static_cast<float>(std::round(x));
    float ry = static_cast<float>(std::round(y));
    float rz = static_cast<float>(std::round(z));
    float s = rx + ry + rz;
    if (!DoubleEquality(s, 0.0))
    {
        float x_err = static_cast<float>(std::abs(rx - x));
        float y_err = static_cast<float>(std::abs(ry - y));
        float z_err = static_cast<float>(std::abs(rz - z));
        if ((x_err >= y_err) && (x_err >= z_err))
        {
            rx -= s;
            if (DoubleEquality(x_err, y_err))
            {
                ry -= s;
            }
            if (DoubleEquality(x_err, z_err))
            {
                rz -= s;
            }
        }
        else if (y_err > z_err)
        {
            ry -= s;
            if (DoubleEquality(y_err, z_err))
            {
                rz -= s;
            }
        }
        else
        {
            rz -= s;
        }
    }
    return Coordinates(static_cast<int>(rx), static_cast<int>(ry), static_cast<int>(rz));
}

