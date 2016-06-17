/// @file   character.cpp
/// @brief  Implements all the methods need to manipulate a character.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

// Basic Include.
#include "character.hpp"

#include <cstdlib>
#include <typeinfo>
#include <algorithm>
#include <list>
#include <sstream>

// Other Include.
#include "mud.hpp"
#include "logger.hpp"
#include "commands.hpp"
#include "constants.hpp"
#include "lua/lua_script.hpp"
#include "sqlite/SQLiteDbms.hpp"
#include "luabridge/LuaBridge.h"

using namespace std;

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
    strength(),
    agility(),
    perception(),
    constitution(),
    intelligence(),
    room(),
    inventory(),
    equipment(),
    posture(CharacterPosture::Stand),
    effects(),
    action(this),
    L(luaL_newstate()),
    opponents(this)
{
    // Nothing to do.
}

Character::~Character()
{
    // Nothing to do.
}

bool Character::setCharacteristic(const std::string & source)
{
    if (source.empty())
    {
        return false;
    }
    std::vector<std::string> charList = SplitString(source, ";");
    if (charList.size() != 5)
    {
        return false;
    }
    this->strength = ToNumber<unsigned int>(charList[0]);
    this->agility = ToNumber<unsigned int>(charList[1]);
    this->perception = ToNumber<unsigned int>(charList[2]);
    this->constitution = ToNumber<unsigned int>(charList[3]);
    this->intelligence = ToNumber<unsigned int>(charList[4]);
    return true;
}

bool Character::check()
{
    bool safe = true;
    safe = SafeAssert(!name.empty());
    safe = SafeAssert(!description.empty());
    safe = SafeAssert(weight > 0);
    safe = SafeAssert(race != nullptr);
    safe = SafeAssert(faction != nullptr);
    safe = SafeAssert(health > 0);
    safe = SafeAssert(stamina > 0);
    safe = SafeAssert(hunger > 0);
    safe = SafeAssert(thirst > 0);
    safe = SafeAssert(strength > 0);
    safe = SafeAssert(agility > 0);
    safe = SafeAssert(perception > 0);
    safe = SafeAssert(constitution > 0);
    safe = SafeAssert(intelligence > 0);
    safe = SafeAssert(thirst > 0);
    safe = SafeAssert(room != nullptr);
    safe = SafeAssert(L != nullptr);
    return safe;
}

bool Character::isMobile()
{
    return false;
}

bool Character::isPlayer()
{
    return false;
}

std::string Character::getName()
{
    if (this->isPlayer())
    {
        return ToLower(this->name);
    }
    else if (this->isMobile())
    {
        Mobile * mobile = this->toMobile();
        return ToLower(mobile->staticdesc);
    }
    else
    {
        return "No name";
    }
}

std::string Character::getNameCapital()
{
    if (this->isPlayer())
    {
        return ToCapitals(this->name);
    }
    else if (this->isMobile())
    {
        Mobile * mobile = this->toMobile();
        return mobile->staticdesc;
    }
    else
    {
        return "No name";
    }
}

std::string Character::getStaticDesc()
{
    // Name
    std::string desc(this->getNameCapital());
    // Posture
    desc += " is";
    if (posture != CharacterPosture::Stand)
    {
        desc += " " + GetPostureName(posture);
    }
    desc += " here";
    // Action
    if ((action.getType() != ActionType::NoAction) && (action.getType() != ActionType::Wait))
    {
        desc += ", " + this->getPronoun() + " is ";
        desc += action.getDescription();
    }
    desc += ".";
    return desc;
}

string Character::getPronoun()
{
    if (gender == GenderType::Male) return "he";
    if (gender == GenderType::Female) return "she";
    return "it";
}

string Character::getPossessivePronoun()
{
    if (gender == GenderType::Male) return "his";
    if (gender == GenderType::Female) return "her";
    return "its";
}

unsigned int Character::getStrength(bool withModifier)
{
    if (!withModifier)
    {
        return this->strength;
    }
    int overall = static_cast<int>(this->strength) + effects.getStrMod();
    if (overall <= 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>(overall);
    }
}

unsigned int Character::getAgility(bool withModifier)
{
    if (!withModifier)
    {
        return this->agility;
    }
    int overall = static_cast<int>(this->agility) + effects.getAgiMod();
    if (overall <= 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>(overall);
    }
}

unsigned int Character::getPerception(bool withModifier)
{
    if (!withModifier)
    {
        return this->perception;
    }
    int overall = static_cast<int>(this->perception) + effects.getPerMod();
    if (overall <= 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>(overall);
    }
}

