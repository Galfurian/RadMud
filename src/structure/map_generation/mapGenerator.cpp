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

MapGenerator::MapGenerator(const MapGeneratorConfiguration & _configuration,
                           const std::shared_ptr<HeightMap> & _heightMap) :
    configuration(_configuration),
    heightMap(_heightMap)
{
    // Nothing to do.
}

bool MapGenerator::generateMap(Map2D<MapCell> & map)
{
    // Generate the structure.
    if (!this->generateStructure(map))
    {
        Logger::log(LogLevel::Error, "While generating the structure.");
        return false;
    }
    // Drop the rivers.
    if (!this->generateRivers(map))
    {
        Logger::log(LogLevel::Error, "While generating the rivers.");
        return false;
    }
//    // Add the forests.
//    this->addForests(map);
    return true;
}

bool MapGenerator::generateStructure(Map2D<MapCell> & map)
{
    // Set the dimension of the map.
    map.setWidth(configuration.width);
    map.setHeight(configuration.height);
    // Initialize the map cells.
    for (auto x = 0; x < map.getWidth(); ++x)
    {
        for (auto y = 0; y < map.getHeight(); ++y)
        {
            map.get(x, y).coordinates = Coordinates(x, y, 0);
        }
    }
    for (auto x = 0; x < map.getWidth(); ++x)
    {
        for (auto y = 0; y < map.getHeight(); ++y)
        {
            map.get(x, y).addNeighbours(map);
        }
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
    // Create the rooms for the map.
    if (!this->generateRooms(map))
    {
        Logger::log(LogLevel::Error, "While generating the rooms for the map.");
        return false;
    }
    return true;
}

bool MapGenerator::generateMountains(Map2D<MapCell> & map)
{
    for (int i = 0; i < configuration.numMountains; ++i)
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
                auto cell = &map.get(x, y);
                // Determine the distance between the cell and the center.
                auto distance = pow(xCenter - x, 2) + pow(yCenter - y, 2);
                // Determine the height of the cell based on the distance.
                auto cellHeight = squareRadius - distance;
                // Set the height value on the map.
                if (cellHeight > 0)
                {
                    cell->height += cellHeight;
                }
            }
        }
    }
    return true;
}

bool MapGenerator::normalizeMap(Map2D<MapCell> & map)
{
    auto minHeight = 0.0, maxHeight = 0.0;
    // Find the minimum and maximum heights.
    for (auto x = 0; x < map.getWidth(); ++x)
    {
        for (auto y = 0; y < map.getHeight(); ++y)
        {
            auto cell = &map.get(x, y);
            if (cell->height < minHeight) minHeight = cell->height;
            if (cell->height > maxHeight) maxHeight = cell->height;
        }
    }
    // Drop the map if it is quite plain.
    if (DoubleEquality(maxHeight, minHeight))
    {
        Logger::log(LogLevel::Error, "Min and max height are the same.");
        // Clear the map.
        this->clearMap(map);
        return false;
    }
    // Normalize the heights to values between 0 and 10.
    for (int x = 0; x < map.getWidth(); ++x)
    {
        for (int y = 0; y < map.getHeight(); ++y)
        {
            auto cell = &map.get(x, y);
            cell->height = this->normalize(cell->height,
                                           minHeight,
                                           maxHeight,
                                           0.0,
                                           100.0);
        }
    }
    return true;
}

bool MapGenerator::applyTerrain(Map2D<MapCell> & map)
{
    for (auto x = 0; x < map.getWidth(); ++x)
    {
        for (auto y = 0; y < map.getHeight(); ++y)
        {
            map.get(x, y).terrain = heightMap->getTerrain(map.get(x, y).height);
            if (map.get(x, y).terrain == nullptr)
            {
                Logger::log(LogLevel::Error, "Applying a terrain.");
                return false;
            }
        }
    }
    return true;
}

bool MapGenerator::generateRooms(Map2D<MapCell> & map)
{
    for (auto x = 0; x < map.getWidth(); ++x)
    {
        for (auto y = 0; y < map.getHeight(); ++y)
        {
            auto room = new Room();
            //room->vnum = ;
            //room->area = ;
            room->coord = map.get(x, y).coordinates;
            room->coord.z = static_cast<int>(map.get(x, y).height);
            room->terrain = map.get(x, y).terrain;
            room->name = map.get(x, y).terrain->name;
            if (map.get(x, y).terrain->liquidContent != nullptr)
            {
                room->liquid = std::make_pair(
                    map.get(x, y).terrain->liquidContent, 100);
            }
            //room->description = ;
            room->flags = 0;
            map.get(x, y).room = room;
        }
    }
    return true;
}

