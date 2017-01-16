/// @file   character.cpp
/// @brief  Implements all the methods need to manipulate a character.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
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

#include "character.hpp"

#include "lightItem.hpp"
#include "armorItem.hpp"
#include "logger.hpp"
#include "aStar.hpp"
#include "mud.hpp"

Character::Character() :
    name(),
    description(),
    gender(),
    weight(),
    level(),
    flags(),
    race(),
    faction(),
    health(),
    stamina(),
    hunger(100),
    thirst(100),
    room(),
    inventory(),
    equipment(),
    posture(CharacterPosture::Stand),
    effects(),
    L(luaL_newstate()),
    combatHandler(this),
    actionQueue(),
    actionQueueMutex(),
    inputProcessor(std::make_shared<ProcessInput>())
{
    // Push the Wait action on the action queue.
    std::lock_guard<std::mutex> lock(actionQueueMutex);
    actionQueue.push_front(std::make_shared<GeneralAction>(this));
}

Character::~Character()
{
    lua_close(L);
}

bool Character::check() const
{
    bool safe = true;
    safe &= CorrectAssert(!name.empty());
    safe &= CorrectAssert(!description.empty());
    safe &= CorrectAssert(weight > 0);
    safe &= CorrectAssert(race != nullptr);
    safe &= CorrectAssert(faction != nullptr);
    safe &= CorrectAssert(this->getAbility(Ability::Strength, false) > 0);
    safe &= CorrectAssert(this->getAbility(Ability::Strength, false) <= 60);
    safe &= CorrectAssert(this->getAbility(Ability::Agility, false) > 0);
    safe &= CorrectAssert(this->getAbility(Ability::Agility, false) <= 60);
    safe &= CorrectAssert(this->getAbility(Ability::Perception, false) > 0);
    safe &= CorrectAssert(this->getAbility(Ability::Perception, false) <= 60);
    safe &= CorrectAssert(this->getAbility(Ability::Constitution, false) > 0);
    safe &= CorrectAssert(this->getAbility(Ability::Constitution, false) <= 60);
    safe &= CorrectAssert(this->getAbility(Ability::Intelligence, false) > 0);
    safe &= CorrectAssert(this->getAbility(Ability::Intelligence, false) <= 60);
    safe &= CorrectAssert(hunger >= 0);
    safe &= CorrectAssert(thirst >= 0);
    safe &= CorrectAssert(room != nullptr);
    safe &= CorrectAssert(L != nullptr);
    return safe;
}

bool Character::isMobile() const
{
    return false;
}

bool Character::isPlayer() const
{
    return false;
}

void Character::getSheet(Table & sheet) const
{
    // Add the columns.
    sheet.addColumn("Attribute", StringAlign::Left);
    sheet.addColumn("Value", StringAlign::Left);
    // Set the values.
    sheet.addRow({"Proper Noun", name});
    sheet.addRow({"Description", description});
    sheet.addRow({"Gender", GetGenderTypeName(gender)});
    sheet.addRow({"Weight", ToString(weight)});
    sheet.addRow({"Level", ToString(level)});
    sheet.addRow({"Flags", GetCharacterFlagString(flags)});
    if (CorrectAssert(race != nullptr))
    {
        sheet.addRow({"Race", race->name});
    }
    else
    {
        sheet.addRow({"Race", "NONE"});
    }
    if (CorrectAssert(faction != nullptr))
    {
        sheet.addRow({"Faction", faction->name});
    }
    else
    {
        sheet.addRow({"Faction", "NONE"});
    }
    sheet.addRow({"Health", ToString(health)});
    sheet.addRow({"Stamina", ToString(stamina)});
    sheet.addRow({"Hunger", ToString(hunger)});
    sheet.addRow({"Thirst", ToString(thirst)});
    sheet.addRow(
        {
            "Strength",
            ToString(this->getAbility(Ability::Strength, false)) +
            " [" +
            ToString(
                effects.getAbilityModifier(AbilityModifier::IncreaseStrength)) +
            "][" +
            ToString(
                effects.getAbilityModifier(AbilityModifier::DecreaseStrength)) +
            "]"});
    sheet.addRow(
        {
            "Agility",
            ToString(this->getAbility(Ability::Agility, false)) +
            " [" +
            ToString(
                effects.getAbilityModifier(AbilityModifier::IncreaseAgility)) +
            "][" +
            ToString(
                effects.getAbilityModifier(AbilityModifier::DecreaseAgility)) +
            "]"});
    sheet.addRow(
        {
            "Perception",
            ToString(this->getAbility(Ability::Perception, false)) +
            " [" +
            ToString(effects.getAbilityModifier(
                AbilityModifier::IncreasePerception)) +
            "][" +
            ToString(effects.getAbilityModifier(
                AbilityModifier::DecreasePerception)) +
            "]"});
    sheet.addRow(
        {
            "Constitution",
            ToString(this->getAbility(Ability::Constitution, false)) +
            " [" +
            ToString(effects.getAbilityModifier(
                AbilityModifier::IncreaseConstitution)) +
            "][" +
            ToString(effects.getAbilityModifier(
                AbilityModifier::DecreaseConstitution)) +
            "]"});
    sheet.addRow(
        {
            "Intelligence",
            ToString(this->getAbility(Ability::Intelligence, false)) +
            " [" +
            ToString(effects.getAbilityModifier(
                AbilityModifier::IncreaseIntelligence)) +
            "][" +
            ToString(effects.getAbilityModifier(
                AbilityModifier::DecreaseIntelligence)) +
            "]"});
    if (CorrectAssert(this->room != nullptr))
    {
        sheet.addRow({"Room", room->name + " [" + ToString(room->vnum) + "]"});
    }
    else
    {
        sheet.addRow({"Room", "NONE"});
    }
    sheet.addRow({"Posture", posture.toString()});
//    sheet.addRow({"Action", actionQueue.front()->getDescription()});
    sheet.addDivider();
    sheet.addRow({"## Equipment", "## Inventory"});
    for (size_t it = 0; it < std::max(inventory.size(), equipment.size()); ++it)
    {
        std::string equipmentItem, inventoryItem;
        if (it < equipment.size())
        {
            equipmentItem = equipment.at(it)->getName();
        }
        if (it < inventory.size())
        {
            inventoryItem = inventory.at(it)->getName();
        }
        sheet.addRow({equipmentItem, inventoryItem});
    }
    sheet.addDivider();
    sheet.addRow({"## Effect Name", "## Remaining TIC"});
    for (const auto & it : effects)
    {
        sheet.addRow({it.name, ToString(it.remainingTic)});
    }
}

