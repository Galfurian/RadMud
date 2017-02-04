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

MapCell::MapCell() :
    room(),
    coordinates(),
    height(),
    terrain(),
    neighbours()
{
    // Nothing to do.
}

void MapCell::addNeighbours(Map2D<MapCell> & map)
{
    if (coordinates.x - 1 > 0)
    {
        neighbours.emplace_back(&map.get(coordinates.x - 1, coordinates.y));
    }
    if (coordinates.x + 1 < map.getWidth())
    {
        neighbours.emplace_back(&map.get(coordinates.x + 1, coordinates.y));
    }
    if (coordinates.y - 1 > 0)
    {
        neighbours.emplace_back(&map.get(coordinates.x, coordinates.y - 1));
    }
    if (coordinates.y + 1 < map.getHeight())
    {
        neighbours.emplace_back(&map.get(coordinates.x, coordinates.y + 1));
    }
}

MapCell * MapCell::findLowestNearbyCell()
{
    MapCell * selectedCell = this;
    for (auto neighbour : neighbours)
    {
        if (neighbour->height < selectedCell->height)
        {
            selectedCell = neighbour;
        }
    }
    if (selectedCell == this)
    {

    }
    return selectedCell;
}

std::string MapCell::getTile() const
{
    if (room != nullptr)
    {
        if (room->liquid.first != nullptr)
        {
            return "w";
        }
        if (HasFlag(room->flags, RoomFlags::SpawnTree))
        {
            return "t";
        }
        return room->terrain->symbol;
    }
    return "X";
}

bool MapCell::operator==(const MapCell & other) const
{
    return (coordinates == other.coordinates);
}
