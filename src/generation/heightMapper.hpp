/// @file   heightMapper.hpp
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

#pragma once

#include <string>
#include <vector>

class Threshold
{
public:
    /// The threshold value.
    double threshold;
    /// The associated tile.
    std::string tile;

    /// @brief Constructor.
    Threshold() :
        threshold(),
        tile()
    {
        // Nothing to do.
    }

    /// @brief Constructor.
    Threshold(double _threshold, std::string _tile) :
        threshold(_threshold),
        tile(_tile)
    {
        // Nothing to do.
    }
};

/// @brief Class which maps the values of an height-map to types of terrain.
class HeightMapper
{
private:
    std::vector<Threshold> thresholds;

public:
    HeightMapper();

    void addThreshold(const Threshold & threshold);

    std::string getTypeByElevation(double elevation);

    void setNormalThresholds();

    void setIslandsThresholds();
};