//std::string Character::getName() const
//{
//    if (this->isPlayer())
//    {
//        return ToLower(name);
//    }
//    else if (this->isMobile())
//    {
//        auto mobile = dynamic_cast<const Mobile *>(this);
//        return ToLower(mobile->staticdesc);
//    }
//    else
//    {
//        return "No name";
//    }
//}

std::string Character::getNameCapital() const
{
    return ToCapitals(this->getName());
}

std::string Character::getStaticDesc() const
{
    // Name
    std::string desc(this->getNameCapital());
    // Posture
    desc += " is";
    if (posture != CharacterPosture::Stand)
    {
        desc += " " + posture.getAction();
    }
    desc += " here";
//    if (actionQueue.front()->getType() != ActionType::Wait)
//    {
//        desc += ", " + this->getSubjectPronoun() + " is ";
//        desc += actionQueue.front()->getDescription();
//    }
    desc += ".";
    return desc;
}

std::string Character::getSubjectPronoun() const
{
    if (gender == GenderType::Male) return "he";
    if (gender == GenderType::Female) return "she";
    return "it";
}

std::string Character::getPossessivePronoun() const
{
    if (gender == GenderType::Male) return "his";
    if (gender == GenderType::Female) return "her";
    return "its";
}

std::string Character::getObjectPronoun() const
{
    if (gender == GenderType::Male) return "him";
    if (gender == GenderType::Female) return "her";
    return "it";
}

bool Character::setAbility(const Ability & ability, const unsigned int & value)
{
    if (value <= 60)
    {
        abilities[ability] = value;
        return true;
    }
    return false;
}

unsigned int Character::getAbility(const Ability & ability,
                                   bool withEffects) const
{
    auto overall = 0;
    // Try to find the ability value.
    auto it = abilities.find(ability);
    if (it != abilities.end())
    {
        overall = static_cast<int>(it->second);
    }
    // Add the effects if needed.
    if (withEffects)
    {
        // Add the effects which increase the ability.
        overall += effects.getAbilityModifier(ability.getIncreaseModifier());
        // Add the effects which decrease the ability.
        overall -= effects.getAbilityModifier(ability.getDecreaseModifier());
        // Prune the value if exceed the boundaries.
        if (overall < 0)
        {
            overall = 0;
        }
        else if (overall > 60)
        {
            overall = 60;
        }
    }
    return static_cast<unsigned int>(overall);
}

unsigned int Character::getAbilityModifier(const Ability & ability,
                                           bool withEffects) const
{
    return Ability::getModifier(this->getAbility(ability, withEffects));
}

unsigned int Character::getAbilityLog(
    const Ability & ability,
    const double & base,
    const double & multiplier,
    const bool & withEffects) const
{
    auto mod = this->getAbilityModifier(ability, withEffects);
    auto logMod = SafeLog10(mod);
    return static_cast<unsigned int>(base + logMod * multiplier);
}

bool Character::setHealth(const unsigned int & value, const bool & force)
{
    unsigned int maximum = this->getMaxHealth();
    if (value > maximum)
    {
        if (force)
        {
            this->health = maximum;
            return true;
        }
        else
        {
            return false;
        }
    }
    this->health = value;
    return true;
}

bool Character::addHealth(const unsigned int & value, const bool & force)
{
    unsigned int maximum = this->getMaxHealth();
    unsigned int result = this->health + value;
    if (result > maximum)
    {
        if (force)
        {
            result = maximum;
        }
        else
        {
            return false;
        }
    }
    this->health = result;
    return true;
}

bool Character::remHealth(const unsigned int & value, const bool & force)
{
    int result = static_cast<int>(this->health) - static_cast<int>(value);
    if (result < 0)
    {
        if (force)
        {
            result = 0;
        }
        else
        {
            return false;
        }
    }
    this->health = static_cast<unsigned int>(result);
    return true;
}

unsigned int Character::getMaxHealth(bool withEffects) const
{
    // Value = 5 + (5 * AbilityModifier(Constitution))
    unsigned int BASE = 5 + (5 * this->getAbility(Ability::Constitution));
    // Return it, if the maximum health is required without the effects.
    if (!withEffects) return BASE;
    // Otherwise evaluate the overall max health with the effects.
    int overall = static_cast<int>(BASE);
    overall += effects.getStatusModifier(StatusModifier::IncreaseHealth);
    overall -= effects.getStatusModifier(StatusModifier::DecreaseHealth);
    // If the maximum value is lesser than zero, return zero.
    if (overall < 0) return 0;
    return static_cast<unsigned int>(overall);
}

std::string Character::getHealthCondition(const bool & self)
{
    std::string sent_be, sent_have;
    // Determine who is the examined character.
    if (self)
    {
        sent_be = "are";
        sent_have = "have";
    }
    else
    {
        sent_be = "is";
        sent_have = "has";
    }
    // Determine the percentage of current health.
    auto percent = (100 * this->health) / this->getMaxHealth();
    // Determine the correct description.
    if (percent >= 100) return sent_be + " in perfect health";
    else if (percent >= 90) return sent_be + " slightly scratched";
    else if (percent >= 80) return sent_have + " a few bruises";
    else if (percent >= 70) return sent_have + " some cuts";
    else if (percent >= 60) return sent_have + " several wounds";
    else if (percent >= 50) return sent_have + " many nasty wounds";
    else if (percent >= 40) return sent_be + " bleeding freely";
    else if (percent >= 30) return sent_be + " covered in blood";
    else if (percent >= 20) return sent_be + " leaking guts";
    else if (percent >= 10) return sent_be + " almost dead";
    else return sent_be + " DYING";
}

