/// @file   heightMap.cpp
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

#include "heightMap.hpp"
#include "formatter.hpp"

HeightMap::HeightMap(const unsigned int & _height) :
    height()
{
    if (_height == 0) height = Void;
    else if (_height == 1) height = DeepWater;
    else if (_height == 2) height = ShallowWater;
    else if (_height == 3) height = Coast;
    else if (_height == 4) height = Plain;
    else if (_height == 5) height = Hill;
    else if (_height == 6) height = Mountain;
    else if (_height == 7) height = HighMountain;
    else height = Void;
}

HeightMap::HeightMap(const Enum & _height) :
    height(_height)
{
    // Nothing to do.
}

bool HeightMap::isValid(const unsigned int & _height)
{
    return (_height <= 7);
}

std::string HeightMap::toString() const
{
    if (height == HeightMap::Void) return "Void";
    if (height == HeightMap::DeepWater) return "DeepWater";
    if (height == HeightMap::ShallowWater) return "ShallowWater";
    if (height == HeightMap::Coast) return "Coast";
    if (height == HeightMap::Plain) return "Plain";
    if (height == HeightMap::Hill) return "Hill";
    if (height == HeightMap::Mountain) return "Mountain";
    else return "HighMountain";
}

unsigned int HeightMap::toUInt() const
{
    return static_cast<unsigned int>(height);
}

std::string HeightMap::toSymbol(bool color) const
{
    if (height == HeightMap::Void)
    {
        return ((color) ? Formatter::reset() : "") + " ";
    }
    if (height == HeightMap::DeepWater)
    {
        return ((color) ? Formatter::blue() : "") + "W";
    }
    if (height == HeightMap::ShallowWater)
    {
        return ((color) ? Formatter::cyan() : "") + "w";
    }
    if (height == HeightMap::Coast)
    {
        return ((color) ? Formatter::yellow() : "") + "c";
    }
    if (height == HeightMap::Plain)
    {
        return ((color) ? Formatter::reset() : "") + ".";
    }
    if (height == HeightMap::Hill)
    {
        return ((color) ? Formatter::brown() : "") + "h";
    }
    if (height == HeightMap::Mountain)
    {
        return ((color) ? Formatter::gray() : "") + "m";
    }
    else
    {
        return ((color) ? Formatter::darkGray() : "") + "M";
    }
}

bool HeightMap::operator==(const HeightMap::Enum & rhs) const
{
    return height == rhs;
}

bool HeightMap::operator!=(const HeightMap::Enum & rhs) const
{
    return height != rhs;
}
