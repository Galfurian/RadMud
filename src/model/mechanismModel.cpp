/// @file   mechanismModel.cpp
/// @brief  Iplement the methods for Mechanism.
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

#include "mechanismModel.hpp"

MechanismModel::MechanismModel() :
    mechanismType(),
    key(),
    difficulty(),
    efficency(),
    command(),
    target()
{
    // Nothing to do.
}

MechanismModel::~MechanismModel()
{
    // Nothing to do.
}

ModelType MechanismModel::getType() const
{
    return ModelType::Mechanism;
}

std::string MechanismModel::getTypeName() const
{
    return "Mechanism";
}

bool MechanismModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 6)
    {
        Logger::log(
            LogLevel::Error,
            "Wrong number of parameters for Mechanism Model (%s).",
            this->name);
        return false;
    }
    this->mechanismType = static_cast<MechanismType>(ToNumber<unsigned int>(functionList[0]));

    if ((this->mechanismType == MechanismType::Door)
        || (this->mechanismType == MechanismType::Lock))
    {
        this->key = ToNumber<unsigned int>(functionList[1]);
        this->difficulty = ToNumber<unsigned int>(functionList[2]);
    }
    else if (this->mechanismType == MechanismType::Picklock)
    {
        this->efficency = ToNumber<unsigned int>(functionList[3]);
    }
    else if (this->mechanismType == MechanismType::Lever)
    {
        this->command = ToNumber<unsigned int>(functionList[4]);
        this->target = ToNumber<unsigned int>(functionList[5]);
    }
    return true;
}

void MechanismModel::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Mechanism Type", GetMechanismTypeName(this->mechanismType)});
    sheet.addRow({"Key", ToString(this->key)});
    sheet.addRow({"Picklock Difficulty", ToString(this->difficulty)});
    sheet.addRow({"Picklock Efficency", ToString(this->efficency)});
    sheet.addRow({"Lever command", ToString(this->command)});
    sheet.addRow({"Target Vnum", ToString(this->target)});
}

std::string GetMechanismTypeName(MechanismType type)
{
    if (type == MechanismType::Door) return "Door";
    if (type == MechanismType::Lock) return "Lock";
    if (type == MechanismType::Picklock) return "Picklock";
    if (type == MechanismType::Lever) return "Lever";
    return "No Mechanism Type";
}
