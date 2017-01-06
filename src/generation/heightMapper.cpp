/// @file   heightMapper.cpp
/// @author Enrico Fraccaroli
/// @date   gen 05 2017
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
#include "formatter.hpp"
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

std::string HeightMapper::getTypeByElevation(double elevation)
{
    // Search for the tile.
    for (auto it : thresholds)
    {
        if (elevation < it.threshold)
        {
            return it.tile;
        }
    }
    // Apply the last threshold.
    if (!thresholds.empty())
    {
        return thresholds.back().tile;
    }
    return Formatter::reset() + " ";
}

void HeightMapper::setNormalThresholds()
{
    thresholds.clear();
    this->addThreshold(Threshold(0.15, Formatter::blue() + "W"));
    this->addThreshold(Threshold(0.20, Formatter::cyan() + "w"));
    this->addThreshold(Threshold(0.25, Formatter::yellow() + "c"));
    this->addThreshold(Threshold(0.50, Formatter::reset() + "."));
    this->addThreshold(Threshold(0.85, Formatter::gray() + "m"));
    this->addThreshold(Threshold(0.95, Formatter::gray() +
                                       Formatter::bold() + "M"));
    this->addThreshold(Threshold(1.00, Formatter::reset() +
                                       Formatter::bold() + "P"));
}

void HeightMapper::setIslandsThresholds()
{
    thresholds.clear();
    this->addThreshold(Threshold(0.40, Formatter::blue() + "W"));
    this->addThreshold(Threshold(0.55, Formatter::cyan() + "w"));
    this->addThreshold(Threshold(0.60, Formatter::yellow() + "c"));
    this->addThreshold(Threshold(0.80, Formatter::reset() + "."));
    this->addThreshold(Threshold(0.85, Formatter::gray() + "m"));
    this->addThreshold(Threshold(0.95, Formatter::gray() +
                                       Formatter::bold() + "M"));
    this->addThreshold(Threshold(1.00, Formatter::reset() +
                                       Formatter::bold() + "P"));
}