bool Character::setStamina(const unsigned int & value, const bool & force)
{
    unsigned int maximum = this->getMaxStamina();
    if (value > maximum)
    {
        if (force)
        {
            stamina = maximum;
            return true;
        }
        else
        {
            return false;
        }
    }
    stamina = value;
    return true;
}

bool Character::addStamina(const unsigned int & value, const bool & force)
{
    unsigned int maximum = this->getMaxStamina();
    unsigned int result = stamina + value;
    if (result > maximum)
    {
        if (force)
        {
            result = maximum;
        }
        else
        {
            return false;
        }
    }
    stamina = result;
    return true;
}

bool Character::remStamina(const unsigned int & value, const bool & force)
{
    int result = static_cast<int>(stamina) - static_cast<int>(value);
    if (result < 0)
    {
        if (force)
        {
            result = 0;
        }
        else
        {
            return false;
        }
    }
    stamina = static_cast<unsigned int>(result);
    return true;
}

unsigned int Character::getMaxStamina(bool withEffects) const
{
    // Value = 10 + (10 * Ability(Constitution))
    unsigned int BASE = 10 + (10 * this->getAbility(Ability::Constitution));
    // Return it, if the maximum stamina is required without the effects.
    if (!withEffects) return BASE;
    // Otherwise evaluate the overall max stamina with the effects.
    int overall = static_cast<int>(BASE);
    overall += effects.getStatusModifier(StatusModifier::IncreaseStamina);
    overall -= effects.getStatusModifier(StatusModifier::DecreaseStamina);
    // If the maximum value is lesser than zero, return zero.
    if (overall < 0) return 0;
    return static_cast<unsigned int>(overall);
}

std::string Character::getStaminaCondition()
{
    // Determine the percentage of current health.
    auto percent = (100 * stamina) / this->getMaxStamina(true);
    // Determine the correct description.
    if (percent >= 100) return "look fresh as a daisy";
    else if (percent >= 80) return "are slightly weary";
    else if (percent >= 60) return "are quite tired";
    else if (percent >= 40) return "need to rest";
    else if (percent >= 20) return "need a good night of sleep";
    else return "are too tired, you can't even blink your eyes";
}

int Character::getViewDistance() const
{
    // Value = 4 + LogMod(PER)
    // MIN   = 4.0
    // MAX   = 7.2
    return 4 + static_cast<int>(this->getAbilityLog(Ability::Perception));
}

void Character::setAction(const std::shared_ptr<GeneralAction> & _action)
{
    std::lock_guard<std::mutex> lock(actionQueueMutex);
    if (_action->getType() != ActionType::Wait)
    {
        actionQueue.push_front(_action);
    }
}

void Character::popAction()
{
    std::lock_guard<std::mutex> lock(actionQueueMutex);
    actionQueue.pop_front();
}

std::shared_ptr<GeneralAction> & Character::getAction()
{
    std::lock_guard<std::mutex> lock(actionQueueMutex);
    return actionQueue.front();
}

void Character::moveTo(
    Room * destination,
    const std::string & msgDepart,
    const std::string & msgArrive,
    const std::string & msgChar)
{
    // Check if the current room exist.
    if (room == nullptr)
    {
        Logger::log(LogLevel::Error, "Character::moveTo: room is a NULLPTR.");
        return;
    }
    // Check if the destination exist.
    if (destination == nullptr)
    {
        Logger::log(LogLevel::Error,
                    "Character::moveTo: DESTINATION is a NULLPTR.");
        return;
    }

    // Activate the entrance event for every mobile in the room.
    for (auto mobile : room->getAllMobile(this))
    {
        if (mobile == this)
        {
            continue;
        }
        if (mobile->canSee(this))
        {
            mobile->triggerEventExit(this);
        }
    }

    // Show a message to the character, if is set.
    if (!msgChar.empty())
    {
        this->sendMsg(msgChar);
    }

    // Set the list of exceptions.
    std::vector<Character *> exceptions;
    exceptions.push_back(this);
    // Tell others where the character went and remove s/he from the old room.
    room->sendToAll(msgDepart, exceptions);

    // Remove the player from the current room.
    room->removeCharacter(this);

    // Add the character to the destionation room.
    destination->addCharacter(this);

    // Look around new room.
    this->doCommand("look");

    // Tell others s/he has arrived and move the character to the new room.
    destination->sendToAll(msgArrive, exceptions);

    // Activate the entrance event for every mobile in the room.
    for (auto mobile : room->getAllMobile(this))
    {
        if (mobile == this)
        {
            continue;
        }
        if (mobile->canSee(this))
        {
            mobile->triggerEventEnter(this);
        }
    }
}

Item * Character::findInventoryItem(std::string search_parameter, int & number)
{
    for (auto iterator : inventory)
    {
        if (iterator->hasKey(ToLower(search_parameter)))
        {
            if (number == 1)
            {
                return iterator;
            }
            number -= 1;
        }
    }
    return nullptr;
}

Item * Character::findEquipmentItem(std::string search_parameter, int & number)
{
    for (auto iterator : equipment)
    {
        if (iterator->hasKey(ToLower(search_parameter)))
        {
            if (number == 1)
            {
                return iterator;
            }
            number -= 1;
        }
    }
    return nullptr;
}

Item * Character::findItemAtBodyPart(
    const std::shared_ptr<BodyPart> & bodyPart) const
{
    for (auto item : equipment)
    {
        for (auto occupiedBodyPart : item->occupiedBodyParts)
        {
            if (occupiedBodyPart->vnum == bodyPart->vnum)
            {
                return item;
            }
        }
    }
    return nullptr;
}