unsigned int Character::getConstitution(bool withModifier)
{
    if (!withModifier)
    {
        return this->constitution;
    }
    int overall = static_cast<int>(this->constitution) + effects.getConMod();
    if (overall <= 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>(overall);
    }
}

unsigned int Character::getIntelligence(bool withModifier)
{
    if (!withModifier)
    {
        return this->intelligence;
    }
    int overall = static_cast<int>(this->intelligence) + effects.getIntMod();
    if (overall <= 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>(overall);
    }
}

unsigned int Character::getMaxHealth(bool withModifier)
{
    if (!withModifier)
    {
        return (constitution * 5);
    }
    int overall = static_cast<int>(constitution * 5) + effects.getHealthMod();
    if (overall <= 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>(overall);
    }
}

unsigned int Character::getMaxStamina(bool withModifier)
{
    if (!withModifier)
    {
        return (constitution * 10);
    }
    int overall = static_cast<int>(constitution * 10) + effects.getStaminaMod();
    if (overall <= 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>(overall);
    }
}

void Character::updateResources()
{
    if (health < getMaxHealth())
    {
        unsigned int maxValue = this->getMaxHealth();
        if (posture == CharacterPosture::Sit)
        {
            health += (maxValue / 100) * 4;
        }
        else if (posture == CharacterPosture::Rest)
        {
            health += (maxValue / 100) * 8;
        }
        else
        {
            health += (maxValue / 100) * 2;
        }
        if (health > maxValue)
        {
            health = maxValue;
        }
    }
    if (stamina < getMaxStamina())
    {
        unsigned int maxValue = this->getMaxStamina();
        if (posture == CharacterPosture::Sit)
        {
            stamina += (maxValue / 100) * 4;
        }
        else if (posture == CharacterPosture::Rest)
        {
            stamina += (maxValue / 100) * 8;
        }
        else
        {
            stamina += (maxValue / 100) * 2;
        }
        if (stamina > maxValue)
        {
            stamina = maxValue;
        }
    }
}

int Character::getViewDistance()
{
    return 3 + static_cast<int>(GetAbilityModifier(perception));
}

Action * Character::getAction()
{
    return &this->action;
}

