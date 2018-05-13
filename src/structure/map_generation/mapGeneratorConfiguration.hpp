/// @file   mapGeneratorConfiguration.hpp
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

#include <string>
#include <cassert>

/// @brief Configuration class for the map generator.
class MapGeneratorConfiguration
{
public:
    /// The width of the map.
    int width;
    /// The height of the map.
    int height;
    /// The elevation of the map.
    int elevation;

    /// The number of mountains that has to be created.
    int numMountains;
    /// The minimum radius of each mountain.
    int minMountainRadius;
    /// The maximum radius of each mountain.
    int maxMountainRadius;

    /// The number of rivers that has to be created.
    int numRivers;
    /// The minimum distance between rivers.
    int minRiverDistance;

    /// The number of forests that has to be created.
    int numForests;
    /// The minimum distance between forests.
    int minForestDistance;

    /// The maximum number of caves.
    int maxCaves;
    /// The minimum size of a cave.
    int minCaveSize;
    /// The maximum size of a cave.
    int maxCaveSize;

    /// @brief Constructor.
    MapGeneratorConfiguration(int _width = 40,
                              int _height = 20,
                              int _elevation = 20) :
        width(_width),
        height(_height),
        elevation(_elevation),
        numMountains((_width + _height) / 2),
        minMountainRadius((_width + _height) / 8),
        maxMountainRadius((_width + _height) / 4),
        numRivers((_width + _height) / 10),
        minRiverDistance((_width + _height) / 10),
        numForests((_width + _height) / 8),
        minForestDistance((_width + _height) / 8),
        maxCaves(4),
        minCaveSize(4),
        maxCaveSize(8)
    {
        assert(minMountainRadius < maxMountainRadius);
        assert(minCaveSize < maxCaveSize);
    }

    /// @brief Returns the string representing the values.
    std::string toString() const
    {
        std::string out;
        out += "Width             :" + std::to_string(width) + ";\n";
        out += "Height            :" + std::to_string(height) + ";\n";
        out += "NumMountains      :" + std::to_string(numMountains) + ";\n";
        out +=
            "MinMountainRadius :" + std::to_string(minMountainRadius) + ";\n";
        out +=
            "MaxMountainRadius :" + std::to_string(maxMountainRadius) + ";\n";
        out += "NumRivers         :" + std::to_string(numRivers) + ";\n";
        out += "MinRiverDistance  :" + std::to_string(minRiverDistance) + ";\n";
        out += "NumForests        :" + std::to_string(numForests) + ";\n";
        out +=
            "MinForestDistance :" + std::to_string(minForestDistance) + ";\n";
        return out;
    }
};