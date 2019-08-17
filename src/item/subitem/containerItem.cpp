/// @file   containerItem.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 04 2017
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

#include "containerItem.hpp"
#include "containerModel.hpp"
#include "formatter.hpp"
#include "mud.hpp"

ContainerItem::ContainerItem()
{
	// Nothing to do.
}

ContainerItem::~ContainerItem()
{
	// Nothing to do.
}

void ContainerItem::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	Item::getSheet(sheet);
	// Add a divider.
	sheet.addDivider();
	// Set the values.
}

bool ContainerItem::isAContainer() const
{
	return true;
}

double ContainerItem::getTotalSpace() const
{
	// The base space.
	double spaceBase = model->to<ContainerModel>()->maxWeight;
	// Evaluate the result.
	return ((spaceBase + (spaceBase * quality.getModifier())) / 2);
}

std::string ContainerItem::lookContent()
{
	std::stringstream ss;
	if (HasFlag(this->flags, ItemFlag::Closed)) {
		ss << Formatter::italic("It is closed.\n\n");
		if (!HasFlag(this->model->modelFlags, ModelFlag::CanSeeThrough)) {
			return ss.str();
		}
	}
	if (content.empty()) {
		ss << Formatter::italic("It's empty.\n\n");
		return ss.str();
	}
	ss << "Looking inside you see:\n";
	for (auto it : content) {
		ss << " [" << std::right << std::setw(3) << it->quantity << "] ";
		ss << it->getNameCapital() << "\n";
	}
	ss << "Has been used " << Formatter::yellow(ToString(getUsedSpace()));
	ss << " out of " << Formatter::yellow(ToString(getTotalSpace())) << ' ';
	ss << Mud::instance().getWeightMeasure() << ".\n";
	return ss.str();
}