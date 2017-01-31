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

HeightMapper::HeightMapper(const unsigned int & _vnum,
                           const std::string & _name) :
    vnum(_vnum),
    name(_name),
    thresholdMap()
{
    this->reset();
}

void HeightMapper::reset()
{
    thresholdMap[MapTile::DeepWater] = 10;
    thresholdMap[MapTile::ShallowWater] = 20;
    thresholdMap[MapTile::Coast] = 25;
    thresholdMap[MapTile::Plain] = 70;
    thresholdMap[MapTile::Hill] = 85;
    thresholdMap[MapTile::Mountain] = 95;
    thresholdMap[MapTile::HighMountain] = 100;
}


void HeightMapper::addThreshold(
    const std::shared_ptr<terrain::Terrain> & terrain,
    const double & threshold)
{
    (void) terrain;
    (void) threshold;
}

MapTile HeightMapper::getHeightMap(const double & height)
{
    for (auto it : thresholdMap)
    {
        if (height <= it.second)
        {
            return it.first;
        }
    }
    return MapTile::Void;
}
