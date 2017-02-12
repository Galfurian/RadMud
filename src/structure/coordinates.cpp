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

Coordinates::Coordinates(const double & _x,
                         const double & _y,
                         const double & _z) :
    x(static_cast<int>(_x)),
    y(static_cast<int>(_y)),
    z(static_cast<int>(_z))
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
