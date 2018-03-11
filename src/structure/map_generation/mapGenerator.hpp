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

#include "mapGeneratorConfiguration.hpp"

#include <memory>
#include <map>

class MapWrapper;
class HeightMap;
class Terrain;

/// @brief Automatic generator of maps.
class MapGenerator
{
private:
    /// Generator configuration.
    MapGeneratorConfiguration config;
    /// Height map.
    std::shared_ptr<HeightMap> heightMap;

public:

    /// @brief Constructor.
    MapGenerator(const MapGeneratorConfiguration & _config,
                 const std::shared_ptr<HeightMap> & _heightMap);

    /// @brief Generates a new map.
    bool generateMap(std::shared_ptr<MapWrapper> const & map);

private:
    struct HeightCell
    {
        int height;
        std::shared_ptr<Terrain> terrain;
    };

    /// @brief Creates the mountains on the map.
    /// @param heights The heights.
    /// @return the error state.
    bool generateMountains(
        std::map<int, std::map<int, HeightCell>> & heights);

    /// @brief Normalizes the map with values between a specific range
    /// according to the HeightMap.
    /// @param heights The heights.
    /// @return the error state.
    bool normalizeMap(
        std::map<int, std::map<int, HeightCell>> & heights);

    /// @brief Apply the terrain on the map based on the height map.
    /// @param heights The heights.
    /// @return the error state.
    bool applyTerrain(
        std::map<int, std::map<int, HeightCell>> & heights);

    /// @brief Finalizes the z coordinates of the cells inside the map around
    /// the mid-value of the altitude.
    /// @param heights The heights.
    /// @return the error state.
    bool flatOutMainland(
        std::map<int, std::map<int, HeightCell>> & heights);

    /// @brief Applies the terrains and computed heights to the map.
    /// @param heights The heights.
    /// @param map     The map to modify.
    /// @return the error state.
    bool applyHeightsToMap(
        std::map<int, std::map<int, HeightCell>> & heights,
        std::shared_ptr<MapWrapper> const & map);

    /// @brief Fills the sea cells.
    /// @param map The map to modify.
    /// @return the error state.
    bool generateSea(
        std::map<int, std::map<int, HeightCell>> & heights,
        std::shared_ptr<MapWrapper> const & map);
};