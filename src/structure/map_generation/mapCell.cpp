/// @file   mapCell.cpp
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

#include "mapCell.hpp"
#include "logger.hpp"

MapCell::MapCell() :
    room(),
    coordinates(),
    terrain(),
    neighbours(),
    flags(),
    liquid()
{
    // Nothing to do.
}

bool MapCell::addNeighbour(const Direction & direction,
                           MapCell * mapCell)
{
    if (mapCell == nullptr)
    {
        return false;
    }
    return neighbours.insert(std::make_pair(direction, mapCell)).second;
}

MapCell * MapCell::findLowestNearbyCell()
{
    MapCell * selectedCell = this;
    for (auto neighbour : neighbours)
    {
        if (neighbour.second->coordinates.z < selectedCell->coordinates.z)
        {
            selectedCell = neighbour.second;
        }
    }
    return selectedCell;
}

std::string MapCell::getTile() const
{
    if (liquid.first != nullptr)
    {
        return "w";
    }
    if (HasFlag(flags, RoomFlags::SpawnTree))
    {
        return "t";
    }
    return terrain->symbol;
}

bool MapCell::operator==(const MapCell & other) const
{
    return (coordinates == other.coordinates);
}
