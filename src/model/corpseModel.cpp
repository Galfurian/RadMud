/// @file   corpseModel.cpp
/// @brief  Iplement the methods for Corpse.
/// @author Enrico Fraccaroli
/// @date   Jul 6 2016
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

#include "corpseModel.hpp"

#include "../mud.hpp"
#include "../item/item.hpp"

CorpseModel::CorpseModel()
{
    // Nothing to do.
}

CorpseModel::~CorpseModel()
{
    // Nothing to do.
}

ModelType CorpseModel::getType() const
{
    return ModelType::Corpse;
}

std::string CorpseModel::getTypeName() const
{
    return "Corpse";
}

bool CorpseModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 0)
    {
        Logger::log(
            LogLevel::Error,
            "Wrong number of parameters for Corpse Model (%s).",
            this->name);
        return false;
    }
    return true;
}

void CorpseModel::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    //sheet.addDivider();
}

Item * CorpseModel::createItem(std::string, Material *, const ItemQuality &)
{
    Logger::log(LogLevel::Error, "Use the proper createCorpse function.");
    return nullptr;
}

Item * CorpseModel::createCorpse(
    std::string maker,
    Material * composition,
    const unsigned int & weight)
{
    // Instantiate the new item.
    Item * newItem = GenerateItem(this->getType());
    if (newItem == nullptr)
    {
        Logger::log(LogLevel::Error, "Cannot create the new item.");
        // Return pointer to nothing.
        return nullptr;
    }

    // First set: Vnum, Model, Maker, Composition, Quality.
    newItem->vnum = Mud::instance().getMinVnumCorpse() - 1;
    newItem->model = this;
    newItem->maker = maker;
    newItem->composition = composition;
    newItem->quality = ItemQuality::Normal;
    // Then set the rest.
    newItem->price = 0;
    newItem->weight = weight;
    newItem->condition = weight;
    newItem->maxCondition = weight;
    newItem->flags = 0;
    newItem->room = nullptr;
    newItem->owner = nullptr;
    newItem->container = nullptr;
    newItem->currentSlot = slot;
    newItem->content = std::vector<Item *>();
    newItem->contentLiq = LiquidContent();

    Mud::instance().addCorpse(newItem);
    return newItem;
}
