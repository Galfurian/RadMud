/// @file   mapGenerator.cpp
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

#include "mapGenerator.hpp"
#include "structureDefines.hpp"
#include "structureUtils.hpp"
#include "mapWrapper.hpp"
#include "heightMap.hpp"
#include "doubleOperators.hpp"

MapGenerator::MapGenerator(const MapGeneratorConfiguration & _config,
                           const std::shared_ptr<HeightMap> & _heightMap) :
    config(_config),
    heightMap(_heightMap)
{
    // Nothing to do.
}

bool MapGenerator::generateMap(std::shared_ptr<MapWrapper> const & map)
{
    if (map == nullptr)
    {
        Logger::log(LogLevel::Error, "No map provided.");
        return false;
    }
    if (heightMap == nullptr)
    {
        Logger::log(LogLevel::Error, "HeightMap not set.");
        return false;
    }
    // Initialize the map.
    map->initialize(config.width, config.height, config.elevation);

    // ------------------------------------------------------------------------
    // Phase I : use a 2d map to generate the terrain layer.
    // The heights of the map.
    std::map<int, std::map<int, HeightCell>> heights;
    // Generate the mountains.
    if (!this->generateMountains(heights)) return false;
    // Normalize the map in order to have values between 0 and 100.
    if (!this->normalizeMap(heights)) return false;
    // Apply the heights to the map.
    if (!this->applyTerrain(heights)) return false;
    // Flat out the mainland.
    if (!this->flatOutMainland(heights)) return false;

    // ------------------------------------------------------------------------
    //Phase II: Use the 2d map to generate the 3d map.
    // Apply the heights to the real map.
    return this->applyHeightsToMap(heights, map);
}

bool MapGenerator::generateMountains(
    std::map<int, std::map<int, HeightCell>> & heights)
{
    Logger::log(LogLevel::Debug, "Generating the mountains.");
    for (int i = 0; i < config.numMountains; ++i)
    {
        // Generate a random dimension for the mountain.
        auto radius = TRand<int>(config.minMountainRadius,
                                 config.maxMountainRadius);
        // Generate a random place for the mountain.
        auto xCenter = TRand<int>(-radius, config.width + radius);
        auto yCenter = TRand<int>(-radius, config.height + radius);
        // Determine the boundaries.
        auto xMin = std::max(xCenter - radius - 1, 0);
        auto xMax = std::min(xCenter + radius + 1, config.width - 1);
        auto yMin = std::max(yCenter - radius - 1, 0);
        auto yMax = std::min(yCenter + radius + 1, config.height - 1);
        // Evaluate the square of the radius.
        auto squareRadius = radius * radius;
        // Evaluate the height of each cell inside the boundaries.
        for (auto x = xMin; x <= xMax; ++x)
        {
            for (auto y = yMin; y <= yMax; ++y)
            {
                // Determine the distance between the cell and the center.
                auto distance = pow(xCenter - x, 2) + pow(yCenter - y, 2);
                // Determine the height of the cell based on the distance.
                auto cellHeight = std::ceil(squareRadius - distance);
                // Set the height value on the map.
                if (cellHeight > 0)
                {
                    heights[x][y].height += static_cast<int>(cellHeight);
                }
            }
        }
    }
    // Print the map for debug purpose.
    for (int y = 0; y < config.height; ++y)
    {
        std::string debug;
        for (int x = 0; x < config.width; ++x)
        {
            debug += Align(ToString(heights[x][y].height), align::center, 2);
        }
        Logger::log(LogLevel::Debug, debug);
    }
    return true;
}

bool MapGenerator::normalizeMap(
    std::map<int, std::map<int, HeightCell>> & heights)
{
    Logger::log(LogLevel::Debug, "Normalizing the map betweem 0 and 100.");
    auto minHeight = 0, maxHeight = 0;
    // Find the minimum and maximum heights.
    for (auto x = 0; x < config.width; ++x)
    {
        for (auto y = 0; y < config.height; ++y)
        {
            if (heights[x][y].height < minHeight)
            {
                minHeight = heights[x][y].height;
            }
            if (heights[x][y].height > maxHeight)
            {
                maxHeight = heights[x][y].height;
            }
        }
    }
    // Drop the map if it is quite plain.
    if (IsEqual(maxHeight, minHeight))
    {
        Logger::log(LogLevel::Error, "Min and max height are the same.");
        return false;
    }
    // Normalize the heights to values between 0 and 100.
    for (int x = 0; x < config.width; ++x)
    {
        for (int y = 0; y < config.height; ++y)
        {
            heights[x][y].height = Normalize(heights[x][y].height,
                                             minHeight, maxHeight,
                                             0, 100);
        }
    }
    // Print the map for debug purpose.
    for (int y = 0; y < config.height; ++y)
    {
        std::string debug;
        for (int x = 0; x < config.width; ++x)
        {
            debug += Align(ToString(heights[x][y].height), align::center, 2);
        }
        Logger::log(LogLevel::Debug, debug);
    }
    return true;
}

bool MapGenerator::applyTerrain(
    std::map<int, std::map<int, HeightCell>> & heights)
{
    Logger::log(LogLevel::Debug, "Applying the terrain to the map.");
    for (auto x = 0; x < config.width; ++x)
    {
        for (auto y = 0; y < config.height; ++y)
        {
            // Get the terrain associated with the cell height.
            auto terrain = heightMap->getTerrain(heights[x][y].height);
            if (terrain == nullptr)
            {
                Logger::log(LogLevel::Error, "Applying a terrain.");
                return false;
            }
            heights[x][y].terrain = terrain;
        }
    }
    // Print the map for debug purpose.
    for (int y = 0; y < config.height; ++y)
    {
        std::string debug;
        for (int x = 0; x < config.width; ++x)
        {
            debug += heights[x][y].terrain->symbol;
        }
        Logger::log(LogLevel::Debug, debug);
    }
    return true;
}

bool MapGenerator::flatOutMainland(
    std::map<int, std::map<int, HeightCell>> & heights)
{
    Logger::log(LogLevel::Debug, "Levelling mainland terrains.\n");
    for (auto x = 0; x < config.width; ++x)
    {
        for (auto y = 0; y < config.height; ++y)
        {
            if (heightMap->isAboveSeaLevel(heights[x][y].terrain))
            {
                auto cellOffset = heightMap->getOffset(heights[x][y].terrain);
                heights[x][y].height = (config.elevation / 2) +
                                       ((cellOffset < 0) ? 0 : cellOffset);
            }
        }
    }
    // Print the map for debug purpose.
    for (int y = 0; y < config.height; ++y)
    {
        std::string debug;
        for (int x = 0; x < config.width; ++x)
        {
            debug += Align(ToString(heights[x][y].height), align::center, 2);
        }
        Logger::log(LogLevel::Debug, debug);
    }
    return true;
}

bool MapGenerator::applyHeightsToMap(
    std::map<int, std::map<int, HeightCell>> & heights,
    std::shared_ptr<MapWrapper> const & map)
{
    Logger::log(LogLevel::Debug, "Applying heights to the real map.\n");
    for (auto x = 0; x < config.width; ++x)
    {
        for (auto y = 0; y < config.height; ++y)
        {
            MapCell cell;
            cell.terrain = heights[x][y].terrain;
            map->set(x, y, heights[x][y].height, cell);
        }
    }
    return true;
}

bool MapGenerator::generateSea(std::shared_ptr<MapWrapper> const & /*map*/)
{
    return true;
}
