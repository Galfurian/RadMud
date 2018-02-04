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
#include "logger.hpp"
#include "area.hpp"
#include "structureUtils.hpp"

MapGenerator::MapGenerator(const MapGeneratorConfiguration & _configuration,
                           const std::shared_ptr<HeightMap> & _heightMap) :
    configuration(_configuration),
    heightMap(_heightMap)
{
    // Nothing to do.
}

bool MapGenerator::generateMap(const std::shared_ptr<MapWrapper> & map)
{
    // Initialize the map.
    if (!this->initializeMap(map))
    {
        Logger::log(LogLevel::Error, "While initializing the map.");
        return false;
    }
    // Generate the mountains.
    if (!this->generateMountains(map))
    {
        Logger::log(LogLevel::Error, "While generating the mountains.");
        return false;
    }
    // Normalize the map in order to have values between 0 and 100.
    if (!this->normalizeMap(map))
    {
        Logger::log(LogLevel::Error, "While normalizing the map.");
        return false;
    }
    // Apply the heights to the map.
    if (!this->applyTerrain(map))
    {
        Logger::log(LogLevel::Error, "While applying the terrains to the map.");
        return false;
    }
    // Generate the rivers.
    if (!this->generateRivers(map))
    {
        Logger::log(LogLevel::Error, "While generating the rivers.");
        return false;
    }
    // Generate the forests.
    if (!this->generateForests(map))
    {
        Logger::log(LogLevel::Error, "While generating the forests.");
        return false;
    }
    // Reset the z coordinates.
    if (!this->resetZCoordinates(map))
    {
        Logger::log(LogLevel::Error, "While setting the z coordinates.");
        return false;
    }
    return true;
}

bool MapGenerator::initializeMap(const std::shared_ptr<MapWrapper> & map)
{
    // Set the dimension of the map.
    map->setWidth(configuration.width);
    map->setHeight(configuration.height);
    // Set the coordinates.
    for (int x = 0; x < map->getWidth(); ++x)
    {
        for (int y = 0; y < map->getHeight(); ++y)
        {
            // Get the cell.
            auto cell = map->getCell(x, y);
            // Set the coordinates.
            cell->coordinates = Coordinates(x, y, 0);
        }
    }
    // Connects the cells.
    for (int x = 0; x < map->getWidth(); ++x)
    {
        for (int y = 0; y < map->getHeight(); ++y)
        {
            // Get the cell.
            auto cell = map->getCell(x, y);
            if (x - 1 >= 0)
            {
                auto neighbour = map->getCell(x - 1, y);
                if (neighbour == nullptr)
                {
                    Logger::log(LogLevel::Error, "Found nullptr neighbour");
                    return false;
                }
                cell->addNeighbour(Direction::West, neighbour);
            }
            if (x + 1 < map->getWidth())
            {
                auto neighbour = map->getCell(x + 1, y);
                if (neighbour == nullptr)
                {
                    Logger::log(LogLevel::Error, "Found nullptr neighbour");
                    return false;
                }
                cell->addNeighbour(Direction::East, neighbour);
            }
            if (y - 1 >= 0)
            {
                auto neighbour = map->getCell(x, y - 1);
                if (neighbour == nullptr)
                {
                    Logger::log(LogLevel::Error, "Found nullptr neighbour");
                    return false;
                }
                cell->addNeighbour(Direction::South, neighbour);
            }
            if (y + 1 < map->getHeight())
            {
                auto neighbour = map->getCell(x, y + 1);
                if (neighbour == nullptr)
                {
                    Logger::log(LogLevel::Error, "Found nullptr neighbour");
                    return false;
                }
                cell->addNeighbour(Direction::North, neighbour);
            }
        }
    }
    return true;
}

bool MapGenerator::generateMountains(const std::shared_ptr<MapWrapper> & map)
{
    for (int i = 0; i < configuration.numMountains; ++i)
    {
        // Generate a random dimension for the mountain.
        auto radius = TRand<int>(configuration.minMountainRadius,
                                 configuration.maxMountainRadius);
        // Generate a random place for the mountain.
        auto xCenter = TRand<int>(-radius, map->getWidth() + radius);
        auto yCenter = TRand<int>(-radius, map->getHeight() + radius);
        // Determine the boundaries.
        auto xMin = std::max(xCenter - radius - 1, 0);
        auto xMax = std::min(xCenter + radius + 1, map->getWidth() - 1);
        auto yMin = std::max(yCenter - radius - 1, 0);
        auto yMax = std::min(yCenter + radius + 1, map->getHeight() - 1);
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
                    map->getCell(x, y)->coordinates.z +=
                        static_cast<int>(std::ceil(cellHeight));
                }
            }
        }
    }
    return true;
}

