/// @file   corpseItem.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 17 2016
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

#include "corpseItem.hpp"

#include "logger.hpp"
#include "mud.hpp"

CorpseItem::CorpseItem()
{
    // Nothing to do.
}

CorpseItem::~CorpseItem()
{
    // Nothing to do.
}

void CorpseItem::removeFromMud()
{
    Item::removeFromMud();
    if (Mud::instance().remCorpse(this))
    {
        Logger::log(LogLevel::Debug,
                    "Removing item '%s' from MUD corpses.",
                    this->getName());
    }
}

bool CorpseItem::updateOnDB()
{
    return true;
}

bool CorpseItem::removeOnDB()
{
    return true;
}

void CorpseItem::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Item::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
}

bool CorpseItem::isAContainer() const
{
    return true;
}

bool CorpseItem::isEmpty() const
{
    return content.empty();
}