Item * Character::findNearbyItem(const std::string & itemName, int & number)
{
    auto item = this->findInventoryItem(itemName, number);
    if (item == nullptr)
    {
        item = this->findEquipmentItem(itemName, number);
    }
    if (item == nullptr)
    {
        if (room != nullptr)
        {
            item = room->findItem(itemName, number);
        }
    }
    return item;
}

Item * Character::findNearbyTool(
    const ToolType & toolType,
    const ItemVector & exceptions,
    bool searchRoom,
    bool searchInventory,
    bool searchEquipment)
{
    if (searchRoom)
    {
        for (auto iterator : room->items)
        {
            if (iterator->model->getType() == ModelType::Tool)
            {
                auto toolModel = iterator->model->toTool();
                if (toolModel->toolType == toolType)
                {
                    // Check if the item is inside the exception list.
                    auto findIt = std::find_if(exceptions.begin(),
                                               exceptions.end(),
                                               [&iterator](Item * item)
                                               {
                                                   return (item->vnum ==
                                                           iterator->vnum);
                                               });
                    // If not, return the item.
                    if (findIt == exceptions.end())
                    {
                        return iterator;
                    }
                }
            }
        }
    }
    if (searchInventory)
    {
        for (auto iterator : inventory)
        {
            if (iterator->model->getType() == ModelType::Tool)
            {
                auto toolModel = iterator->model->toTool();
                if (toolModel->toolType == toolType)
                {
                    auto findIt = std::find(exceptions.begin(),
                                            exceptions.end(), iterator);
                    if (findIt == exceptions.end())
                    {
                        return iterator;
                    }
                }
            }
        }
    }
    if (searchEquipment)
    {
        for (auto iterator : equipment)
        {
            if (iterator->model->getType() == ModelType::Tool)
            {
                auto toolModel = iterator->model->toTool();
                if (toolModel->toolType == toolType)
                {
                    auto findIt = std::find(exceptions.begin(),
                                            exceptions.end(), iterator);
                    if (findIt == exceptions.end())
                    {
                        return iterator;
                    }
                }
            }
        }
    }
    return nullptr;
}

bool Character::findNearbyTools(
    std::set<ToolType> tools,
    ItemVector & foundOnes,
    bool searchRoom,
    bool searchInventory,
    bool searchEquipment)
{
    // TODO: Prepare a map with key the tool type and as value:
    //  Option A: A bool which determine if the tool has been found.
    //  Option B: A pointer to the found tool (more interesting, can pass to
    //              this function as reference the map and then update it with
    //              the found tools inside this function).
    for (auto toolType : tools)
    {
        auto tool = this->findNearbyTool(
            toolType,
            foundOnes,
            searchRoom,
            searchInventory,
            searchEquipment);
        if (tool == nullptr)
        {
            return false;
        }
        else
        {
            foundOnes.push_back(tool);
        }
    }
    return true;
}

bool Character::findNearbyResouces(
    std::map<ResourceType, unsigned int> ingredients,
    std::vector<std::pair<Item *, unsigned int>> & foundOnes)
{
    for (auto ingredient : ingredients)
    {
        // Quantity of ingredients that has to be found.
        auto quantityNeeded = ingredient.second;
        for (auto item : inventory)
        {
            auto model = item->model;
            if (model->getType() == ModelType::Resource)
            {
                auto resourceModel = model->toResource();
                if (resourceModel->resourceType == ingredient.first)
                {
                    auto quantityAvailable = item->quantity;
                    auto quantityUsed = quantityAvailable;
                    if (quantityAvailable > quantityNeeded)
                    {
                        quantityUsed = (quantityAvailable - quantityNeeded);
                    }
                    foundOnes.push_back(std::make_pair(item, quantityUsed));
                    quantityNeeded -= quantityUsed;
                    if (quantityNeeded == 0)
                    {
                        break;
                    }
                }
            }
        }
        // If the ingredients are not enough, search even in the room.
        if (quantityNeeded > 0)
        {
            for (auto item : room->items)
            {
                auto model = item->model;
                if (model->getType() == ModelType::Resource)
                {
                    auto resourceModel = model->toResource();
                    if (resourceModel->resourceType == ingredient.first)
                    {
                        auto quantityAvailable = item->quantity;
                        auto quantityUsed = quantityAvailable;
                        if (quantityAvailable > quantityNeeded)
                        {
                            quantityUsed = (quantityAvailable - quantityNeeded);
                        }
                        foundOnes.push_back(std::make_pair(item, quantityUsed));
                        quantityNeeded -= quantityUsed;
                        if (quantityNeeded == 0)
                        {
                            break;
                        }
                    }
                }
            }
        }
        // If the ingredients are still not enough, return false.
        if (quantityNeeded > 0)
        {
            return false;
        }
    }
    return true;
}

ItemVector Character::findCoins()
{
    ItemVector foundCoins;
    auto FindCoinInContainer = [&](Item * item)
    {
        if (item->getType() == ModelType::Container)
        {
            // Cast the item to container.
            auto containerItem = static_cast<ContainerItem *>(item);
            for (auto content : containerItem->content)
            {
                if (content->getType() == ModelType::Currency)
                {
                    foundCoins.emplace_back(content);
                }
            }
        }
    };
    for (auto it : equipment)
    {
        FindCoinInContainer(it);
    }
    for (auto it : inventory)
    {
        if (it->getType() == ModelType::Currency)
        {
            foundCoins.emplace_back(it);
        }
        else
        {
            FindCoinInContainer(it);
        }
    }
    foundCoins.orderBy(ItemVector::ByPrice);
    return foundCoins;
}

void Character::addInventoryItem(Item *& item)
{
    // Add the item to the inventory.
    inventory.push_back_item(item);
    // Set the owner of the item.
    item->owner = this;
    // Log it.
    Logger::log(LogLevel::Debug,
                "Item '%s' added to '%s' inventory;",
                item->getName(),
                this->getName());
}