bool MapGenerator::normalizeMap(const std::shared_ptr<MapWrapper> & map)
{
    auto minHeight = 0, maxHeight = 0;
    // Find the minimum and maximum heights.
    for (auto x = 0; x < map->getWidth(); ++x)
    {
        for (auto y = 0; y < map->getHeight(); ++y)
        {
            auto cell = map->getCell(x, y);
            if (cell->coordinates.z < minHeight)
            {
                minHeight = cell->coordinates.z;
            }
            if (cell->coordinates.z > maxHeight)
            {
                maxHeight = cell->coordinates.z;
            }
        }
    }
    // Drop the map if it is quite plain.
    if (DoubleEquality(maxHeight, minHeight))
    {
        Logger::log(LogLevel::Error, "Min and max height are the same.");
        // Clear the map.
        map->destroy();
        return false;
    }
    // Normalize the heights to values between 0 and 10.
    for (int x = 0; x < map->getWidth(); ++x)
    {
        for (int y = 0; y < map->getHeight(); ++y)
        {
            auto cell = map->getCell(x, y);
            cell->coordinates.z = Normalize(cell->coordinates.z,
                                            minHeight,
                                            maxHeight,
                                            0,
                                            100);
        }
    }
    return true;
}

bool MapGenerator::applyTerrain(const std::shared_ptr<MapWrapper> & map)
{
    if (heightMap == nullptr)
    {
        Logger::log(LogLevel::Error, "HeightMap not set.");
        return false;
    }
    for (auto x = 0; x < map->getWidth(); ++x)
    {
        for (auto y = 0; y < map->getHeight(); ++y)
        {
            // Get the cell.
            auto cell = map->getCell(x, y);
            // Get the terrain associated with the cell height.
            auto terrain = heightMap->getTerrain(cell->coordinates.z);
            if (terrain == nullptr)
            {
                Logger::log(LogLevel::Error, "Applying a terrain.");
                return false;
            }
            cell->terrain = terrain;
            cell->liquidContent = terrain->liquidContent;
        }
    }
    return true;
}

bool MapGenerator::generateRivers(const std::shared_ptr<MapWrapper> & map)
{
    // List of possible starting points for a river.
    std::list<MapCell *> startingPoints;
    // Lamba used to check if a cell is far away from pre-existing starting
    // points and if it is a mountain.
    auto IsSuitable = [&](MapCell * cell)
    {
        if (cell->terrain->liquidSources.empty())
        {
            return false;
        }
        for (auto point : startingPoints)
        {
            auto distance = StructUtils::getDistance(cell->coordinates,
                                                          point->coordinates);
            if (distance <= configuration.minRiverDistance)
            {
                return false;
            }
        }
        return true;
    };
    // Retrieve all the starting points for rivers.
    for (auto x = 0; x < map->getWidth(); ++x)
    {
        for (auto y = 0; y < map->getHeight(); ++y)
        {
            auto cell = map->getCell(x, y);
            // Check if it is a suitable cell.
            if (IsSuitable(cell))
            {
                startingPoints.emplace_back(cell);
            }
        }
    }
    // Number of dropped rivers.
    auto iterations = std::min(static_cast<size_t >(configuration.numRivers),
                               startingPoints.size());
    for (unsigned int it = 0; it < iterations; ++it)
    {
        // Prepare a vector for the river.
        std::vector<MapCell *> river;
        // Set the starting cell.
        MapCell * cell = startingPoints.front();
        startingPoints.pop_front();
        // Pick from the available liquids of the starting cell.
        auto liquid = cell->terrain->getRandomLiquidSource();
        if (liquid == nullptr)
        {
            Logger::log(LogLevel::Error, "No liquid source.");
            return false;
        }
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
            // If a liquid is reached stop.
            if (nextCell->liquidContent.first != nullptr)
            {
                break;
            }
            // Add the next cell to the path of the river.
            river.push_back(nextCell);
            // Set the current cell to the next cell.
            cell = nextCell;
        }
        // Drop the river on the map.
        for (auto it2 = river.begin(); it2 != river.end(); ++it2)
        {
            auto riverCell = (*it2);
            if (it2 == river.begin())
            {
                riverCell->liquidContent = std::make_pair(liquid, 20);
                continue;
            }
            riverCell->liquidContent = std::make_pair(liquid, 5);
        }
    }
    return true;
}

