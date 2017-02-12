/// @file   terrain.cpp
/// @brief  Implementation of Terrain class functions.
/// @author Enrico Fraccaroli
/// @date   08/12/2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "terrain.hpp"
#include "utils.hpp"

Terrain::Terrain() :
    vnum(),
    name(),
    flags(),
    generationFlags(),
    space(),
    symbol(),
    liquidContent(),
    liquidSources()
{
    // Nothing to do.
}

void Terrain::addLiquidSource(Liquid * _liquid,
                              const unsigned int & _assignedProbability)
{
    LiquidSource liquidSource;
    liquidSource.liquid = _liquid;
    liquidSource.assignedProbability = _assignedProbability;
    if (liquidSources.empty())
    {
        liquidSource.cumulativeProbability = _assignedProbability;
    }
    else
    {
        liquidSource.cumulativeProbability =
            _assignedProbability + liquidSources.back().cumulativeProbability;
    }
    liquidSources.emplace_back(liquidSource);
}

Liquid * Terrain::getRandomLiquidSource() const
{
    if (liquidSources.empty())
    {
        return nullptr;
    }
    auto pickedValue = TRandInteger<unsigned int>(
        0, liquidSources.back().cumulativeProbability - 1);
    for (auto liquidSource : liquidSources)
    {
        if (pickedValue <= liquidSource.cumulativeProbability)
        {
            return liquidSource.liquid;
        }
    }
    return nullptr;
}