void Character::addEquipmentItem(Item *& item)
{
    // Add the item to the equipment.
    equipment.push_back_item(item);
    // Set the owner of the item.
    item->owner = this;
    // Log it.
    Logger::log(LogLevel::Debug,
                "Item '%s' added to '%s' equipment;",
                item->getName(), this->getName());
}

bool Character::remInventoryItem(Item * item)
{
    // Remove the item from the inventory.
    if (!inventory.removeItem(item))
    {
        return false;
    }
    // Clear the owner of the item.
    item->owner = nullptr;
    // Log it.
    Logger::log(LogLevel::Debug,
                "Item '%s' removed from '%s';",
                item->getName(), this->getName());
    return true;
}

bool Character::remEquipmentItem(Item * item)
{
    // Remove the item from the equipment.
    if (!equipment.removeItem(item))
    {
        return false;
    }
    // Clear the owner of the item.
    item->owner = nullptr;
    // Empty the occupied body parts.
    item->occupiedBodyParts.clear();
    // Log it.
    Logger::log(LogLevel::Debug,
                "Item '%s' removed from '%s';",
                item->getName(), this->getName());
    return true;
}

bool Character::canCarry(Item * item, unsigned int quantity) const
{
    return ((this->getCarryingWeight() + (item->getWeight(false) * quantity)) <
            this->getMaxCarryingWeight());
}

double Character::getCarryingWeight() const
{
    auto carrying = 0.0;
    for (auto iterator : inventory)
    {
        carrying += iterator->getWeight(true);
    }
    for (auto iterator : equipment)
    {
        carrying += iterator->getWeight(true);
    }
    return carrying;
}

double Character::getMaxCarryingWeight() const
{
    // Value = 100 + (AbilMod(STR) * 10)
    // MIN   = 100
    // MAX   = 350
    return 100 + (this->getAbilityModifier(Ability::Strength) * 10);
}

std::vector<std::shared_ptr<BodyPart>> Character::canWield(
    Item * item,
    std::string & error) const
{
    // Prepare the list of occupied body parts.
    std::vector<std::shared_ptr<BodyPart>> occupiedBodyParts;
    // Get the compatible body parts.
    auto compatibleBodyParts = item->model->getBodyParts(race);
    if (compatibleBodyParts.empty())
    {
        error = "It is not designed for your fisionomy.";
        return occupiedBodyParts;
    }
    for (auto bodyPart : compatibleBodyParts)
    {
        if (!HasFlag(bodyPart->flags, BodyPartFlag::CanWield))
        {
            continue;
        }
        if (this->findItemAtBodyPart(bodyPart) != nullptr)
        {
            continue;
        }
        occupiedBodyParts.emplace_back(bodyPart);
        if (!HasFlag(item->model->modelFlags, ModelFlag::TwoHand))
        {
            break;
        }
        else
        {
            if (occupiedBodyParts.size() == 2)
            {
                break;
            }
        }
    }
    if (HasFlag(item->model->modelFlags, ModelFlag::TwoHand) &&
        (occupiedBodyParts.size() != 2))
    {
        occupiedBodyParts.clear();
    }
    if (occupiedBodyParts.empty())
    {
        error = "There is no room where it can be wielded.";
    }
    return occupiedBodyParts;
}

std::vector<std::shared_ptr<BodyPart>> Character::canWear(
    Item * item,
    std::string & error) const
{
    // Prepare the list of occupied body parts.
    std::vector<std::shared_ptr<BodyPart>> occupiedBodyParts;
    // Get the compatible body parts.
    auto compatibleBodyParts = item->model->getBodyParts(race);
    if (compatibleBodyParts.empty())
    {
        error = "It is not designed for your fisionomy.";
        return occupiedBodyParts;
    }
    for (auto bodyPart : compatibleBodyParts)
    {
        if (!HasFlag(bodyPart->flags, BodyPartFlag::CanWear))
        {
            continue;
        }
        if (this->findItemAtBodyPart(bodyPart) != nullptr)
        {
            continue;
        }
        occupiedBodyParts.emplace_back(bodyPart);
        break;
    }
    if (occupiedBodyParts.empty())
    {
        error = "There is no room where it can be worn.";
    }
    return occupiedBodyParts;
}

bool Character::inventoryIsLit() const
{
    for (auto it : inventory)
    {
        if (it->getType() == ModelType::Light)
        {
            if (static_cast<LightItem *>(it)->isActive())
            {
                return true;
            }
        }
    }
    return false;
}

void Character::addThirst(const int & value)
{
    thirst += value;
    if (thirst < 0) thirst = 0;
    else if (thirst > 100) thirst = 100;
}

std::string Character::getThirstCondition() const
{
    // Determine the correct description.
    if (thirst >= 90) return "are not thirsty";
    else if (thirst >= 60) return "are quite thirsty";
    else if (thirst >= 30) return "are thirsty";
    else return "are dying of thirst";
}

void Character::addHunger(const int & value)
{
    hunger += value;
    if (hunger < 0) hunger = 0;
    else if (hunger > 100) hunger = 100;
}

std::string Character::getHungerCondition() const
{
    // Determine the correct description.
    if (hunger >= 90) return "are not hungry";
    else if (hunger >= 60) return "are quite hungry";
    else if (hunger >= 30) return "are hungry";
    else return "are dying of hunger";
}

void Character::updateHealth()
{
    auto logModifier = this->getAbilityLog(Ability::Constitution);
    // Set the modifier due to the posture.
    unsigned int posModifier = 0;
    if (posture == CharacterPosture::Sit) posModifier = 2;
    else if (posture == CharacterPosture::Rest) posModifier = 4;
    else if (posture == CharacterPosture::Rest) posModifier = 8;
    if (this->health < this->getMaxHealth())
    {
        this->addHealth((1 + 3 * logModifier) * (1 + 2 * posModifier), true);
    }
}

