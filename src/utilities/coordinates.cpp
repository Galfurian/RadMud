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

/// @brief Define operator <, less than.
/// @param right The right parameter.
/// @return True if left Coordinates are less than right Coordinates.

#include "coordinates.hpp"

template<>
template<>
bool Coordinates<unsigned int>::operator<(const Coordinates<int> & right) const
{
    if (static_cast<int>(x) < right.x)
    {
        return true;
    }
    if (static_cast<int>(y) < right.y)
    {
        return true;
    }
    if (static_cast<int>(z) < right.z)
    {
        return true;
    }
    return false;
}

template<>
template<>
Coordinates<unsigned int> Coordinates<unsigned int>::operator+(const Coordinates<int> & right) const
{
    if ((*this) < right)
    {
        throw std::runtime_error("Wrong type of coordinates.");
    }
    Coordinates<unsigned int> coord;
    coord.x = x + static_cast<unsigned int>(right.x);
    coord.y = y + static_cast<unsigned int>(right.y);
    coord.z = z + static_cast<unsigned int>(right.z);
    return coord;
}

template class Coordinates<int> ;
template class Coordinates<unsigned int> ;
