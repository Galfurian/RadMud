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

HeightMapper::HeightMapper()
{
    // Nothing to do.
}

void HeightMapper::addThreshold(const Threshold & threshold)
{
    thresholds.emplace_back(threshold);
    auto SortThreshold = [](const Threshold & left, const Threshold & right)
    {
        return left.threshold < right.threshold;
    };
    std::sort(thresholds.begin(), thresholds.end(), SortThreshold);
}

void HeightMapper::setNormalThresholds()
{
    thresholds.clear();
    this->addThreshold(Threshold(15, HeightMap::DeepWater));
    this->addThreshold(Threshold(20, HeightMap::ShallowWater));
    this->addThreshold(Threshold(25, HeightMap::Coast));
    this->addThreshold(Threshold(50, HeightMap::Plain));
    this->addThreshold(Threshold(85, HeightMap::Hill));
    this->addThreshold(Threshold(95, HeightMap::Mountain));
    this->addThreshold(Threshold(100, HeightMap::HighMountain));
}

void HeightMapper::setIslandThresholds()
{
    thresholds.clear();
    this->addThreshold(Threshold(40, HeightMap::DeepWater));
    this->addThreshold(Threshold(55, HeightMap::ShallowWater));
    this->addThreshold(Threshold(60, HeightMap::Coast));
    this->addThreshold(Threshold(80, HeightMap::Plain));
    this->addThreshold(Threshold(85, HeightMap::Hill));
    this->addThreshold(Threshold(95, HeightMap::Mountain));
    this->addThreshold(Threshold(100, HeightMap::HighMountain));
}

void HeightMapper::applyHeightMap(Map2D<MapCell> & map)
{
    if (thresholds.empty())
    {
        this->setNormalThresholds();
    }
    for (auto x = 0; x < map.getWidth(); ++x)
    {
        for (auto y = 0; y < map.getHeight(); ++y)
        {
            for (auto it : thresholds)
            {
                if (map.get(x, y).height <= it.threshold)
                {
                    map.get(x, y).heightMap = it.heightMap;
                    break;
                }
            }
        }
    }
}