bool MapGenerator::generateRivers(Map2D<MapCell> & map)
{
    // List of possible starting points for a river.
    std::list<MapCell *> startingPoints;
    // Lamba used to check if a cell is far away from pre-existing starting
    // points and if it is a mountain.
    auto IsSuitable = [&](MapCell * cell)
    {
        if(cell->terrain->liquidSources.empty())
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
            if (nextCell->room->liquid.first != nullptr)
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
                riverCell->room->coord.z -= 2;
                riverCell->room->liquid = std::make_pair(liquid, 100);
                continue;
            }
            riverCell->room->coord.z -= 2;
            riverCell->room->liquid = std::make_pair(liquid, 100);
        }
    }
    return true;
}

void MapGenerator::addForests(Map2D<MapCell> & map)
{
    (void) map;
//    // List of locations for forests.
//    std::list<MapCell *> forestDropPoints;
//    // Lamba used to check if a cell is far away from pre-existing drop points
//    // and if it is nearby a source of water.
//    auto IsSuitable = [&](MapCell * cell)
//    {
//        // Check if the cell is a Plain|Hill|Mountain.
//        if ((cell->mapTile != MapTile::Plain) &&
//            (cell->mapTile != MapTile::Hill) &&
//            (cell->mapTile != MapTile::Mountain))
//        {
//            return false;
//        }
//        // Check the distance from another forest drop point.
//        for (auto point : forestDropPoints)
//        {
//            auto distance = Area::getDistance(cell->coordinates,
//                                              point->coordinates);
//            if (distance <= configuration.minForestDistance)
//            {
//                return false;
//            }
//        }
//        return true;
//    };
//    // Retrieve all the starting points for rivers.
//    for (auto x = 0; x < map.getWidth(); ++x)
//    {
//        for (auto y = 0; y < map.getHeight(); ++y)
//        {
//            auto cell = &map.get(x, y);
//            // Check if it is a suitable cell.
//            if (IsSuitable(cell))
//            {
//                forestDropPoints.emplace_back(cell);
//            }
//        }
//    }
//    std::function<void(int, int, int &)> FloodFill;
//    FloodFill = [&](int x, int y, int iterationLeft)
//    {
//        auto cell = &map.get(x, y);
//        if ((cell->mapTile == MapTile::Forest) || (iterationLeft == 0))
//        {
//            return;
//        }
//        if (cell->mapTile == MapTile::Plain)
//        {
//            if (TRandInteger<int>(0, 100) < 20) return;
//        }
//        else if (cell->mapTile == MapTile::Hill)
//        {
//            if (TRandInteger<int>(0, 100) < 40) return;
//        }
//        else if (cell->mapTile == MapTile::Mountain)
//        {
//            if (TRandInteger<int>(0, 100) < 65) return;
//        }
//        else
//        {
//            return;
//        }
//        cell->mapTile = MapTile::Forest;
//        iterationLeft--;
//        FloodFill(std::max(x - 1, 0), y, iterationLeft);
//        FloodFill(std::min(x + 1, map.getWidth() - 1), y, iterationLeft);
//        FloodFill(x, std::max(y - 1, 0), iterationLeft);
//        FloodFill(x, std::min(y + 1, map.getHeight() - 1), iterationLeft);
//    };
//    auto maxForestExpansion = TRandInteger(3,
//                                           configuration.minForestDistance - 1);
//    // Number of dropped rivers.
//    auto iterations = std::min(static_cast<size_t>(configuration.numForests),
//                               forestDropPoints.size());
//    for (unsigned int it = 0; it < iterations; ++it)
//    {
//        // Pick a random forest drop point.
//        auto dpIt = forestDropPoints.begin();
//        std::advance(dpIt,
//                     TRandInteger<size_t>(0, forestDropPoints.size() - 1));
//        MapCell * cell = (*dpIt);
//        forestDropPoints.erase(dpIt);
//        // Create the forest.
//        FloodFill(cell->coordinates.x,
//                  cell->coordinates.y,
//                  maxForestExpansion);
//    }
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

double MapGenerator::normalize(double value, double LbFrom, double UbFrom,
                               double LbTo, double UbTo) const
{
    return (((UbTo - LbTo) * (value - LbFrom)) / ((UbFrom - LbFrom))) + LbTo;
}