void Character::updateStamina()
{
    auto logModifier = this->getAbilityLog(Ability::Constitution);
    // Set the modifier due to the posture.
    unsigned int posModifier = 0;
    if (posture == CharacterPosture::Sit) posModifier = 3;
    else if (posture == CharacterPosture::Rest) posModifier = 5;
    else if (posture == CharacterPosture::Rest) posModifier = 10;
    if (stamina < this->getMaxStamina())
    {
        this->addStamina((1 + 4 * logModifier) * (1 + 3 * posModifier), true);
    }
}

void Character::updateHunger()
{
    hunger = ((hunger - 1) < 0) ? 0 : hunger - 1;
}

void Character::updateThirst()
{
    thirst = ((thirst - 1) < 0) ? 0 : thirst - 1;
}

void Character::updateExpiredEffects()
{
    std::vector<std::string> messages;
    if (this->effects.effectUpdate(messages))
    {
        for (auto message : messages)
        {
            this->sendMsg(message + "\n");
        }
    }
}

void Character::updateActivatedEffects()
{
    std::vector<std::string> messages;
    if (this->effects.effectActivate(messages))
    {
        for (auto message : messages)
        {
            this->sendMsg(message + "\n");
        }
    }
}

std::string Character::getLook()
{
    // Check if the room is lit.
    bool roomIsLit = false;
    if (this->room != nullptr)
    {
        roomIsLit = this->room->isLit();
    }
    std::string output;
    output += "You look at " + this->getName() + ".\n";
    // Add the condition.
    output += ToCapitals(this->getSubjectPronoun());
    output += " " + this->getHealthCondition() + ".\n";
    // Add the description.
    output += description + "\n";

    for (auto bodyPart : this->race->bodyParts)
    {
#if 0 // Tabular version
        // Add the body part name to the row.
        output += AlignString(bodyPart->getDescription(true),
                              StringAlign::Left, 15);
        auto item = this->findItemAtBodyPart(bodyPart);
        if (item != nullptr)
        {
            if (roomIsLit)
            {
                output += " - " + item->getNameCapital(true);
            }
            else
            {
                output += " - Something";
            }
        }
        output += "\n";
#else
        auto item = this->findItemAtBodyPart(bodyPart);
        if (item == nullptr)
        {
            continue;
        }
        output += ToCapitals(this->getSubjectPronoun()) + " is ";
        if (HasFlag(bodyPart->flags, BodyPartFlag::CanWear))
        {
            output += "wearing ";
        }
        else
        {
            output += "wielding ";
        }
        if (roomIsLit)
        {
            output += item->getName(true) + " ";
        }
        else
        {
            output += "something ";
        }
        if (HasFlag(bodyPart->flags, BodyPartFlag::CanWear))
        {
            output += "on ";
        }
        else
        {
            output += "with ";
        }
        output += this->getPossessivePronoun() + " ";
        output += Formatter::yellow();
        output += bodyPart->getDescription(false);
        output += Formatter::reset() + ". ";
#endif
    }
    return output;
}

bool Character::canSee(Character * target) const
{
    return !HasFlag(target->flags, CharacterFlag::Invisible) ||
           HasFlag(flags, CharacterFlag::IsGod);
}

unsigned int Character::getArmorClass() const
{
    unsigned int result = 10;
    // + AGILITY MODIFIER
    result += this->getAbilityModifier(Ability::Agility);
    // + ARMOR BONUS
    for (auto item : equipment)
    {
        if (item->model->getType() == ModelType::Armor)
        {
            // Cast the item to armor.
            result += static_cast<ArmorItem *>(item)->getArmorClass();
        }
    }
    return result;
}

bool Character::canAttackWith(const std::shared_ptr<BodyPart> & bodyPart) const
{
    if (HasFlag(bodyPart->flags, BodyPartFlag::CanWield))
    {
        auto wpn = this->findItemAtBodyPart(bodyPart);
        if (wpn != nullptr)
        {
            // Check if there is actually a weapon equipped.
            if ((wpn->model->getType() == ModelType::MeleeWeapon) ||
                (wpn->model->getType() == ModelType::RangedWeapon))
            {
                return true;
            }
        }
    }
    return false;
}

bool Character::isAtRange(Character * target, const int & range)
{
    if (WrongAssert(this->room == nullptr)) return false;
    if (WrongAssert(this->room->area == nullptr)) return false;
    if (WrongAssert(target == nullptr)) return false;
    if (WrongAssert(target->room == nullptr)) return false;
    if (WrongAssert(target->room->area == nullptr)) return false;
    if (target->isMobile())
    {
        if (WrongAssert(!target->toMobile()->isAlive())) return false;
    }
    return this->room->area->los(this->room->coord, target->room->coord, range);
}

std::vector<MeleeWeaponItem *> Character::getActiveMeleeWeapons()
{
    std::vector<MeleeWeaponItem *> weapons;
    for (auto item : equipment)
    {
        // If at least one of the occupied body parts can be used to wield
        // a weapon, consider it an active weapon.
        for (auto bodyPart : item->occupiedBodyParts)
        {
            if (HasFlag(bodyPart->flags, BodyPartFlag::CanWield))
            {
                if (item->getType() == ModelType::MeleeWeapon)
                {
                    auto wpn = static_cast<MeleeWeaponItem *>(item);
                    weapons.emplace_back(wpn);
                    // Break the loop otherwise the weapon would be added
                    // for each occupied body part.
                    break;
                }
            }
        }
    }
    return weapons;
}

std::vector<RangedWeaponItem *> Character::getActiveRangedWeapons()
{
    std::vector<RangedWeaponItem *> weapons;
    for (auto item : equipment)
    {
        // If at least one of the occupied body parts can be used to wield
        // a range weapon, consider it an active weapon.
        for (auto bodyPart : item->occupiedBodyParts)
        {
            if (HasFlag(bodyPart->flags, BodyPartFlag::CanWield))
            {
                if (item->getType() == ModelType::RangedWeapon)
                {
                    auto wpn = static_cast<RangedWeaponItem *>(item);
                    weapons.emplace_back(wpn);
                    // Break the loop otherwise the weapon would be added
                    // for each occupied body part.
                    break;
                }
            }
        }
    }
    return weapons;
}

