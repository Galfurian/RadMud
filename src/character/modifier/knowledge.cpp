/// @file   knowledge.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 13 2017
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

#include "knowledge.hpp"

Knowledge::Knowledge() :
    value(None)
{
    // Nothing to do.
}

Knowledge::Knowledge(const unsigned int & _value) :
    value()
{
    if (_value == 1) value = GatherWood;
    else if (_value == 2) value = GatherStone;
    else if (_value == 3) value = GatherErbs;
    else if (_value == 4) value = GatherPlant;
    else if (_value == 5) value = ButcherAnimal;
    else if (_value == 6) value = SkinAnimal;
    else if (_value == 7) value = TanHide;
    else value = None;
}

Knowledge::Knowledge(const Enum & _value) :
    value(_value)
{
    // Nothing to do.
}

bool Knowledge::isValid(const unsigned int & _value)
{
    return (_value >= 1) && (_value <= 7);
}

std::string Knowledge::toString() const
{
    if (value == Knowledge::GatherWood)
        return "GatherWood";
    if (value == Knowledge::GatherStone)
        return "GatherStone";
    if (value == Knowledge::GatherErbs)
        return "GatherErbs";
    if (value == Knowledge::GatherPlant)
        return "GatherPlant";
    if (value == Knowledge::ButcherAnimal)
        return "ButcherAnimal";
    if (value == Knowledge::SkinAnimal)
        return "SkinAnimal";
    if (value == Knowledge::TanHide)
        return "TanHide";
    else return "None";
}

unsigned int Knowledge::toUInt() const
{
    return static_cast<unsigned int>(value);
}
