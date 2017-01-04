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

#include "rangedWeaponItem.hpp"
#include "meleeWeaponItem.hpp"
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
    inputProcessor(std::make_shared<ProcessInput>())
{
    actionQueue.push_back(std::make_shared<GeneralAction>(this));
    // Nothing to do.
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
            ToString(effects.getAbilityModifier(Ability::Strength)) +
            "]"});
    sheet.addRow(
        {
            "Agility",
            ToString(this->getAbility(Ability::Agility, false)) +
            " [" +
            ToString(effects.getAbilityModifier(Ability::Agility)) +
            "]"});
    sheet.addRow(
        {
            "Perception",
            ToString(this->getAbility(Ability::Perception, false)) +
            " [" +
            ToString(effects.getAbilityModifier(Ability::Perception)) +
            "]"});
    sheet.addRow(
        {
            "Constitution",
            ToString(this->getAbility(Ability::Constitution, false)) +
            " [" +
            ToString(effects.getAbilityModifier(Ability::Constitution)) +
            "]"});
    sheet.addRow(
        {
            "Intelligence",
            ToString(this->getAbility(Ability::Intelligence, false)) +
            " [" +
            ToString(effects.getAbilityModifier(Ability::Intelligence)) +
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
    sheet.addRow({"Action", this->getAction()->getDescription()});
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
        desc += " " + posture.toString();
    }
    desc += " here";
    if (this->getAction()->getType() != ActionType::Wait)
    {
        desc += ", " + this->getSubjectPronoun() + " is ";
        desc += this->getAction()->getDescription();
    }
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

unsigned int
Character::getAbility(const Ability & ability, bool withEffects) const
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
        overall += effects.getAbilityModifier(ability);
        // Prune the value if exceed the boundaries.
        if (overall <= 0)
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

unsigned int
Character::getAbilityModifier(const Ability & ability, bool withEffects) const
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
    if (!withEffects)
    {
        return BASE;
    }
    int overall = static_cast<int>(BASE) + effects.getHealthMod();
    if (overall <= 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>(overall);
    }
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
    if (!withEffects)
    {
        return BASE;
    }
    int overall = static_cast<int>(BASE) + effects.getStaminaMod();
    if (overall <= 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>(overall);
    }
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

void Character::setAction(std::shared_ptr<GeneralAction> _action)
{
    if (_action->getType() != ActionType::Wait)
    {
        actionQueue.push_front(_action);
    }
}

std::shared_ptr<GeneralAction> Character::getAction() const
{
    return actionQueue.front();
}

void Character::popAction()
{
    if (actionQueue.front()->getType() != ActionType::Wait)
    {
        actionQueue.pop_front();
    }
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

Item * Character::findEquipmentSlotItem(EquipmentSlot slot) const
{
    for (auto iterator : equipment)
    {
        if (iterator->getCurrentSlot() == slot)
        {
            return iterator;
        }
    }
    return nullptr;
}

Item * Character::findEquipmentSlotTool(EquipmentSlot slot, ToolType type)
{
    auto tool = findEquipmentSlotItem(slot);
    if (tool != nullptr)
    {
        if (tool->model != nullptr)
        {
            if (tool->model->getType() == ModelType::Tool)
            {
                ToolModel * toolModel = tool->model->toTool();
                if (toolModel->toolType == type)
                {
                    return tool;
                }
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
    const std::vector<Item *> & exceptions,
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
                ToolModel * toolModel = iterator->model->toTool();
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
                ToolModel * toolModel = iterator->model->toTool();
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
                ToolModel * toolModel = iterator->model->toTool();
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
    std::vector<Item *> & foundOnes,
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

std::vector<Item *> Character::findCoins()
{
    ItemVector foundCoins;
    auto findCointInContainer = [&](Item * item)
    {
        if (item->isAContainer() && !item->isEmpty())
        {
            for (auto content : item->content)
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
        findCointInContainer(it);
    }
    for (auto it : inventory)
    {
        if (it->getType() == ModelType::Currency)
        {
            foundCoins.emplace_back(it);
        }
        else
        {
            findCointInContainer(it);
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
    equipment.push_back(item);
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
    // Value = 50 + (AbilMod(STR) * 10)
    // MIN   =  50
    // MAX   = 300
    return 50 + (this->getAbilityModifier(Ability::Strength) * 10);
}

bool Character::canWield(Item * item,
                         std::string & error,
                         EquipmentSlot & where) const
{
    // Gather the item in the right hand, if there is one.
    auto rightHand = findEquipmentSlotItem(EquipmentSlot::RightHand);
    // Gather the item in the left hand, if there is one.
    auto leftHand = findEquipmentSlotItem(EquipmentSlot::LeftHand);
    if (HasFlag(item->model->modelFlags, ModelFlag::TwoHand))
    {
        if ((rightHand != nullptr) || (leftHand != nullptr))
        {
            error = "You must have both your hand free to wield it.\n";
            return false;
        }
        where = EquipmentSlot::RightHand;
    }
    else
    {
        if ((rightHand != nullptr) && (leftHand != nullptr))
        {
            error = "You have both your hand occupied.\n";
            return false;
        }
        else if ((rightHand == nullptr) && (leftHand != nullptr))
        {
            if (HasFlag(leftHand->model->modelFlags, ModelFlag::TwoHand))
            {
                error = "You have both your hand occupied.\n";
                return false;
            }
            where = EquipmentSlot::RightHand;
        }
        else if ((rightHand != nullptr) && (leftHand == nullptr))
        {
            if (HasFlag(rightHand->model->modelFlags, ModelFlag::TwoHand))
            {
                error = "You have both your hand occupied.\n";
                return false;
            }
            where = EquipmentSlot::LeftHand;
        }
        else
        {
            where = EquipmentSlot::RightHand;
        }
    }
    return true;
}

bool Character::canWear(Item * item, std::string & error) const
{
    bool result = false;
    if (item->model->getType() == ModelType::Armor)
    {
        result = true;
    }
    else if (item->model->getType() == ModelType::Container)
    {
        if (item->getCurrentSlot() != EquipmentSlot::None)
        {
            result = true;
        }
    }
    if (!result)
    {
        error = "The item is not meant to be worn.\n";
        return false;
    }
    if (findEquipmentSlotItem(item->getCurrentSlot()) != nullptr)
    {
        error = "There is already something in that location.\n";
        return false;
    }
    return true;
}

bool Character::inventoryIsLit() const
{
    for (auto it : inventory)
    {
        if (it->getType() == ModelType::Light)
        {
            if (it->toLightItem()->isActive())
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
    unsigned int posModifier = 0;
    auto logModifier = this->getAbilityLog(Ability::Constitution);
    if (posture == CharacterPosture::Sit)
    {
        posModifier = 2;
    }
    else if (posture == CharacterPosture::Rest)
    {
        posModifier = 4;
    }
    if (this->health < this->getMaxHealth())
    {
        this->addHealth((1 + 3 * logModifier) * (1 + 2 * posModifier), true);
    }
}

void Character::updateStamina()
{
    unsigned int posModifier = 0;
    auto logModifier = this->getAbilityLog(Ability::Constitution);
    if (posture == CharacterPosture::Sit)
    {
        posModifier = 3;
    }
    else if (posture == CharacterPosture::Rest)
    {
        posModifier = 5;
    }
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
    std::string output;
    output += "You look at " + this->getName() + ".\n";
    // Add the condition.
    output += ToCapitals(this->getSubjectPronoun());
    output += " " + this->getHealthCondition() + ".\n";
    // Add the description.
    output += description + "\n";
    // Add what the target is wearing.
    if (equipment.empty())
    {
        output += Formatter::italic();
        output += ToCapitals(this->getSubjectPronoun());
        output += " is wearing nothing.\n";
        output += Formatter::reset();
        return output;
    }
    output += ToCapitals(this->getSubjectPronoun()) + " is wearing:\n";
    // Retrieve the equipment.
    auto head = this->findEquipmentSlotItem(EquipmentSlot::Head);
    auto back = this->findEquipmentSlotItem(EquipmentSlot::Back);
    auto torso = this->findEquipmentSlotItem(EquipmentSlot::Torso);
    auto legs = this->findEquipmentSlotItem(EquipmentSlot::Legs);
    auto feet = this->findEquipmentSlotItem(EquipmentSlot::Feet);
    auto right = this->findEquipmentSlotItem(EquipmentSlot::RightHand);
    auto left = this->findEquipmentSlotItem(EquipmentSlot::LeftHand);
    if (head != nullptr)
    {
        output += "\tHead       : " + head->getNameCapital(true) + "\n";
    }
    if (back != nullptr)
    {
        output += "\tBack       : " + back->getNameCapital(true) + "\n";
    }
    if (torso != nullptr)
    {
        output += "\tTorso      : " + torso->getNameCapital(true) + "\n";
    }
    if (legs != nullptr)
    {
        output += "\tLegs       : " + legs->getNameCapital(true) + "\n";
    }
    if (feet != nullptr)
    {
        output += "\tFeet       : " + feet->getNameCapital(true) + "\n";
    }
    if (right != nullptr)
    {
        if (HasFlag(right->model->modelFlags, ModelFlag::TwoHand))
        {
            output += "\tBoth Hands : ";
        }
        else
        {
            output += "\tRight Hand : ";
        }
        output += right->getNameCapital() + "\n";
    }
    if (left != nullptr)
    {
        output += "\tLeft Hand  : " + left->getNameCapital(true) + "\n";
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
            result += item->toArmorItem()->getArmorClass();
        }
    }
    return result;
}

bool Character::canAttackWith(const EquipmentSlot & slot) const
{
    if ((slot == EquipmentSlot::RightHand) || (slot == EquipmentSlot::LeftHand))
    {
        auto wpn = this->findEquipmentSlotItem(slot);
        if (wpn != nullptr)
        {
            // Check if there is actually a weapon equiped.
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
    std::vector<MeleeWeaponItem *> gatheredWeapons;
    auto RetrieveWeapon = [&](const EquipmentSlot & slot)
    {
        // First get the item at the given position.
        auto wpn = this->findEquipmentSlotItem(slot);
        if (wpn != nullptr)
        {
            if (wpn->getType() == ModelType::MeleeWeapon)
            {
                gatheredWeapons.emplace_back(wpn->toMeleeWeaponItem());
            }
        }
    };
    RetrieveWeapon(EquipmentSlot::RightHand);
    RetrieveWeapon(EquipmentSlot::LeftHand);
    return gatheredWeapons;
}

std::vector<RangedWeaponItem *> Character::getActiveRangedWeapons()
{
    std::vector<RangedWeaponItem *> gatheredWeapons;
    auto RetrieveWeapon = [&](const EquipmentSlot & slot)
    {
        // First get the item at the given position.
        auto wpn = this->findEquipmentSlotItem(slot);
        if (wpn != nullptr)
        {
            if (wpn->getType() == ModelType::RangedWeapon)
            {
                gatheredWeapons.emplace_back(wpn->toRangedWeaponItem());
            }
        }
    };
    RetrieveWeapon(EquipmentSlot::RightHand);
    RetrieveWeapon(EquipmentSlot::LeftHand);
    return gatheredWeapons;
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
    actionQueue.clear();
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
    auto corpse = race->corpse.createCorpse(name, race->material, weight);
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
    equipment.push_back(item);
}

bool Character::luaRemEquipment(Item * item)
{
    if (equipment.removeItem(item))
    {
        item->owner = nullptr;
        return true;
    }
    return false;
}

void Character::luaAddInventory(Item * item)
{
    equipment.push_back(item);
}

bool Character::luaRemInventory(Item * item)
{
    if (inventory.removeItem(item))
    {
        item->owner = nullptr;
        return true;
    }
    return false;
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