void Character::kill()
{
    // Create a corpse at the current position.
    auto corpse = this->createCorpse();
    // Transfer all the items from the character to the corpse.
    auto tempInventory = inventory;
    for (auto it = tempInventory.begin(); it != tempInventory.end(); ++it)
    {
        auto item = (*it);
        // Remove the item from the inventory.
        this->remInventoryItem(item);
        // Add the item to the corpse.
        corpse->putInside(item);
    }
    auto tempEquipment = equipment;
    for (auto it = tempEquipment.begin(); it != tempEquipment.end(); ++it)
    {
        auto item = (*it);
        // Remove the item from the inventory.
        this->remEquipmentItem(item);
        // Add the item to the corpse.
        corpse->putInside(item);
    }
    // Reset the action of the character.
    this->setAction(std::make_shared<GeneralAction>(this));
    // Reset the list of opponents.
    this->combatHandler.resetList();
    // Remove the character from the current room.
    if (room != nullptr)
    {
        room->removeCharacter(this);
    }
}

Item * Character::createCorpse()
{
    // Create the corpse.
    auto corpse = race->corpse->createCorpse(name, weight);
    // Add the corpse to the room.
    room->addItem(corpse);
    // Return the corpse.
    return corpse;
}

bool Character::doCommand(const std::string & command)
{
    ArgumentHandler argumentHandler(command);
    return inputProcessor->process(this, argumentHandler);
}

Player * Character::toPlayer()
{
    return static_cast<Player *>(this);
}

Mobile * Character::toMobile()
{
    return static_cast<Mobile *>(this);
}

void Character::loadScript(const std::string & scriptFilename)
{
    //Logger::log(LogLevel::Debug, "Loading script '%s'...", scriptFilename);
    // Open lua libraries.
    luaL_openlibs(L);

    // Register utilities functions.
    LuaRegisterUtils(L);

    // Register all the classes.
    Character::luaRegister(L);
    Area::luaRegister(L);
    Faction::luaRegister(L);
    ItemModel::luaRegister(L);
    Item::luaRegister(L);
    Material::luaRegister(L);
    Race::luaRegister(L);
    Coordinates::luaRegister(L);
    Exit::luaRegister(L);
    Terrain::luaRegister(L);
    Room::luaRegister(L);

    Direction::luaRegister(L);
    ModelType::luaRegister(L);
    ToolType::luaRegister(L);

    if (luaL_dofile(L, scriptFilename.c_str()) != LUABRIDGE_LUA_OK)
    {
        Logger::log(LogLevel::Error, "Can't open script %s.", scriptFilename);
        Logger::log(LogLevel::Error,
                    "Error :%s",
                    std::string(lua_tostring(L, -1)));
    }
}

luabridge::LuaRef Character::luaGetEquipmentItems()
{
    luabridge::LuaRef luaRef(L, luabridge::newTable(L));
    for (auto item : equipment)
    {
        luaRef.append(item);
    }
    return luaRef;
}

luabridge::LuaRef Character::luaGetInventoryItems()
{
    luabridge::LuaRef luaRef(L, luabridge::newTable(L));
    for (auto item : inventory)
    {
        luaRef.append(item);
    }
    return luaRef;
}

luabridge::LuaRef Character::luaGetRoomsInSight()
{
    luabridge::LuaRef luaRef(L, luabridge::newTable(L));
    if (room != nullptr)
    {
        CharacterContainer characterContainer;
        auto validCoordinates = room->area->fov(room->coord,
                                                this->getViewDistance());
        for (auto coordinates : validCoordinates)
        {
            luaRef.append(room->area->getRoom(coordinates));
        }
    }
    return luaRef;
}

luabridge::LuaRef Character::luaGetCharactersInSight()
{
    luabridge::LuaRef luaRef(L, luabridge::newTable(L));
    if (room != nullptr)
    {
        CharacterContainer exceptions;
        exceptions.emplace_back_character(this);
        for (auto it : room->area->getCharactersInSight(exceptions,
                                                        room->coord,
                                                        this->getViewDistance()))
        {
            luaRef.append(it);
        }
    }
    return luaRef;
}

luabridge::LuaRef Character::luaGetItemsInSight()
{
    luabridge::LuaRef luaRef(L, luabridge::newTable(L));
    if (room != nullptr)
    {
        ItemVector exceptions;
        for (auto it : room->area->getItemsInSight(exceptions,
                                                   room->coord,
                                                   this->getViewDistance()))
        {
            luaRef.append(it);
        }
    }
    return luaRef;
}

luabridge::LuaRef Character::luaGetPathTo(Room * destination)
{
    auto checkFunction = [&](Room * from, Room * to)
    {
        // Get the direction.
        auto direction = Area::getDirection(from->coord, to->coord);
        // Get the exit;
        auto destExit = from->findExit(direction);
        // If the direction is upstairs, check if there is a stair.
        if (direction == Direction::Up)
        {
            if (!HasFlag(destExit->flags, ExitFlag::Stairs)) return false;
        }
        // Check if the destination is correct.
        if (destExit->destination == nullptr) return false;
        // Check if the destination is bocked by a door.
        auto door = destExit->destination->findDoor();
        if (door != nullptr)
        {
            if (HasFlag(door->flags, ItemFlag::Closed)) return false;
        }
        // Check if the destination has a floor.
        auto destDown = destExit->destination->findExit(Direction::Down);
        if (destDown != nullptr)
        {
            if (!HasFlag(destDown->flags, ExitFlag::Stairs)) return false;
        }
        // Check if the destination is forbidden for mobiles.
        return !(this->isMobile() && HasFlag(destExit->flags, ExitFlag::NoMob));
    };
    AStar<Room *> aStar;
    std::vector<Room *> path;

    luabridge::LuaRef luaRef(L, luabridge::LuaRef::newTable(L));
    if (this->room != nullptr)
    {
        if (aStar.findPath(this->room, destination, path, checkFunction))
        {
            Coordinates previous = this->room->coord;
            for (auto node : path)
            {
                luaRef.append(Area::getDirection(previous, node->coord));
                previous = node->coord;
            }
        }
    }
    return luaRef;
}

