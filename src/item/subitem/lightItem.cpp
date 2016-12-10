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
#include "sqliteDbms.hpp"
#include "resourceModel.hpp"
#include "updater.hpp"

LightItem::LightItem() :
    active(),
    remainingHours()
{
    // Nothing to do.
}

LightItem::~LightItem()
{
    // Nothing to do.
}

void LightItem::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Item::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow({"Remaining Autonomy", ToString(remainingHours) + " h"});
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
    // Set by default the ammout to load to the maximum.
    ammount = this->model->toLight()->maxHours;
    unsigned int ammountAlreadyLoaded = 0;
    // Retrieve any already loaded fuel.
    Item * alreadyLoaded = this->getAlreadyLoadedFuel();
    if (alreadyLoaded != nullptr)
    {
        // Set the ammount of already loaded fuel.
        ammountAlreadyLoaded = alreadyLoaded->quantity;
        if (ammount <= ammountAlreadyLoaded)
        {
            error = this->getNameCapital(true) + " is already at full capacity.";
            return false;
        }
        ammount -= ammountAlreadyLoaded;
    }
    return true;

}

Item * LightItem::getAlreadyLoadedFuel() const
{
    if (model->toLight()->fuelType == ResourceType::None)
    {
        return nullptr;
    }
    if (!this->isEmpty())
    {
        Item * fuel = this->content.front();
        if (fuel != nullptr)
        {
            if (fuel->getType() == ModelType::Resource)
            {
                return fuel;
            }
        }
    }
    return nullptr;
}

void LightItem::updateHourImpl()
{
    auto fuel = this->getAlreadyLoadedFuel();
    if (fuel != nullptr)
    {
        // Check if it is the last unit of fuel.
        if (fuel->quantity == 1)
        {
            MudUpdater::instance().addItemToDestroy(fuel);
        }
        else
        {
            fuel->quantity--;
        }
    }
    // Call the hourly update.
    Item::updateHourImpl();
}
