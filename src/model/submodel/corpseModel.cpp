/// @file   corpseModel.cpp
/// @brief  Iplement the methods for Corpse.
/// @author Enrico Fraccaroli
/// @date   Jul 6 2016
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

#include "model/submodel/corpseModel.hpp"
#include "item/subitem/corpseItem.hpp"
#include "utilities/logger.hpp"
#include "mud.hpp"

CorpseModel::CorpseModel() :
    corpseRace(),
    corpseComposition()
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
        Logger::log(LogLevel::Error,
                    "Function list is empty (%s).",
                    this->name);
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
    sheet.addDivider();
    if (corpseRace != nullptr)
    {
        sheet.addRow({"Corpse", corpseRace->name});
    }
}

Item * CorpseModel::createItem(std::string,
                               Material *,
                               bool,
                               const ItemQuality &,
                               const unsigned int &)
{
    Logger::log(LogLevel::Error, "Use the proper createCorpse function.");
    return nullptr;
}

Item * CorpseModel::createCorpse(
    std::string maker,
    const double & weight)
{
    // Instantiate the new item.
    auto newCorpse = new CorpseItem();
    if (newCorpse == nullptr)
    {
        Logger::log(LogLevel::Error, "Cannot create the new item.");
        // Return pointer to nothing.
        return nullptr;
    }

    // First set: Vnum, Model, Maker, Composition, Quality.
    newCorpse->vnum = Mud::instance().getMinVnumCorpse() - 1;
    newCorpse->model = this->shared_from_this();
    newCorpse->maker = maker;
    newCorpse->quality = ItemQuality::Normal;
    // Then set the rest.
    newCorpse->weight = weight;
    newCorpse->condition = static_cast<unsigned int>(weight);
    newCorpse->maxCondition = static_cast<unsigned int>(weight);
    newCorpse->composition = corpseComposition;
    newCorpse->remainingBodyParts = corpseRace->bodyParts;

    Mud::instance().addCorpse(newCorpse);
    return newCorpse;
}