Room * Character::canMoveTo(Direction direction, std::string & error)
{
    // Check if the player it's in a non-walking position.
    if (posture == CharacterPosture::Rest || posture == CharacterPosture::Sit)
    {
        error = "You first need to stand up.";
        return nullptr;
    }

    // Find the exit to the destination.
    std::shared_ptr<Exit> destExit = room->findExit(direction);
    if (destExit == nullptr)
    {
        error = "You cannot go that way.";
        return nullptr;
    }

    // If the direction is upstairs, check if there is a stair.
    if (direction == Direction::Up)
    {
        Logger::log(LogLevel::Debug, "Flags :" + ToString(destExit->flags));
        if (!HasFlag(destExit->flags, ExitFlag::Stairs))
        {
            error = "You can't go upstairs, there are no stairs.";
            return nullptr;
        }
    }

    // Check if the destination is correct.
    if (destExit->destination == nullptr)
    {
        error = "That direction can't take you anywhere.";
        return nullptr;
    }

    // Check if the destination is bocked by a door.
    Item * door = destExit->destination->findDoor();
    if (door != nullptr)
    {
        if (HasFlag(door->flags, ItemFlag::Closed))
        {
            error = "Maybe you have to open that door first.";
            return nullptr;
        }
    }

    // Check if the destination has a floor.
    std::shared_ptr<Exit> destDown = destExit->destination->findExit(Direction::Down);
    if (destDown != nullptr)
    {
        if (!HasFlag(destDown->flags, ExitFlag::Stairs))
        {
            error = "Do you really want to fall in that pit?";
            return nullptr;
        }
    }

    if (isMobile())
    {
        if (HasFlag(destExit->flags, ExitFlag::NoMob))
        {
            return nullptr;
        }
    }

    return destExit->destination;
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
        Logger::log(LogLevel::Error, "Character::moveTo: DESTINATION is a NULLPTR.");
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
    CharacterVector exceptions;
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

Item* Character::findInventoryItem(string search_parameter, int & number)
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

Item* Character::findEquipmentItem(string search_parameter, int & number)
{
    for (auto * iterator : equipment)
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

Item * Character::findEquipmentSlotItem(EquipmentSlot slot)
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
    Item * tool = findEquipmentSlotItem(slot);
    if (tool == nullptr)
    {
        return nullptr;
    }
    if (tool->model == nullptr)
    {
        return nullptr;
    }
    if (tool->model->type != ModelType::Tool)
    {
        return nullptr;
    }
    if (tool->model->getToolFunc().type != type)
    {
        return nullptr;
    }
    return tool;
}

Item * Character::findNearbyItem(std::string itemName, int & number)
{
    Item * item;
    if ((item = findInventoryItem(itemName, number)) == nullptr)
    {
        if ((item = findEquipmentItem(itemName, number)) == nullptr)
        {
            item = room->findItem(itemName, number);
        }
    }
    return item;
}

bool Character::findNearbyTools(ToolSet tools, ItemVector & foundOnes)
{
    for (auto iterator : tools)
    {
        bool found = false;
        for (auto iterator2 : equipment)
        {
            ToolType itemToolType = iterator2->model->getToolFunc().type;
            if (itemToolType == iterator)
            {
                foundOnes.push_back(iterator2);
                found = true;
                break;
            }
        }
        // If the ingredients are not enough, search even in the room.
        if (!found)
        {
            for (auto iterator2 : inventory)
            {
                ToolType itemToolType = iterator2->model->getToolFunc().type;
                if (itemToolType == iterator)
                {
                    foundOnes.push_back(iterator2);
                    found = true;
                    break;
                }
            }
        }
        // If the ingredients are still not enough, return false.
        if (!found)
        {
            return false;
        }
    }
    return true;
}

bool Character::findNearbyResouces(IngredientMap ingredients, ItemVector & foundOnes)
{
    for (auto iterator : ingredients)
    {
        // Quantity of ingredients that has to be found.
        unsigned int quantityNeeded = iterator.second;

        for (auto iterator2 : inventory)
        {
            ResourceType itemResourceType = iterator2->model->getResourceFunc().type;
            if (itemResourceType == iterator.first)
            {
                foundOnes.push_back(iterator2);
                quantityNeeded--;
                if (quantityNeeded == 0)
                {
                    break;
                }
            }
        }

        // If the ingredients are not enough, search even in the room.
        if (quantityNeeded > 0)
        {
            for (auto iterator2 : room->items)
            {
                ResourceType itemResourceType = iterator2->model->getResourceFunc().type;
                if (itemResourceType == iterator.first)
                {
                    foundOnes.push_back(iterator2);
                    quantityNeeded--;
                    if (quantityNeeded == 0)
                    {
                        break;
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

bool Character::hasInventoryItem(Item * item)
{
    return (std::find(inventory.begin(), inventory.end(), item) != inventory.end());
}

bool Character::hasEquipmentItem(Item * item)
{
    return (std::find(equipment.begin(), equipment.end(), item) != equipment.end());
}

bool Character::addInventoryItem(Item * item)
{
    if (item == nullptr)
    {
        Logger::log(LogLevel::Error, "[addInventoryItem] Item is a nullptr.");
        return false;
    }
    // Set the owner of the item.
    item->owner = this;
    // Add the item to the inventory.
    inventory.push_back(item);
    Logger::log(LogLevel::Debug, "Item '" + item->getName() + "' added to '" + this->getName() + "' inventory;");
    return true;
}

bool Character::remInventoryItem(Item *item)
{
    if (item == nullptr)
    {
        Logger::log(LogLevel::Error, "[remInventoryItem] Item is a nullptr.");
        return false;
    }
    else if (!FindErase(inventory, item))
    {
        Logger::log(LogLevel::Error, "[remInventoryItem] Error during item removal from inventory.");
        return false;
    }
    else
    {
        Logger::log(LogLevel::Debug, "Item '" + item->getName() + "' removed from '" + this->getName() + "';");
        item->owner = nullptr;
        return true;
    }
}

bool Character::canCarry(Item * item)
{
    return ((this->getCarryingWeight() + item->getWeight()) < this->getMaxCarryingWeight());
}

unsigned int Character::getCarryingWeight()
{
    unsigned int carrying = 0;
    for (auto iterator : inventory)
    {
        carrying += iterator->getWeight();
    }
    for (auto iterator : equipment)
    {
        carrying += iterator->getWeight();
    }
    return carrying;
}

unsigned int Character::getMaxCarryingWeight()
{
    return (strength * 10);
}

bool Character::addEquipmentItem(Item * item)
{
    // Check if the item exist.
    if (item == nullptr)
    {
        Logger::log(LogLevel::Error, "[addEquipmentItem] Item is a nullptr.");
        return false;
    }
    // Set the owner of the item.
    item->owner = this;
    // Add the item to the equipment.
    equipment.push_back(item);
    Logger::log(LogLevel::Debug, "Item '" + item->getName() + "' added to '" + this->getName() + "' equipment;");
    return true;
}

bool Character::remEquipmentItem(Item * item)
{
    // Check if the item exist.
    if (item == nullptr)
    {
        Logger::log(LogLevel::Error, "[remEquipmentItem] Item is a nullptr.");
        return false;
    }
    // Try to remove the item from the equipment.
    else if (!FindErase(equipment, item))
    {
        Logger::log(LogLevel::Error, "[remEquipmentItem] Error during item removal from equipment.");
        return false;
    }
    else
    {
        Logger::log(LogLevel::Debug, "Item '" + item->getName() + "' removed from '" + this->getName() + "';");
        item->owner = nullptr;
        return true;
    }
}

bool Character::canWield(Item * item, std::string & message, EquipmentSlot & where)
{
    // Gather the item in the right hand, if there is one.
    Item * rightHand = findEquipmentSlotItem(EquipmentSlot::RightHand);
    // Gather the item in the left hand, if there is one.
    Item * leftHand = findEquipmentSlotItem(EquipmentSlot::LeftHand);
    if (HasFlag(item->model->flags, ModelFlag::TwoHand))
    {
        if ((rightHand != nullptr) || (leftHand != nullptr))
        {
            message = "You must have both your hand free to wield it.\n";
            return false;
        }
        where = EquipmentSlot::RightHand;
    }
    else
    {
        if ((rightHand != nullptr) && (leftHand != nullptr))
        {
            message = "You have both your hand occupied.\n";
            return false;
        }
        else if ((rightHand == nullptr) && (leftHand != nullptr))
        {
            if (HasFlag(leftHand->model->flags, ModelFlag::TwoHand))
            {
                message = "You have both your hand occupied.\n";
                return false;
            }
            where = EquipmentSlot::RightHand;
        }
        else if ((rightHand != nullptr) && (leftHand == nullptr))
        {
            if (HasFlag(rightHand->model->flags, ModelFlag::TwoHand))
            {
                message = "You have both your hand occupied.\n";
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

bool Character::canWear(Item * item, std::string & message)
{
    bool result = false;
    if (item->model->type == ModelType::Armor)
    {
        result = true;
    }
    else if (item->model->type == ModelType::Container)
    {
        if (item->getCurrentSlot() != EquipmentSlot::None)
        {
            result = true;
        }
    }
    if (!result)
    {
        message = "The item is not meant to be weared.\n";
        return false;
    }
    if (findEquipmentSlotItem(item->getCurrentSlot()) != nullptr)
    {
        message = "There is already something in that location.\n";
        return false;
    }
    return true;
}

string Character::getHealthCondition(Character * character)
{
    string condition;
    string sent_be, sent_have;
    int percent;

    // Determine who is the examined character.
    if (character != nullptr && character != this)
    {
        sent_be = "is";
        sent_have = "has";
    }
    else
    {
        character = this;
        sent_be = "are";
        sent_have = "have";
    }

    // Determine the percentage of current health.
    if (character->health > 0)
    {
        percent = static_cast<int>((100.0 * static_cast<double>(character->health))
            / static_cast<double>(character->getMaxHealth()));
    }
    else
    {
        percent = -1;
    }

    // Determine the correct description.
    if (percent >= 100) condition = " " + sent_be + " in perfect health.\n";
    else if (percent >= 90) condition = " " + sent_be + " slightly scratched.\n";
    else if (percent >= 80) condition = " " + sent_have + " a few bruises.\n";
    else if (percent >= 70) condition = " " + sent_have + " some cuts.\n";
    else if (percent >= 60) condition = " " + sent_have + " several wounds.\n";
    else if (percent >= 50) condition = " " + sent_have + " many nasty wounds.\n";
    else if (percent >= 40) condition = " " + sent_be + " bleeding freely.\n";
    else if (percent >= 30) condition = " " + sent_be + " covered in blood.\n";
    else if (percent >= 20) condition = " " + sent_be + " leaking guts.\n";
    else if (percent >= 10) condition = " " + sent_be + " almost dead.\n";
    else condition = " " + sent_be + " DYING.\n";
    return condition;
}

std::string Character::getThirst(Character * character)
{
    std::string output, sent_be, sent_have;

    // Determine who is the examined character.
    if (character != nullptr && character != this)
    {
        sent_be = "is";
        sent_have = "has";
    }
    else
    {
        character = this;
        sent_be = "are";
        sent_have = "have";
    }
    int percent = static_cast<int>((100.0 * static_cast<double>(character->thirst)) / (100.0));

    // Determine the correct description.
    if (percent >= 90) output = sent_be + " not thirsty.\n";
    else if (percent >= 60) output = sent_be + " quite thirsty.\n";
    else if (percent >= 30) output = sent_be + " thirsty.\n";
    else output = sent_be + " dying of thirst.\n";
    return output;
}

std::string Character::getHunger(Character * character)
{
    std::string output, sent_be, sent_have;
    // Determine who is the examined character.
    if (character != nullptr && character != this)
    {
        sent_be = "is";
        sent_have = "has";
    }
    else
    {
        character = this;
        sent_be = "are";
        sent_have = "have";
    }
    int percent = static_cast<int>((100.0 * static_cast<double>(character->hunger)) / (100.0));
    // Determine the correct description.
    if (percent >= 90) output = sent_be + " not hungry.\n";
    else if (percent >= 60) output = sent_be + " quite hungry.\n";
    else if (percent >= 30) output = sent_be + " hungry.\n";
    else output = sent_be + " dying of hunger.\n";
    return output;
}

string Character::getLook(Character * character)
{
    string sent_be, sent_pronoun;
    string output;

    // Determine who is the examined character.
    if (character != nullptr && character != this)
    {
        output = "You look at " + character->getName() + ".\n";
        sent_be = "is";
        sent_pronoun = character->getPronoun();
    }
    else
    {
        character = this;
        output = "You give a look at yourself.\n";
        sent_be = "are";
        sent_pronoun = "you";
    }

    Item * head = character->findEquipmentSlotItem(EquipmentSlot::Head);
    Item * back = character->findEquipmentSlotItem(EquipmentSlot::Back);
    Item * torso = character->findEquipmentSlotItem(EquipmentSlot::Torso);
    Item * legs = character->findEquipmentSlotItem(EquipmentSlot::Legs);
    Item * feet = character->findEquipmentSlotItem(EquipmentSlot::Feet);
    Item * right = character->findEquipmentSlotItem(EquipmentSlot::RightHand);
    Item * left = character->findEquipmentSlotItem(EquipmentSlot::LeftHand);

    // Add the condition.
    output += ToCapitals(sent_pronoun) + getHealthCondition(character);
    // Add the description.
    output += character->description + "\n";
    // Add what the target is wearing.
    output += ToCapitals(sent_pronoun) + " " + sent_be + " wearing:\n";

    // Equipment Slot : HEAD
    output += "    " + Formatter::yellow() + "Head" + Formatter::reset() + "       : ";
    output += (head != nullptr) ? Formatter::cyan() + head->getNameCapital() : Formatter::gray() + "Nothing";
    output += Formatter::reset() + ".\n";
    // Equipment Slot : BACK
    output += "    " + Formatter::yellow() + "Back" + Formatter::reset() + "       : ";
    output += (back != nullptr) ? Formatter::cyan() + back->getNameCapital() : Formatter::gray() + "Nothing";
    output += Formatter::reset() + ".\n";
    // Equipment Slot : TORSO
    output += "    " + Formatter::yellow() + "Torso" + Formatter::reset() + "      : ";
    output += (torso != nullptr) ? Formatter::cyan() + torso->getNameCapital() : Formatter::gray() + "Nothing";
    output += Formatter::reset() + ".\n";
    // Equipment Slot : LEGS
    output += "    " + Formatter::yellow() + "Legs" + Formatter::reset() + "       : ";
    output += (legs != nullptr) ? Formatter::cyan() + legs->getNameCapital() : Formatter::gray() + "Nothing";
    output += Formatter::reset() + ".\n";
    // Equipment Slot : FEET
    output += "    " + Formatter::yellow() + "Feet" + Formatter::reset() + "       : ";
    output += (feet != nullptr) ? Formatter::cyan() + feet->getNameCapital() : Formatter::gray() + "Nothing";
    output += Formatter::reset() + ".\n";

    // Print what is wielding.
    if (right != nullptr)
    {
        if (HasFlag(right->model->flags, ModelFlag::TwoHand))
        {
            output += "    " + Formatter::yellow() + "Both Hands" + Formatter::reset() + " : ";
        }
        else
        {
            output += "    " + Formatter::yellow() + "Right Hand" + Formatter::reset() + " : ";
        }
        output += Formatter::cyan() + right->getNameCapital() + Formatter::reset() + ".\n";
    }
    else
    {
        output += "    " + Formatter::yellow() + "Right Hand" + Formatter::reset() + " : " + "Nothing";
    }
    output += ".\n";

    if (left != nullptr)
    {
        output += "    " + Formatter::yellow() + "Left Hand" + Formatter::reset() + "  : ";
        output += Formatter::cyan() + left->getNameCapital() + Formatter::reset() + ".\n";
    }
    return output;
}

bool Character::canSee(Character * target)
{
    if (HasFlag(target->flags, CharacterFlag::Invisible))
    {
        return false;
    }
    return true;
}

unsigned int Character::getArmorClass()
{
    // 10
    unsigned int result = 10;
    // + ARMOR BONUS
    for (auto it : equipment)
    {
        if (it->model->type == ModelType::Armor)
        {
            result += it->model->getArmorFunc().damageAbs;
        }
    }
    // + SHIELD BONUS
    Item * rh = this->findEquipmentSlotItem(EquipmentSlot::RightHand);
    if (rh != nullptr)
    {
        if (rh->model->type == ModelType::Shield)
        {
            result += rh->model->getShieldFunc().parryChance;
        }
    }
    Item * lh = this->findEquipmentSlotItem(EquipmentSlot::LeftHand);
    if (lh != nullptr)
    {
        if (lh->model->type == ModelType::Shield)
        {
            result += lh->model->getShieldFunc().parryChance;
        }
    }
    // + DEXTERITY MODIFIER
    int overallAgility = static_cast<int>(this->agility) + this->effects.getAgiMod();
    if (overallAgility > 0)
    {
        result += GetAbilityModifier(static_cast<unsigned int>(overallAgility));
    }
    return result;
}

unsigned int Character::getCooldown(CombatAction combatAction)
{
    double BASE = 6.0;
    // The agility modifier.
    double AGI = static_cast<double>(this->getAgility()) * 3.5 * (BASE / 100);
    // The weight modifier.
    double WGT = weight * (BASE / 100);
    // The carried weight.
    double CAR = (this->getCarryingWeight() / 2) * (BASE / 100);
    // The cooldown.
    unsigned int cooldown = 6;
    if (combatAction == CombatAction::BasicAttack)
    {
        double EQS = 0;
        Item * rh = findEquipmentSlotItem(EquipmentSlot::RightHand);
        if (rh != nullptr)
        {
            if (rh->model->type == ModelType::Weapon)
            {
                EQS += rh->getWeight() * 2.5 * (BASE / 100);
            }
            else if (rh->model->type == ModelType::Shield)
            {
                EQS += rh->getWeight() * 3.0 * (BASE / 100);
            }
        }
        Item * lh = findEquipmentSlotItem(EquipmentSlot::LeftHand);
        if (lh != nullptr)
        {
            if (lh->model->type == ModelType::Weapon)
            {
                EQS += lh->getWeight() * 2.5 * (BASE / 100);
            }
            else if (lh->model->type == ModelType::Shield)
            {
                EQS += lh->getWeight() * 3.0 * (BASE / 100);
            }
        }
        cooldown = static_cast<unsigned int>(BASE - AGI + WGT + CAR + EQS);
    }
    else if (combatAction == CombatAction::Flee)
    {
        cooldown = static_cast<unsigned int>(BASE - AGI + WGT + CAR);
    }
    return cooldown;
}

bool Character::canAttackWith(const EquipmentSlot & slot)
{
    if ((slot == EquipmentSlot::RightHand) || (slot == EquipmentSlot::LeftHand))
    {
        Item * weapon = this->findEquipmentSlotItem(slot);
        if (weapon != nullptr)
        {
            // Check if there is actually a weapon equiped.
            if (weapon->model->type == ModelType::Weapon)
            {
                return true;
            }
        }
    }
    return false;
}

bool Character::isAtRange(Character * target, const unsigned int & range)
{
    if (target == nullptr)
    {
        Logger::log(LogLevel::Error, "Received a null target.");
        return false;
    }
    if (this->room == nullptr)
    {
        Logger::log(LogLevel::Error, "Character has a null room.");
        return false;
    }
    if (this->room->area == nullptr)
    {
        Logger::log(LogLevel::Error, "Character's room has a null area.");
        return false;
    }
    if (target->room == nullptr)
    {
        Logger::log(LogLevel::Error, "Target has a null room.");
        return false;
    }
    if (target->room->area == nullptr)
    {
        Logger::log(LogLevel::Error, "Target's room has a null area.");
        return false;
    }
    if (this->room->area != target->room->area)
    {
        Logger::log(LogLevel::Error, "Character and Target are not in the same area.");
        return false;
    }
    return this->room->area->fastInSight(this->room->coord, target->room->coord, range);
}

Character * Character::getNextOpponentAtRange(const unsigned int & range)
{
    if (opponents.hasOpponents())
    {
        for (auto iterator : opponents.aggressionList)
        {
            if (this->isAtRange(iterator.aggressor, range))
            {
                return iterator.aggressor;
            }
        }
    }
    return nullptr;
}

ItemVector Character::getActiveWeapons()
{
    ItemVector ret;
    if (this->canAttackWith(EquipmentSlot::RightHand))
    {
        Item * weapon = this->findEquipmentSlotItem(EquipmentSlot::RightHand);
        if (weapon != nullptr)
        {
            ret.push_back(weapon);
        }
    }
    if (this->canAttackWith(EquipmentSlot::LeftHand))
    {
        Item * weapon = this->findEquipmentSlotItem(EquipmentSlot::LeftHand);
        if (weapon != nullptr)
        {
            ret.push_back(weapon);
        }
    }
    return ret;
}

bool Character::hasStaminaToAttackWith(const EquipmentSlot & slot, unsigned int & consumed)
{
    if (this->canAttackWith(slot))
    {
        double BASE = (5.0 / 100);
        // The strength modifier.
        double STR = BASE * static_cast<double>(this->getStrength());
        // The weight modifier.
        double WGT = BASE * weight;
        // The carried weight.
        double CAR = BASE * (this->getCarryingWeight() / 2);
        Item * item = findEquipmentSlotItem(EquipmentSlot::RightHand);
        if (item != nullptr)
        {
            double EQS = BASE * item->getWeight();
            Logger::log(LogLevel::Warning, "%s - %s + %s + %s", ToString(BASE), ToString(STR), ToString(CAR),
                ToString(EQS));
            BASE += -STR + WGT + CAR + EQS;
            if (BASE > 0)
            {
                consumed = static_cast<unsigned int>(BASE);
                if (this->stamina >= consumed)
                {
                    return true;
                }
            }
            else
            {
                Logger::log(LogLevel::Warning, "Evaluated cosumed stamina is below zero(%s).", ToString(BASE));
            }
        }
        else
        {
            Logger::log(LogLevel::Warning, "No item at the given position.");
        }
    }
    return false;
}

bool Character::dealDamage(const unsigned int & value, const bool & force)
{
    if (this->health >= value)
    {
        this->health -= value;
        return true;
    }
    if (force)
    {
        this->health = 0;
    }
    return false;
}

bool Character::healDamage(const unsigned int & value, const bool & force)
{
    unsigned int maximum = this->getMaxHealth();
    if ((this->health + value) <= maximum)
    {
        this->health += value;
        return true;
    }
    if (force)
    {
        this->health = maximum;
    }
    return false;
}

bool Character::consumeStamina(const unsigned int & value, const bool & force)
{
    if (this->stamina >= value)
    {
        this->stamina -= value;
        return true;
    }
    if (force)
    {
        this->stamina = 0;
    }
    return false;
}

bool Character::gainStamina(const unsigned int & value, const bool & force)
{
    unsigned int maximum = this->getMaxStamina();
    if ((this->stamina + value) <= maximum)
    {
        this->stamina += value;
        return true;
    }
    if (force)
    {
        this->stamina = maximum;
    }
    return false;
}

void Character::triggerDeath()
{
    // Create a corpse at the current position.
    this->createCorpse();
    // Reset the action of the character.
    this->getAction()->reset();
    // Reset the list of opponents.
    this->opponents.aggressionList.clear();
    // Remove the character from the current room.
    if (room != nullptr)
    {
        room->removeCharacter(this);
    }
}

Item * Character::createCorpse()
{
    Model * model = Mud::instance().findModel(1);
    if (model == nullptr)
    {
        Logger::log(LogLevel::Error, "Can't find the model of CORPSE.");
        return nullptr;
    }

    // Set the item.
    Item * corpse = new Item();
    corpse->vnum = Mud::instance().getMinVnumCorpse() - 1;
    corpse->model = new Model(*model);
    corpse->maker = "Death";
    corpse->condition = 120;
    corpse->composition = race->material;
    corpse->quality = ItemQuality::Normal;
    corpse->flags = 0;
    corpse->room = this->room;
    corpse->owner = nullptr;
    corpse->container = nullptr;
    corpse->currentSlot = EquipmentSlot::None;
    corpse->content = std::vector<Item *>();
    corpse->contentLiq = LiquidContent();

    // Set properly the corpse.
    corpse->model->vnum = 1;
    corpse->model->name = "corpse";
    corpse->model->article = "a";
    corpse->model->shortdesc = "the corpse of a " + ToLower(race->name); // TODO :FiX RACE NAME.
    std::vector<std::string> newKeys = GetWords(race->name);
    corpse->model->keys.insert(corpse->model->keys.end(), newKeys.begin(), newKeys.end());
    corpse->model->description = race->corpseDescription;
    corpse->model->type = ModelType::Corpse;
    corpse->model->slot = EquipmentSlot::None;
    corpse->model->flags = 0;
    corpse->model->weight = this->weight;
    corpse->model->price = 0;
    corpse->model->condition = 120;
    corpse->model->decay = 1;
    corpse->model->material = race->material->type;
    //corpse->model->tileSet = ; // TODO : Race specific corpse.
    //corpse->model->tileId = ; // TODO : Race specific corpse.
    //corpse->model->functions = ; // TODO : Race specific functions.

    // Add the corpse to the room.
    room->addItem(corpse);

    // Add the corpse to the mud.
    Mud::instance().addCorpse(corpse);

    // Return the newly created corpse.
    return corpse;
}

void Character::doCommand(const string & command)
{
    std::istringstream is(command);
    ProcessCommand(this, is);
}

Player * Character::toPlayer()
{
    return static_cast<Player *>(this);
}

Mobile * Character::toMobile()
{
    return static_cast<Mobile *>(this);
}

VectorHelper<Character *> Character::luaGetTargets()
{
    if (room != nullptr)
    {
        return VectorHelper<Character *>(room->characters);
    }
    return VectorHelper<Character *>();
}

void Character::loadScript(const std::string & scriptFilename)
{
    Logger::log(LogLevel::Debug, "Loading script '%s'...", scriptFilename);

    // Open lua libraries.
    luaL_openlibs(L);

    // Register utilities functions.
    LuaRegisterUtils(L);

    // Register all the classes.
    Character::luaRegister(L);
    Area::luaRegister(L);
    Faction::luaRegister(L);
    Model::luaRegister(L);
    Item::luaRegister(L);
    Material::luaRegister(L);
    Race::luaRegister(L);
    Coordinates<int>::luaRegister(L);
    Exit::luaRegister(L);
    Room::luaRegister(L);

    if (luaL_dofile(L, scriptFilename.c_str()) != LUABRIDGE_LUA_OK)
    {
        Logger::log(LogLevel::Error, "Can't open script+" + scriptFilename + ".");
        Logger::log(LogLevel::Error, "Error :" + std::string(lua_tostring(L, -1)));
    }
}

void Character::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass < Character > ("Character") //
    .addData("name", &Character::name) //
    .addData("race", &Character::race) //
    .addData("faction", &Character::faction) //
    .addData("room", &Character::room) //
    .addFunction("inventoryAdd", &Character::addInventoryItem) //
    .addFunction("inventoryRemove", &Character::remInventoryItem) //
    .addFunction("equipmentAdd", &Character::addEquipmentItem) //
    .addFunction("equipmentRemove", &Character::remEquipmentItem) //
    .addFunction("doCommand", &Character::doCommand) //
    .addFunction("getTargets", &Character::luaGetTargets) //
    .addFunction("isMobile", &Character::isMobile) //
    .endClass() //
    .deriveClass<Mobile, Character>("Mobile") //
    .addData("id", &Mobile::id) //
    .addData("spawnRoom", &Mobile::respawnRoom) //
    .addData("shortdesc", &Mobile::shortdesc) //
    .addData("staticdesc", &Mobile::staticdesc) //
    .addData("message_buffer", &Mobile::message_buffer) //
    .addData("alive", &Mobile::alive) //
    .addData("controller", &Mobile::controller) //
    .addFunction("isMobile", &Mobile::isMobile) //
    .endClass() //
    .deriveClass<Player, Character>("Player") //
    .addData("age", &Player::age, false) //
    .addData("experience", &Player::experience, false) //
    .addData("prompt", &Player::prompt, false) //
    .addData("rent_room", &Player::rent_room, false) //
    .addData("remaining_points", &Player::remaining_points, false) //
    .addData("rent_room", &Player::rent_room, false) //
    //std::map<int, unsigned int> skills;
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
