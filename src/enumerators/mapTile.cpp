/// @file   mapTile.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 06 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "mapTile.hpp"
#include "formatter.hpp"

MapTile::MapTile(const unsigned int & _mapTile) :
    mapTile()
{
    if (_mapTile == 0) mapTile = Void;
    else if (_mapTile == 1) mapTile = DeepWater;
    else if (_mapTile == 2) mapTile = ShallowWater;
    else if (_mapTile == 3) mapTile = Coast;
    else if (_mapTile == 4) mapTile = Plain;
    else if (_mapTile == 5) mapTile = Forest;
    else if (_mapTile == 6) mapTile = River;
    else if (_mapTile == 7) mapTile = LiquidSource;
    else if (_mapTile == 8) mapTile = Hill;
    else if (_mapTile == 9) mapTile = Mountain;
    else if (_mapTile == 10) mapTile = HighMountain;
    else mapTile = Void;
}

MapTile::MapTile(const Enum & _mapTile) :
    mapTile(_mapTile)
{
    // Nothing to do.
}

bool MapTile::isValid(const unsigned int & _mapTile)
{
    return (_mapTile <= 10);
}

std::string MapTile::toString() const
{
    if (mapTile == MapTile::Void) return "Void";
    if (mapTile == MapTile::DeepWater) return "Deep Water";
    if (mapTile == MapTile::ShallowWater) return "Shallow Water";
    if (mapTile == MapTile::Coast) return "Coast";
    if (mapTile == MapTile::Plain) return "Plain";
    if (mapTile == MapTile::Forest) return "Forest";
    if (mapTile == MapTile::River) return "River";
    if (mapTile == MapTile::LiquidSource) return "Liquid Source";
    if (mapTile == MapTile::Hill) return "Hill";
    if (mapTile == MapTile::Mountain) return "Mountain";
    else return "HighMountain";
}

unsigned int MapTile::toUInt() const
{
    return static_cast<unsigned int>(mapTile);
}

std::string MapTile::toSymbol(bool color) const
{
    if (mapTile == MapTile::Void)
    {
        return ((color) ? Formatter::reset() : "") + " ";
    }
    if (mapTile == MapTile::DeepWater)
    {
        return ((color) ? Formatter::blue() : "") + "W";
    }
    if (mapTile == MapTile::ShallowWater)
    {
        return ((color) ? Formatter::cyan() : "") + "w";
    }
    if (mapTile == MapTile::Coast)
    {
        return ((color) ? Formatter::yellow() : "") + "c";
    }
    if (mapTile == MapTile::Plain)
    {
        return ((color) ? Formatter::reset() : "") + ".";
    }
    if (mapTile == MapTile::Forest)
    {
        return ((color) ? Formatter::green() : "") + "f";
    }
    if (mapTile == MapTile::River)
    {
        return ((color) ? Formatter::cyan() : "") + "r";
    }
    if (mapTile == MapTile::LiquidSource)
    {
        return ((color) ? Formatter::blue() : "") + "S";
    }
    if (mapTile == MapTile::Hill)
    {
        return ((color) ? Formatter::brown() : "") + "h";
    }
    if (mapTile == MapTile::Mountain)
    {
        return ((color) ? Formatter::gray() : "") + "m";
    }
    return ((color) ? Formatter::darkGray() : "") + "M";
}

bool MapTile::operator==(const MapTile & rhs) const
{
    return mapTile == rhs.mapTile;
}

bool MapTile::operator!=(const MapTile & rhs) const
{
    return mapTile != rhs.mapTile;
}

bool MapTile::operator>(const MapTile & rhs) const
{
    return mapTile > rhs.mapTile;
}

bool MapTile::operator>=(const MapTile & rhs) const
{
    return mapTile >= rhs.mapTile;
}

bool MapTile::operator<(const MapTile & rhs) const
{
    return mapTile < rhs.mapTile;
}

bool MapTile::operator<=(const MapTile & rhs) const
{
    return mapTile <= rhs.mapTile;
}
