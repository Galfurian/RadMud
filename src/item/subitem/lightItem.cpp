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

#include "lightItem.hpp"

#include "lightModel.hpp"
#include "resourceModel.hpp"
#include "updater.hpp"
#include "logger.hpp"
#include "formatter.hpp"

LightItem::LightItem() :
    active()
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
    if (this->active)
    {
        itemName += " (lit)";
    }
    return itemName;
}

std::string LightItem::getNameCapital(bool colored) const
{
    std::string itemName = Item::getNameCapital(colored);
    if (this->active)
    {
        itemName += " (lit)";
    }
    return itemName;
}

void LightItem::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Item::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Remaining Autonomy", ToString(this->getAutonomy()) + " h"});
}


std::string LightItem::lookContent()
{
    std::string output;
    auto autonomyInHour = this->getAutonomy() / MudUpdater::instance().getHourTicSize();
    output += Formatter::italic();
    if (model->toLight()->fuelType != ResourceType::None)
    {
        if (content.empty())
        {
            output += "It does not contain any fuel.\n";
        }
        else
        {
            output += "It contains enough fuel for " + ToString(autonomyInHour) + "h.\n";
        }
    }
    else
    {
        output += "It should have an autonomy of " + ToString(autonomyInHour) + "h.\n";
    }
    if (active)
    {
        output += "It is lit.";
    }
    else
    {
        output += "It is unlit.";
    }
    output += Formatter::reset() + "\n";
    return output;
}

bool LightItem::canRefillWith(Item * item, std::string & error) const
{
    if (item == nullptr)
    {
        error = "You are not able to refill " + this->getName(true) + ".";
        return false;
    }
    if (item->getType() != ModelType::Resource)
    {
        error = "You can't refill " + this->getName(true) + " with " + item->getName(true);
        return false;
    }
    if (model->getType() != ModelType::Light)
    {
        error = "You can't refill " + this->getName(true) + " with " + item->getName(true);
        return false;
    }
    if (model->toLight()->fuelType == ResourceType::None)
    {
        error = this->getNameCapital(true) + " is not meant to be refilled.";
        return false;
    }
    if (model->toLight()->fuelType != item->model->toResource()->resourceType)
    {
        error = "You can't refill " + this->getName(true) + " with " + item->getName(true);
        return false;
    }
    return true;
}

bool LightItem::getAmmountToRefill(Item * item, unsigned int & ammount, std::string & error) const
{
    if (!this->canRefillWith(item, error))
    {
        return false;
    }
    // Get the weight of the fuel.
    auto fuelWeight = item->getWeight(false);
    // Set by default the ammout to the maximum.
    auto maxWeight = this->model->toLight()->maxWeight;
    // Evaluate the weight of the content.
    auto contentWeight = 0.0;
    for (auto it : content)
    {
        contentWeight += it->getWeight(true);
    }
    // Check if the weight of the fuel exceeds the available space.
    if ((maxWeight - contentWeight) < fuelWeight)
    {
        Logger::log(LogLevel::Debug, "(%s - %s) < %s", maxWeight, contentWeight, fuelWeight);
        error = this->getNameCapital(true) + " is already at full capacity.";
        return false;
    }
    // Get the weight that can still be used.
    auto canRefill = (maxWeight - contentWeight) / fuelWeight;
    // Set the ammount.
    ammount = static_cast<unsigned int>(std::floor(canRefill));
    return true;
}

std::vector<Item *> LightItem::getAlreadyLoadedFuel() const
{
    std::vector<Item *> fuel;
    if (model->toLight()->fuelType != ResourceType::None)
    {
        for (auto it : content)
        {
            fuel.push_back(it);
        }
    }
    return fuel;
}

double LightItem::getAutonomy() const
{
    double autonomy = 0;
    if (model->toLight()->fuelType == ResourceType::None)
    {
        autonomy = (condition / this->getDecayRate());
    }
    else
    {
        auto loadedFuel = this->getAlreadyLoadedFuel();
        for (auto fuel : loadedFuel)
        {
            if (fuel != nullptr)
            {
                autonomy += (fuel->condition / fuel->getDecayRate());
            }
        }
    }
    return autonomy;
}

void LightItem::updateTicImpl()
{
    if (active)
    {
        if (model->toLight()->fuelType == ResourceType::None)
        {
            // Trigger the dacay.
            this->triggerDecay();
            // Just for precaution, deactivate the light source if the condition is below zero.
            if (this->condition < 0)
            {
                active = false;
            }
        }
        else
        {
            auto loadedFuel = this->getAlreadyLoadedFuel();
            if (loadedFuel.empty())
            {
                active = false;
            }
            else
            {
                // Get the first element of fuel.
                auto fuel = loadedFuel.front();
                // Trigger the dacay.
                fuel->triggerDecay();
                // Check if it is the last unit of fuel.
                if (fuel->condition < 0)
                {
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
