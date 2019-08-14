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

#include "formatter.hpp"
#include "logger.hpp"
#include "mud.hpp"

CorpseItem::CorpseItem() : remainingBodyParts()
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
	if (Mud::instance().remCorpse(this)) {
		Logger::log(LogLevel::Debug, "Removing item '%s' from MUD corpses.",
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

void CorpseItem::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	Item::getSheet(sheet);
	// Add a divider.
	sheet.addDivider();
	// Set the values.
	for (auto it : remainingBodyParts) {
		sheet.addRow({ "Body Part", it->name });
	}
}

std::string CorpseItem::lookContent()
{
	if (content.empty()) {
		return Formatter::italic("The corpse does not contain anything.\n");
	}
	std::stringstream ss;
	ss << "Looking inside the corpse you see:\n";
	for (auto it : content) {
		ss << " [" << std::right << std::setw(3) << it->quantity << "] ";
		ss << it->getNameCapital() << "\n";
	}
	return ss.str();
}

bool CorpseItem::isAContainer() const
{
	return true;
}

std::shared_ptr<BodyPart> CorpseItem::getAvailableBodyPart()
{
	// Lock the access to the item.
	std::lock_guard<std::mutex> lock(itemMutex);
	// Proceed with the function.
	if (remainingBodyParts.empty()) {
		return nullptr;
	}
	return remainingBodyParts.front();
}

bool CorpseItem::removeBodyPart(const std::shared_ptr<BodyPart> &bodyPart)
{
	// Lock the access to the item.
	std::lock_guard<std::mutex> lock(itemMutex);
	// Proceed with the function.
	for (auto it = remainingBodyParts.begin(); it != remainingBodyParts.end();
		 ++it) {
		if ((*it)->vnum == bodyPart->vnum) {
			remainingBodyParts.erase(it);
			remainingBodyParts.shrink_to_fit();
			return true;
		}
	}
	return false;
}