Item * Character::luaLoadItem(int vnumModel,
                              int vnumMaterial,
                              unsigned int qualityValue)
{
    auto model = Mud::instance().findItemModel(vnumModel);
    if (model == nullptr)
    {
        Logger::log(LogLevel::Error,
                    "Can't find model :" + ToString(vnumModel));
        return nullptr;
    }
    auto composition = Mud::instance().findMaterial(vnumMaterial);
    if (composition == nullptr)
    {
        Logger::log(LogLevel::Error,
                    "Can't find material :" + ToString(vnumMaterial));
        return nullptr;
    }
    ItemQuality quality = ItemQuality::Normal;
    if (ItemQuality::isValid(qualityValue))
    {
        quality = ItemQuality(qualityValue);
    }
    return model->createItem(this->getName(), composition, true, quality);
}

void Character::luaAddEquipment(Item * item)
{
    std::string error;
    auto occupiedBodyParts = this->canWear(item, error);
    if (occupiedBodyParts.empty())
    {
        Logger::log(LogLevel::Error,
                    "The mobile %s cannot equip %s.",
                    this->getName(),
                    item->getName());
        Logger::log(LogLevel::Error, "Error: %s", error);
        return;
    }
    else
    {
        item->setOccupiedBodyParts(occupiedBodyParts);
        this->addEquipmentItem(item);
    }
}

bool Character::luaRemEquipment(Item * item)
{
    return this->remEquipmentItem(item);
}

void Character::luaAddInventory(Item * item)
{
    this->addInventoryItem(item);
}

bool Character::luaRemInventory(Item * item)
{
    return this->remInventoryItem(item);
}

void Character::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L)
        .beginClass<Character>("Character")
        .addData("name", &Character::name)
        .addData("race", &Character::race)
        .addData("faction", &Character::faction)
        .addData("room", &Character::room)
        .addFunction("getName", &Character::getName)
        .addFunction("getNameCapital", &Character::getNameCapital)
        .addFunction("inventoryAdd", &Character::luaAddInventory)
        .addFunction("inventoryRem", &Character::luaRemInventory)
        .addFunction("equipmentAdd", &Character::luaAddEquipment)
        .addFunction("equipmentRem", &Character::luaRemEquipment)
        .addFunction("doCommand", &Character::doCommand)
        .addFunction("getEquipmentItems", &Character::luaGetEquipmentItems)
        .addFunction("getInventoryItems", &Character::luaGetInventoryItems)
        .addFunction("getRoomsInSight", &Character::luaGetRoomsInSight)
        .addFunction("getCharactersInSight",
                     &Character::luaGetCharactersInSight)
        .addFunction("getItemsInSight", &Character::luaGetItemsInSight)
        .addFunction("luaGetPathTo", &Character::luaGetPathTo)
        .addFunction("loadItem", &Character::luaLoadItem)
        .addFunction("isMobile", &Character::isMobile)
        .addFunction("isPlayer", &Character::isPlayer)
        .addFunction("toMobile", &Character::toMobile)
        .addFunction("toPlayer", &Character::toPlayer)
        .endClass()
        .deriveClass<Mobile, Character>("Mobile")
        .addData("id", &Mobile::id)
        .addData("spawnRoom", &Mobile::respawnRoom)
        .addData("shortdesc", &Mobile::shortdesc)
        .addData("staticdesc", &Mobile::staticdesc)
        .addData("message_buffer", &Mobile::message_buffer)
        .addData("controller", &Mobile::controller)
        .addFunction("isMobile", &Mobile::isMobile)
        .addFunction("isAlive", &Mobile::isAlive)
        .endClass()
        .deriveClass<Player, Character>("Player")
        .addData("age", &Player::age, false)
        .addData("experience", &Player::experience, false)
        .addData("prompt", &Player::prompt, false)
        .addData("rent_room", &Player::rent_room, false)
        .addData("remaining_points", &Player::remaining_points, false)
        .addData("rent_room", &Player::rent_room, false)
        .addFunction("setVariable", &Player::setLuaVariable)
        .addFunction("getVariable", &Player::getLuaVariable)
        .addFunction("removeVariable", &Player::removeLuaVariable)
        .addFunction("isPlayer", &Player::isPlayer)
        .endClass();
}

bool Character::operator<(const class Character & source) const
{
    return name < source.name;
}

bool Character::operator==(const class Character & source) const
{
    return name == source.name;
}

void Character::sendMsg(const std::string & msg)
{
    Logger::log(LogLevel::Error, "[SEND_MESSAGE] Msg :" + msg);
}

void Character::updateTicImpl()
{
    this->updateHealth();
    this->updateStamina();
    this->updateHunger();
    this->updateThirst();
    this->updateExpiredEffects();
    this->updateActivatedEffects();
}

void Character::updateHourImpl()
{
    // Nothing to do.
}

std::string GetGenderTypeName(GenderType type)
{
    if (type == GenderType::Male) return "Male";
    if (type == GenderType::Female) return "Female";
    return "none";
}

std::string GetCharacterFlagName(CharacterFlag flag)
{
    if (flag == CharacterFlag::IsGod) return "IsGod";
    if (flag == CharacterFlag::IsGod) return "Invisible";
    return "None";
}

std::string GetCharacterFlagString(unsigned int flags)
{
    std::string flagList;
    if (HasFlag(flags, CharacterFlag::IsGod))
    { flagList += "|IsGod"; }
    if (HasFlag(flags, CharacterFlag::Invisible))
    { flagList += "|Invisible"; }
    flagList += "|";
    return flagList;
}
