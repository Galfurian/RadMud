/// @file   corpseItem.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 17 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include "corpseItem.hpp"

#include "../model/corpseModel.hpp"
#include "../material.hpp"
#include "../utils.hpp"
#include "../mud.hpp"

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
        Logger::log(LogLevel::Debug, "Removing item '%s' from MUD corpses.", this->getName());
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
