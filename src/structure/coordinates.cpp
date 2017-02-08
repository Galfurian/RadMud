/// @file   coordinates.cpp
/// @brief  Implements the methods of the class Coordinates.
/// @author Enrico Fraccaroli
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

#include "coordinates.hpp"
#include "logger.hpp"

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
    if (x != right.x) return false;
    if (y != right.y) return false;
    return (z == right.z);
}

bool Coordinates::operator!=(const Coordinates & right) const
{
    if (x != right.x) return true;
    if (y != right.y) return true;
    return (z != right.z);
}

bool Coordinates::operator<(const Coordinates & right) const
{
    if (x < right.x) return true;
    if (x > right.x) return false;
    if (y < right.y) return true;
    if (y > right.y) return false;
    if (z < right.z) return true;
    if (z > right.z) return false;
    return false;
}

Coordinates Coordinates::operator+(const Coordinates & right) const
{
    return Coordinates(x + right.x, y + right.y, z + right.z);
}

int Coordinates::square() const
{
    return (x * x) + (y * y);
}

std::string Coordinates::toString() const
{
    return "[X:" + ToString(x) + ";" +
           " Y:" + ToString(y) + ";" +
           " Z:" + ToString(z) + "]";
}

Coordinates Coordinates::round(double x, double y, double z)
{
    double rx = std::round(x), ry = std::round(y), rz = std::round(z);
    double s = rx + ry + rz;
    if (!DoubleEquality(s, 0.0))
    {
        double x_err = std::abs(rx - x);
        double y_err = std::abs(ry - y);
        double z_err = std::abs(rz - z);
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
    return Coordinates(static_cast<int>(rx),
                       static_cast<int>(ry),
                       static_cast<int>(rz));
}

void Coordinates::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L)
        .beginClass<Coordinates>("Coordinates")
        .addFunction("toString", &Coordinates::toString)
        .addData("x", &Coordinates::x, false)
        .addData("y", &Coordinates::y, false)
        .addData("z", &Coordinates::z, false)
        .endClass();
}
