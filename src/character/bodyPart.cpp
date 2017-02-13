/// @file   bodyPart.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 08 2017
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

#include "bodyPart.hpp"
#include "resourceModel.hpp"
#include "formatter.hpp"
#include <cassert>

BodyPart::BodyPart() :
    vnum(),
    name(),
    description(),
    flags(),
    resources()
{
    // Nothing to do.
}

BodyPart::~BodyPart()
{
    // Nothing to do.
}

bool BodyPart::check()
{
    assert(vnum > 0);
    assert(!name.empty());
    assert(!description.empty());
    return true;
}

std::string BodyPart::getDescription(bool capital) const
{
    return (capital) ? ToCapitals(description) : description;
}

void BodyPart::getSheet(Table & sheet) const
{
    // Add the columns.
    sheet.addColumn("Attribute", StringAlign::Left);
    sheet.addColumn("Value", StringAlign::Left);
    // Set the values.
    sheet.addRow({"Vnum", ToString(vnum)});
    sheet.addRow({"Name", name});
    sheet.addRow({"Description", description});
    sheet.addRow({"Flags", ToString(flags)});
    if (!resources.empty())
    {
        sheet.addDivider();
        sheet.addRow({"Resource", "Quantity"});
        for (auto it : resources)
        {
            sheet.addRow({it.resource->name, ToString(it.quantity)});
        }
    }
}

std::string BodyPart::BodyWeapon::getName(bool colored) const
{
    if (colored)
    {
        return Formatter::cyan() + name + Formatter::reset();
    }
    return name;
}

unsigned int BodyPart::BodyWeapon::rollDamage() const
{
    return TRand<unsigned int>(minDamage, maxDamage);
}