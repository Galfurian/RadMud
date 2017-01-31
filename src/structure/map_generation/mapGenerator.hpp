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
#include "utils.hpp"
#include "heightMapper.hpp"
#include "mapGeneratorConfiguration.hpp"

#include <memory>

/// @brief Automatic generator of maps.
class MapGenerator
{
public:
    /// Generator configuration.
    MapGeneratorConfiguration configuration;
    /// Height mapper.
    HeightMapper heightMapper;

    /// @brief Constructor.
    MapGenerator(const MapGeneratorConfiguration & _configuration,
                 const HeightMapper & _heightMapper);

    /// @brief Generates a new map.
    Map2D<MapCell> generateMap();

private:
    /// @brief Creates a mountain on the map.
    void dropMountain(Map2D<MapCell> & map);

    /// @brief Normalizes the map with values between a specific range
    /// according to the HeightMapper.
    void normalizeMap(Map2D<MapCell> & map);

    /// @brief Creates the rivers on the map.
    void dropRivers(Map2D<MapCell> & map);

    /// @brief Add the forests to the map.
    void addForests(Map2D<MapCell> & map);

    /// @brief Clears the map.
    void clearMap(Map2D<MapCell> & map);

    /// @brief Sets to each cell the tile based on the HeightMapper.
    void applyMapTiles(Map2D<MapCell> & map);

    /// @brief Normalizes the value from the range (LbFrom, UbFrom) to the
    /// range (LbTo, UbTo).
    /// @param value  The value that has to be normalized.
    /// @param LbFrom The lower bound of the original range.
    /// @param UbFrom The upper bound of the original range.
    /// @param LbTo   The lower bound of the destiantion range.
    /// @param UbTo   The upper bound of the destiantion range.
    /// @return The normalized value.
    inline double normalize(double value,
                            double LbFrom,
                            double UbFrom,
                            double LbTo,
                            double UbTo) const;
};