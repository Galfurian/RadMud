/// @file   mapCell.hpp
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

#pragma once

#include "structure/terrain/terrain.hpp"
#include "structure/room.hpp"

#include <vector>

/// @brief Holds information about the cell of an automatically generated map.
class MapCell
{
public:
    /// The associated room.
    Room * room;
    /// The cell coordinates.
    Coordinates coordinates;
    /// Associated tile.
    std::shared_ptr<Terrain> terrain;
    /// List of neighbours.
    std::map<Direction, MapCell *> neighbours;
    /// The flags of the room.
    unsigned int flags;
    /// The liquid which will fill the room.
    std::pair<Liquid *, unsigned int> liquidContent;

    /// @brief Constructor.
    MapCell();

    /// @brief Tries to add the given cell map in the given direction.
    bool addNeighbour(const Direction & direction, MapCell * mapCell);

    /// @brief Find the lowest nearby cell.
    /// @return The found cell.
    MapCell * findLowestNearbyCell();

    /// @brief Returns the tile describing the cell.
    std::string getTile() const;

    /// @brief Equality operator.
    bool operator==(const MapCell & other) const;
};