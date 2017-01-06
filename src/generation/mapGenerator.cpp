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
#include "heightMapper.hpp"
#include "utils.hpp"
#include "area.hpp"

MapGenerator::MapGenerator(const MapGeneratorConfiguration & _configuration) :
    configuration(_configuration)
{
    // Nothing to do.
}

Map2D<MapCell> MapGenerator::generateMap()
{
    // Prepare the map.
    Map2D<MapCell> map(configuration.width, configuration.height, MapCell());
    // Initialize the map cells.
    for (auto x = 0; x < map.getWidth(); ++x)
    {
        for (auto y = 0; y < map.getHeight(); ++y)
        {
            map.get(x, y).coordinates = Coordinates(x, y, 0);
            map.get(x, y).addNeighbours(map);
        }
    }
    // Drop the mountains.
    for (int i = 0; i < configuration.numMountains; ++i)
    {
        this->dropMountain(map);
    }
    // Normalize the map in order to have values between 0 and 1.
    this->normalizeMap(map);
    // Apply the heights to the map.
    HeightMapper heightMapper;
    heightMapper.setNormalThresholds();
    heightMapper.applyHeightMap(map);
    // Drop the rivers.
    this->dropRivers(map);
    return map;
}

void MapGenerator::dropMountain(Map2D<MapCell> & map)
{
    // Generate a random dimension for the mountain.
    auto radius = TRandInteger<int>(configuration.minMountainRadius,
                                    configuration.maxMountainRadius);
    // Generate a random place for the mountain.
    auto xCenter = TRandInteger<int>(-radius, map.getWidth() + radius);
    auto yCenter = TRandInteger<int>(-radius, map.getHeight() + radius);
    // Determine the boundaries.
    auto xMin = std::max(xCenter - radius - 1, 0);
    auto xMax = std::min(xCenter + radius + 1, map.getWidth() - 1);
    auto yMin = std::max(yCenter - radius - 1, 0);
    auto yMax = std::min(yCenter + radius + 1, map.getHeight() - 1);
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
            auto cellHeight = squareRadius - distance;
            // Set the height value on the map.
            if (cellHeight > 0)
            {
                map.get(x, y).height += cellHeight;
            }
        }
    }
}

void MapGenerator::normalizeMap(Map2D<MapCell> & map)
{
    auto minHeight = 0.0, maxHeight = 0.0;
    // Find the minimum and maximum heights.
    for (auto x = 0; x < map.getWidth(); ++x)
    {
        for (auto y = 0; y < map.getHeight(); ++y)
        {
            auto cellHeight = map.get(x, y).height;
            if (cellHeight < minHeight) minHeight = cellHeight;
            if (cellHeight > maxHeight) maxHeight = cellHeight;
        }
    }
    // If the map is quite plain, clear it.
    if (!DoubleEquality(maxHeight, minHeight))
    {
        // Normalize the heights to values between 0 and 10.
        for (int x = 0; x < map.getWidth(); ++x)
        {
            for (int y = 0; y < map.getHeight(); ++y)
            {
                map.get(x, y).height = this->normalize(map.get(x, y).height,
                                                       minHeight, maxHeight,
                                                       0.0, 100.0);
            }
        }
    }
}

void MapGenerator::dropRivers(Map2D<MapCell> & map)
{
    std::list<MapCell *> startingPoints;
    // Lamba used to check if a cell is far away from pre-existing starting
    // points and if it is a mountain.
    auto IsSuitable = [&](MapCell * cell)
    {
        if (cell->heightMap != HeightMap::Mountain)
        {
            return false;
        }
        for (auto point : startingPoints)
        {
            auto distance = Area::getDistance(cell->coordinates,
                                              point->coordinates);
            if (distance <= configuration.minRiverDistance)
            {
                return false;
            }
        }
        return true;
    };
    // Retrieve all the starting points for rivers.
    for (auto x = 0; x < map.getWidth(); ++x)
    {
        for (auto y = 0; y < map.getHeight(); ++y)
        {
            auto cell = &map.get(x, y);
            if (IsSuitable(cell))
            {
                startingPoints.emplace_back(cell);
            }
        }
    }
    auto iterations = std::min(static_cast<size_t >(configuration.numRivers),
                               startingPoints.size());
    // Prepare a vector for the rivers.
    std::vector<std::vector<MapCell *>> rivers;
    for (unsigned int it = 0; it < iterations; ++it)
    {
        // Prepare a vector for the river.
        std::vector<MapCell *> river;
        // Set the starting cell.
        MapCell * cell = startingPoints.front();
        startingPoints.pop_front();
        // Add the starting cell to the river.
        river.push_back(cell);
        while (true)
        {
            // Get the lowest nearby cell.
            auto nextCell = cell->findLowestNearbyCell();
            // Check if the lowest is the current cell itself.
            if (nextCell->coordinates == cell->coordinates)
            {
                break;
            }
            if ((nextCell->heightMap == HeightMap::ShallowWater) ||
                (nextCell->heightMap == HeightMap::DeepWater))
            {
                break;
            }
            // Add the next cell to the path of the river.
            river.push_back(nextCell);
            // Set the current cell to the next cell.
            cell = nextCell;
        }
        rivers.emplace_back(river);
    }
    // Modify the height of the river.
    for (auto river : rivers)
    {
        for (auto it = river.begin(); it != river.end(); ++it)
        {
            auto cell = (*it);
            if (it == river.begin())
            {
                cell->height = 0;
                cell->heightMap = HeightMap::DeepWater;
                continue;
            }
            cell->height -= 1;
            cell->heightMap = HeightMap::ShallowWater;
        }
    }
}

void MapGenerator::clearMap(Map2D<MapCell> & map)
{
    for (int x = 0; x < map.getWidth(); ++x)
    {
        for (int y = 0; y < map.getHeight(); ++y)
        {
            map.get(x, y).height = 0.0;
        }
    }
}

double MapGenerator::normalize(double value,
                               double LbFrom,
                               double UbFrom,
                               double LbTo,
                               double UbTo) const
{
    return (((UbTo - LbTo) * (value - LbFrom)) / ((UbFrom - LbFrom))) + LbTo;
}