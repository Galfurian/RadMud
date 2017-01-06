/// @file   heightMapper.cpp
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

#include "heightMapper.hpp"
#include <algorithm>

HeightMapper::HeightMapper() :
    thresholdMap()
{
    this->reset();
}

void HeightMapper::reset()
{
    thresholdMap[HeightMap::DeepWater] = 15;
    thresholdMap[HeightMap::ShallowWater] = 20;
    thresholdMap[HeightMap::Coast] = 25;
    thresholdMap[HeightMap::Plain] = 50;
    thresholdMap[HeightMap::Hill] = 85;
    thresholdMap[HeightMap::Mountain] = 95;
    thresholdMap[HeightMap::HighMountain] = 100;
}

HeightMap HeightMapper::getHeightMap(const double & height)
{
    for (auto it : thresholdMap)
    {
        if (height <= it.second)
        {
            return it.first;
        }
    }
    return HeightMap::Void;
}

double HeightMapper::getThreshold(const HeightMap & heightMap)
{
    for (auto it : thresholds)
    {
        if (it.heightMap == heightMap)
        {
            return it.threshold;
        }
    }
    return thresholdMap[HeightMap::HighMountain];
}

void HeightMapper::applyHeightMap(Map2D<MapCell> & map)
{
    for (auto x = 0; x < map.getWidth(); ++x)
    {
        for (auto y = 0; y < map.getHeight(); ++y)
        {
            map.get(x, y).heightMap = this->getHeightMap(map.get(x, y).height);
        }
    }
}
