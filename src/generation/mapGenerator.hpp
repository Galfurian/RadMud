/// @file   mapGenerator.hpp
/// @author Enrico Fraccaroli
/// @date   Jan 05 2017
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

#include "mapCell.hpp"
#include "map2D.hpp"
#include <memory>

class MapGenerator
{
public:
    /// @brief Constructor.
    MapGenerator();

    /// @brief Generates a new map.
    Map2D<MapCell> generateMap(int width,
                               int height,
                               int numMountains,
                               int minMountainRadius,
                               int maxMountainRadius,
                               int numRivers);

private:
    void dropMountain(Map2D<MapCell> & map,
                      int minMountainRadius,
                      int maxMountainRadius);

    void normalizeMap(Map2D<MapCell> & map);

    void dropRiver(Map2D<MapCell> & map);

    void clearMap(Map2D<MapCell> & map);

    inline double normalize(double value,
                            double LbFrom,
                            double UbFrom,
                            double LbTo,
                            double UbTo) const;
};