bool MapGenerator::generateForests(const std::shared_ptr<MapWrapper> & map)
{
    // List of locations for forests.
    std::list<MapCell *> forestDropPoints;
    // Lamba used to check if a cell is far away from pre-existing drop points
    // and if it is nearby a source of water.
    auto IsSuitable = [&](MapCell * cell)
    {
        // Check if the cell can host a forest.
        if (!HasFlag(cell->terrain->generationFlags,
                     TerrainGenerationFlags::CanHostForest))
        {
            return false;
        }
        // Check the distance from another forest drop point.
        for (auto point : forestDropPoints)
        {
            auto distance = StructUtils::getDistance(cell->coordinates,
                                                          point->coordinates);
            if (distance <= configuration.minForestDistance)
            {
                return false;
            }
        }
        return true;
    };
    // Retrieve all the starting points for rivers.
    for (auto x = 0; x < map->getWidth(); ++x)
    {
        for (auto y = 0; y < map->getHeight(); ++y)
        {
            auto cell = map->getCell(x, y);
            // Check if it is a suitable cell.
            if (IsSuitable(cell))
            {
                forestDropPoints.emplace_back(cell);
            }
        }
    }
    std::function<bool(const int &, const int &, const int &, int)> FloodFill;
    FloodFill = [&](const int & x,
                    const int & y,
                    const int & iterationTotal,
                    int iterationLeft)
    {
        auto cell = map->getCell(x, y);
        if (iterationLeft == 0) return true;
        if (HasFlag(cell->flags, RoomFlags::SpawnTree)) return true;
        // Check if the cell can host a forest.
        if (!HasFlag(cell->terrain->generationFlags,
                     TerrainGenerationFlags::CanHostForest))
        {
            return true;
        }
        auto normalized = Normalize(iterationLeft, 0, iterationTotal, 0,
                                    100);
        if (TRand<int>(0, 100) >= normalized) return true;
        SetFlag(cell->flags, RoomFlags::SpawnTree);
        iterationLeft--;
        if (!FloodFill(std::max(x - 1, 0), y, iterationTotal, iterationLeft))
        {
            return false;
        }
        if (!FloodFill(std::min(x + 1, map->getWidth() - 1), y,
                       iterationTotal,
                       iterationLeft))
        {
            return false;
        }
        if (!FloodFill(x, std::max(y - 1, 0), iterationTotal, iterationLeft))
        {
            return false;
        }
        return FloodFill(x, std::min(y + 1, map->getHeight() - 1),
                         iterationTotal,
                         iterationLeft);
    };
    auto maxForestExpansion = TRand(3,
                                    configuration.minForestDistance - 1);
    // Number of dropped rivers.
    auto iterations = std::min(static_cast<size_t>(configuration.numForests),
                               forestDropPoints.size());
    for (unsigned int it = 0; it < iterations; ++it)
    {
        // Pick a random forest drop point.
        auto dpIt = forestDropPoints.begin();
        std::advance(dpIt,
                     TRand<size_t>(0, forestDropPoints.size() - 1));
        MapCell * cell = (*dpIt);
        forestDropPoints.erase(dpIt);
        // Create the forest.
        if (!FloodFill(cell->coordinates.x,
                       cell->coordinates.y,
                       maxForestExpansion,
                       maxForestExpansion))
        {
            Logger::log(LogLevel::Error, "Found nullptr room.");
            return false;
        }
    }
    return true;
}

bool MapGenerator::resetZCoordinates(const std::shared_ptr<MapWrapper> & map)
{
    for (auto x = 0; x < map->getWidth(); ++x)
    {
        for (auto y = 0; y < map->getHeight(); ++y)
        {
            auto cell = map->getCell(x, y);
            cell->coordinates.z = 50;
        }
    }
    return true;
}
