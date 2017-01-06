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

#include "map2D.hpp"
#include <vector>

/// @brief Holds information about the cell of an automatically generated map.
class MapCell
{
public:
    /// The cell coordinates.
    Coordinates coordinates;
    /// The height of the cell.
    double height;
    /// List of neighbours.
    std::vector<MapCell *> neighbours;

    /// @brief Constructor.
    MapCell();

    /// @brief Constructor.
    MapCell(const Coordinates & _coordinates,
            const double & _height);

    /// @brief Add the neighbours based on the given map.
    void addNeighbours(Map2D<MapCell> & map);

    /// @brief Find all the nearby cells w.r.t. the policy (lowest/highest).
    /// @param lowest If true it resturns the lowest cell, the highest
    ///               otherwise.
    /// @return The found cell.
    MapCell * findCellNearby(bool lowest);

    /// @brief Equality operator.
    bool operator==(const MapCell & other) const;
};