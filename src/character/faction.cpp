/// @file   faction.cpp
/// @brief  Implements the faction functions.
/// @author Enrico Fraccaroli
/// @date   Sep 30 2014
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

#include "character/faction.hpp"

#include "model/submodel/currencyModel.hpp"
#include "utilities/logger.hpp"

Faction::Faction() : vnum(), name(), description(), currency()
{
	// Nothing to do.
}

Faction::~Faction()
{
	//    MudLog(LogLevel::Debug,
	//                "Deleted faction\t[%s]\t\t(%s)",
	//                ToString(this->vnum),
	//                this->name);
}

bool Faction::check()
{
	if (vnum == 0)
		return false;
	if (name.empty())
		return false;
	if (description.empty())
		return false;
	if (currency == nullptr)
		return false;
	return true;
}

void Faction::getSheet(Table &sheet) const
{
	// Add the columns.
	sheet.addColumn("Attribute", align::left);
	sheet.addColumn("Value", align::left);
	// Set the values.
	sheet.addRow({ "Vnum", ToString(this->vnum) });
	sheet.addRow({ "Name", this->name });
	sheet.addRow({ "Description", this->description });
	sheet.addRow({ "Currency", currency->getName() + " (" + ToString(currency->vnum) + ")" });
}

std::string Faction::getName()
{
	return ToLower(name);
}

std::string Faction::getNameCapital()
{
	return name;
}
