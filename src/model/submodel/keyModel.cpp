/// @file   keyModel.cpp
/// @brief  Iplement the methods for Key.
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

#include "keyModel.hpp"

#include "logger.hpp"

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

bool KeyModel::setModel(const std::string &source)
{
	if (source.empty()) {
		MudLog(LogLevel::Error, "Function list is empty (%s).",
					this->name);
		return false;
	}
	std::vector<std::string> functionList = SplitString(source, " ");
	if (functionList.size() != 0) {
		MudLog(LogLevel::Error,
					"Wrong number of parameters for Key Model (%s).",
					this->name);
		return false;
	}
	return true;
}

void KeyModel::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	ItemModel::getSheet(sheet);
	// Add a divider.
	//sheet.addDivider();
}
