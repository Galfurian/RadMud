/// @file   mapGeneratorConfiguration.cpp
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

#include "mapGeneratorConfiguration.hpp"
#include "utils.hpp"

MapGeneratorConfiguration::MapGeneratorConfiguration() :
    width(60),
    height(30),
    numMountains(50),
    minMountainRadius(5),
    maxMountainRadius(15),
    numRivers(2),
    minRiverDistance(8),
    numForests(2),
    minForestDistance(8)
{
    // Nothing to do.
}

std::string MapGeneratorConfiguration::toString() const
{
    std::string output;
    output += "Width             :" + ToString(width) + ";\n";
    output += "Height            :" + ToString(height) + ";\n";
    output += "NumMountains      :" + ToString(numMountains) + ";\n";
    output += "MinMountainRadius :" + ToString(minMountainRadius) + ";\n";
    output += "MaxMountainRadius :" + ToString(maxMountainRadius) + ";\n";
    output += "NumRivers         :" + ToString(numRivers) + ";\n";
    output += "MinRiverDistance  :" + ToString(minRiverDistance) + ";\n";
    output += "NumForests        :" + ToString(numForests) + ";\n";
    output += "MinForestDistance :" + ToString(minForestDistance) + ";\n";
    return output;
}