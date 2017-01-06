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
#include "pathFinder.hpp"
#include "utils.hpp"

MapGenerator::MapGenerator()
{
    // Nothing to do.
}

Map2D<MapCell> MapGenerator::generateMap(int width,
                                         int height,
                                         int numMountains,
                                         int minMountainRadius,
                                         int maxMountainRadius,
                                         int numRivers)
{
    // Prepare the map.
    Map2D<MapCell> map(width, height, MapCell());
    // Initialize the map cells.
    for (auto x = 0; x < width; ++x)
    {
        for (auto y = 0; y < height; ++y)
        {
            map.get(x, y).coordinates = Coordinates(x, y, 0);
        }
    }
    // Drop the mountains.
    for (int i = 0; i < numMountains; ++i)
    {
        this->dropMountain(map, minMountainRadius, maxMountainRadius);
    }
    // Normalize the map in order to have values between 0 and 1.
    this->normalizeMap(map);
    // Drop the rivers.
    for (int i = 0; i < numRivers; ++i)
    {
        this->dropRiver(map);
    }
    return map;
}

void MapGenerator::dropMountain(Map2D<MapCell> & map,
                                int minMountainRadius,
                                int maxMountainRadius)
{
    // Generate a random dimension for the mountain.
    auto radius = TRandInteger<int>(minMountainRadius, maxMountainRadius);
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
        // Normalize the heights to values between 0 and 1.
        for (int x = 0; x < map.getWidth(); ++x)
        {
            for (int y = 0; y < map.getHeight(); ++y)
            {
                map.get(x, y).height =
                    (map.get(x, y).height - minHeight) /
                    (maxHeight - minHeight);
            }
        }
    }
}

void MapGenerator::dropRiver(Map2D<MapCell> & map)
{
    int xSource = 0, ySource = 0;
    double heightSource = 0.0;
    auto SearchNextCell = [&](int currentX,
                              int currentY,
                              double currentHeight)
    {
        int nextX = currentX, nextY = currentY;
        double nextHeight = currentHeight;
        if (currentX - 1 > 0) // West
        {
            auto h = map.get(currentX - 1, currentY).height;
            if (h < nextHeight)
            {
                nextX = currentX - 1;
                nextY = currentY;
                nextHeight = h;
            }
        }
        if (currentX + 1 < map.getWidth()) // East
        {
            auto h = map.get(currentX + 1, currentY).height;
            if (h < nextHeight)
            {
                nextX = currentX + 1;
                nextY = currentY;
                nextHeight = h;
            }
        }
        if (currentY - 1 > 0) // South
        {
            auto h = map.get(currentX, currentY - 1).height;
            if (h < nextHeight)
            {
                nextX = currentX;
                nextY = currentY - 1;
                nextHeight = h;
            }
        }
        if (currentY + 1 < map.getHeight()) // North
        {
            auto h = map.get(currentX, currentY + 1).height;
            if (h < nextHeight)
            {
                nextX = currentX;
                nextY = currentY + 1;
            }
        }
        return std::make_pair(nextX, nextY);
    };
    // Find the minimum and maximum heights.
    for (auto x = 0; x < map.getWidth(); ++x)
    {
        for (auto y = 0; y < map.getHeight(); ++y)
        {
            auto cellHeight = map.get(x, y).height;
            if (cellHeight > heightSource)
            {
                xSource = x;
                ySource = y;
                heightSource = cellHeight;
            }
        }
    }
    std::vector<std::pair<int, int> > riverPath;
    int currentX = xSource, currentY = ySource;
    while (true)
    {
        auto nextCell = SearchNextCell(currentX,
                                       currentY,
                                       map.get(currentX, currentY).height);
        riverPath.push_back(nextCell);
        if ((nextCell.first == currentX) && (nextCell.second == currentY))
        {
            break;
        }
        currentX = nextCell.first;
        currentY = nextCell.second;
    }
    for (auto nextCell :riverPath)
    {
        map.get(nextCell.first, nextCell.second).height = -10;
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