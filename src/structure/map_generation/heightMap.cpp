/// @file   heightMap.cpp
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

#include "heightMap.hpp"
#include "terrain.hpp"

#include <algorithm>

HeightMap::HeightMap(const unsigned int & _vnum,
                     const std::string & _name,
                     const std::shared_ptr<Terrain> & _seaLevelTerrain) :
    vnum(_vnum),
    name(_name),
    seaLevelTerrain(_seaLevelTerrain),
    thresholds()
{
    // Nothing to do.
}

void HeightMap::addThreshold(
    const std::shared_ptr<Terrain> & terrain,
    const int & threshold)
{
    thresholds.emplace_back(std::make_pair(threshold, terrain));
    std::sort(thresholds.begin(), thresholds.end(), [](
        const std::pair<int, std::shared_ptr<Terrain>> & left,
        const std::pair<int, std::shared_ptr<Terrain>> & right)
    {
        return left.first < right.first;
    });
}

std::shared_ptr<Terrain> HeightMap::getTerrain(
    const int & height)
{
    for (auto it : thresholds)
    {
        if (height <= it.first)
        {
            return it.second;
        }
    }
    return nullptr;
}

int HeightMap::getOffset(
    const std::shared_ptr<Terrain> & terrain) const
{
    int terrainLevel = 0;
    for (auto it : thresholds)
    {
        if (it.second->vnum == terrain->vnum)
        {
            break;
        }
        ++terrainLevel;
    }
    int seaLevel = 0;
    for (auto it : thresholds)
    {
        if (it.second->vnum == seaLevelTerrain->vnum)
        {
            break;
        }
        ++seaLevel;
    }
    return terrainLevel - seaLevel;
}

int HeightMap::getSeaLevelHeight() const
{
    for (auto const & it : thresholds)
    {
        if (it.second->vnum == seaLevelTerrain->vnum)
        {
            return it.first;
        }
    }
    return -1;
}
