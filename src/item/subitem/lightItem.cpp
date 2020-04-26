/// @file   lightItem.cpp
/// @brief
/// @author Enrico Fraccaroli
/// @date   05/12/2016
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

#include "item/subitem/lightItem.hpp"

#include "model/submodel/lightModel.hpp"
#include "model/submodel/resourceModel.hpp"
#include "updater/updater.hpp"
#include "utilities/logger.hpp"
#include "utilities/formatter.hpp"

LightItem::LightItem() : active()
{
	// Nothing to do.
}

LightItem::~LightItem()
{
	// Nothing to do.
}

std::string LightItem::getName(bool colored) const
{
	std::string itemName = Item::getName(colored);
	if (this->isActive()) {
		itemName += " (lit)";
	}
	return itemName;
}

std::string LightItem::getNameCapital(bool colored) const
{
	std::string itemName = Item::getNameCapital(colored);
	if (this->isActive()) {
		itemName += " (lit)";
	}
	return itemName;
}

void LightItem::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	Item::getSheet(sheet);
	// Add a divider.
	sheet.addDivider();
	// Set the values.
	sheet.addRow(
		{ "Remaining Autonomy", ToString(this->getAutonomy()) + " h" });
}

std::string LightItem::lookContent()
{
	std::string output;
	auto autonomyInHour =
		this->getAutonomy() / MudUpdater::instance().getHourTicSize();
	output += Formatter::italic();
	if (model->to<LightModel>()->fuelType == ResourceType::None) {
		if (!HasFlag(model->modelFlags, ModelFlag::Unbreakable)) {
			output += "It should have an autonomy of " +
					  ToString(autonomyInHour) + "h.\n";
		}
	} else {
		if (content.empty()) {
			output += "It does not contain any fuel.\n";
		} else {
			output += "It contains enough fuel for " +
					  ToString(autonomyInHour) + "h.\n";
		}
	}
	if (this->isActive()) {
		output += "It is lit.";
	} else {
		output += "It is unlit.";
	}
	output += Formatter::reset() + "\n";
	return output;
}

double LightItem::getTotalSpace() const
{
	// The base space.
	double spaceBase = model->to<LightModel>()->maxWeight;
	// Evaluate the result.
	return ((spaceBase + (spaceBase * quality.getModifier())) / 2);
}

bool LightItem::isActive() const
{
	return active || HasFlag(model->to<LightModel>()->lightSourceFlags,
							 LightModelFlags::AlwaysActive);
}

bool LightItem::canRefillWith(Item *item, std::string &error) const
{
	if (item == nullptr) {
		error = "You are not able to refill " + this->getName(true) + ".";
		return false;
	}
	if (item->getType() != ModelType::Resource) {
		error = "You can't refill " + this->getName(true) + " with " +
				item->getName(true);
		return false;
	}
	if (model->getType() != ModelType::Light) {
		error = "You can't refill " + this->getName(true) + " with " +
				item->getName(true);
		return false;
	}
	if (model->to<LightModel>()->fuelType == ResourceType::None) {
		error = this->getNameCapital(true) + " is not meant to be refilled.";
		return false;
	}
	if (model->to<LightModel>()->fuelType !=
		item->model->to<ResourceModel>()->resourceType) {
		error = "You can't refill " + this->getName(true) + " with " +
				item->getName(true);
		return false;
	}
	return true;
}

bool LightItem::getAmountToRefill(Item *item, unsigned int &amount,
								  std::string &error) const
{
	if (!this->canRefillWith(item, error)) {
		return false;
	}
	// Get the weight of the fuel.
	auto fuelWeight = item->getWeight(false);
	// Set by default the amount to the maximum.
	auto maxWeight = this->model->to<LightModel>()->maxWeight;
	// Evaluate the weight of the content.
	auto contentWeight = 0.0;
	for (auto it : content) {
		contentWeight += it->getWeight(true);
	}
	// Check if the weight of the fuel exceeds the available space.
	if ((maxWeight - contentWeight) < fuelWeight) {
		MudLog(LogLevel::Debug, "(%f - %f) < %f", maxWeight, contentWeight,
			   fuelWeight);
		error = this->getNameCapital(true) + " is already at full capacity.";
		return false;
	}
	// Get the weight that can still be used.
	auto canRefill = (maxWeight - contentWeight) / fuelWeight;
	// Set the amount.
	amount = static_cast<unsigned int>(std::floor(canRefill));
	return true;
}

ItemVector LightItem::getAlreadyLoadedFuel() const
{
	ItemVector fuel;
	if (model->to<LightModel>()->fuelType != ResourceType::None) {
		for (auto it : content) {
			fuel.emplace_back(it);
		}
	}
	return fuel;
}

double LightItem::getAutonomy() const
{
	double autonomy = 0;
	if (model->to<LightModel>()->fuelType == ResourceType::None) {
		autonomy = (condition / this->getDecayRate());
	} else {
		auto loadedFuel = this->getAlreadyLoadedFuel();
		for (auto fuel : loadedFuel) {
			if (fuel != nullptr) {
				autonomy += (fuel->condition / fuel->getDecayRate());
			}
		}
	}
	return autonomy;
}

void LightItem::updateTicImpl()
{
	if (this->isActive()) {
		if (model->to<LightModel>()->fuelType == ResourceType::None) {
			// Trigger the dacay.
			this->triggerDecay();
			// Just for precaution, deactivate the light source if
			//  the condition is below zero.
			if (this->condition < 0) {
				active = false;
			}
		} else {
			auto loadedFuel = this->getAlreadyLoadedFuel();
			if (loadedFuel.empty()) {
				active = false;
			} else {
				// Get the first element of fuel.
				auto fuel = loadedFuel.front();
				// Trigger the decay.
				fuel->triggerDecay();
				// Check if it is the last unit of fuel.
				if (fuel->condition < 0) {
					// Set the fuel to destroy.
					MudUpdater::instance().addItemToDestroy(fuel);
				}
			}
		}
	}
	Item::updateTicImpl();
}

void LightItem::updateHourImpl()
{
	Item::updateHourImpl();
}
