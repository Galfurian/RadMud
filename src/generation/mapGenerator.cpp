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
#include "utils.hpp"

MapGenerator::MapGenerator(int _width,
                           int _height,
                           int _numMountains,
                           int _minMountainRadius,
                           int _maxMountainRadius) :
    width(_width),
    height(_height),
    numMountains(_numMountains),
    minMountainRadius(_minMountainRadius),
    maxMountainRadius(_maxMountainRadius),
    map(_width, _height)
{
    // Nothing to do.
}

void MapGenerator::generateMap()
{
    // First clear the map.
    this->clearMap();
    // Drop the mountains.
    for (int i = 0; i < numMountains; ++i)
    {
        this->dropMountain();
    }
    // Normalize the map in order to have values between 0 and 1.
    this->normalizeMap();
}

void MapGenerator::dropMountain()
{
    // Generate a random dimension for the mountain.
    auto radius = TRandInteger<int>(minMountainRadius, maxMountainRadius);
    // Generate a random place for the mountain.
    auto xCenter = TRandInteger<int>(-radius, width + radius);
    auto yCenter = TRandInteger<int>(-radius, height + radius);
    // Determine the boundaries.
    auto xMin = std::max(xCenter - radius - 1, 0);
    auto xMax = std::min(xCenter + radius + 1, width - 1);
    auto yMin = std::max(yCenter - radius - 1, 0);
    auto yMax = std::min(yCenter + radius + 1, height - 1);
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
                map.set(x, y, map.get(x, y) + cellHeight);
            }
        }
    }
}

void MapGenerator::normalizeMap()
{
    auto minHeight = 0.0, maxHeight = 0.0;
    // Find the minimum and maximum heights.
    for (auto x = 0; x < width; ++x)
    {
        for (auto y = 0; y < height; ++y)
        {
            auto cellHeight = map.get(x, y);
            if (cellHeight < minHeight) minHeight = cellHeight;
            if (cellHeight > maxHeight) maxHeight = cellHeight;
        }
    }
    // If the map is quite plain, clear it.
    if (!DoubleEquality(maxHeight, minHeight))
    {
        // Normalize the heights to values between 0 and 1.
        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                map.set(x, y,
                        (map.get(x, y) - minHeight) / (maxHeight - minHeight));
            }
        }
    }
}

void MapGenerator::clearMap()
{
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            map.set(x, y, 0.0);
        }
    }
}