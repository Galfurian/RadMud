/// @file   keyModel.cpp
/// @brief  Iplement the methods for Key.
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

#include "keyModel.hpp"

KeyModel::KeyModel()
{
    // Nothing to do.
}

KeyModel::~KeyModel()
{
    // Nothing to do.
}

ModelType KeyModel::getType() const
{
    return ModelType::Key;
}

std::string KeyModel::getTypeName() const
{
    return "Key";
}

bool KeyModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 0)
    {
        Logger::log(LogLevel::Error, "Wrong number of parameters for Key Model (%s).", this->name);
        return false;
    }
    return true;
}

void KeyModel::getSheet(Table & sheet)
{
    // Call the function of the father class.
    ItemModel::getSheet(sheet);
    // Add a divider.
    //sheet.addDivider();
}
