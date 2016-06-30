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

bool Character::check()
{
    bool safe = true;
    safe &= SafeAssert(!name.empty());
    safe &= SafeAssert(!description.empty());
    safe &= SafeAssert(weight > 0);
    safe &= SafeAssert(race != nullptr);
    safe &= SafeAssert(faction != nullptr);
    safe &= SafeAssert(health > 0);
    safe &= SafeAssert(stamina > 0);
    safe &= SafeAssert(hunger > 0);
    safe &= SafeAssert(thirst > 0);
    safe &= SafeAssert(abilities[Ability::Strength] > 0);
    safe &= SafeAssert(abilities[Ability::Strength] <= 60);
    safe &= SafeAssert(abilities[Ability::Agility] > 0);
    safe &= SafeAssert(abilities[Ability::Agility] <= 60);
    safe &= SafeAssert(abilities[Ability::Perception] > 0);
    safe &= SafeAssert(abilities[Ability::Perception] <= 60);
    safe &= SafeAssert(abilities[Ability::Constitution] > 0);
    safe &= SafeAssert(abilities[Ability::Constitution] <= 60);
    safe &= SafeAssert(abilities[Ability::Intelligence] > 0);
    safe &= SafeAssert(abilities[Ability::Intelligence] <= 60);
    safe &= SafeAssert(thirst > 0);
    safe &= SafeAssert(room != nullptr);
    safe &= SafeAssert(L != nullptr);
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

bool Character::setAbility(const Ability & ability, const unsigned int & value)
{
    if (value <= 60)
    {
        abilities[ability] = value;
        return true;
    }
    else
    {
        return false;
    }
}

unsigned int Character::getAbility(const Ability & ability, bool withEffects)
{
    if (AbilityTest::is_value(ability))
    {
        if (!withEffects)
        {
            return abilities[ability];
        }
        int overall = static_cast<int>(this->abilities[ability]) + effects.getAbilityModifier(ability);
        if (overall <= 0)
        {
            return 0;
        }
        else if (overall > 60)
        {
            return 60;
        }
        else
        {
            return static_cast<unsigned int>(overall);
        }
    }
    else
    {
        return 0;
    }
}

unsigned int Character::getAbilityModifier(const Ability & ability, bool withEffects)
{
    return GetAbilityModifier(this->getAbility(ability, withEffects));
}

unsigned int Character::getAbilityLog(
    const Ability & ability,
    const double & base,
    const double & multiplier,
    const bool & withEffects)
{
    double result = base;
    double modifier = static_cast<double>(this->getAbilityModifier(ability, withEffects));
    if (modifier > 0)
    {
        if (modifier > 25)
        {
            modifier = 25;
        }
        result += multiplier * log10(modifier);
    }
    return static_cast<unsigned int>(result);
}

unsigned int Character::getMaxHealth(bool withEffects)
{
    // Value = 100 + (10 * AbilityModifier(Constitution))
    // MIN   = 100
    // MAX   = 350
    unsigned int BASE = 100 + (10 * this->getAbilityModifier(Ability::Constitution));
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

unsigned int Character::getMaxStamina(bool withEffects)
{
    // Value = 100 + (15 * AbilityModifier(Constitution))
    // MIN   = 100
    // MAX   = 475
    unsigned int BASE = 100 + (15 * this->getAbilityModifier(Ability::Constitution));
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

void Character::updateResources()
{
    unsigned int PosMod = 0;
    if (posture == CharacterPosture::Sit)
    {
        PosMod = 2;
    }
    else if (posture == CharacterPosture::Rest)
    {
        PosMod = 4;
    }
    if (this->health < this->getMaxHealth())
    {
        unsigned int LogMod = this->getAbilityLog(Ability::Constitution, 0.0, 1.0);
        // Value = 5 + ((1 + 3*LogMod(CON))*(1 + 2*PosMod))
        // MIN   =  6.00
        // MAX   = 51.74
        this->addHealth(5 + ((1 + 3 * LogMod) * (1 + 2 * PosMod)), true);
    }
    if (this->stamina < this->getMaxStamina())
    {
        unsigned int LogMod = this->getAbilityLog(Ability::Constitution, 0.0, 1.0);
        // Value = 5 + ((1 + 4*LogMod(CON))*(1 + 3*PosMod))
        // MIN   =  6.00
        // MAX   = 90.69
        this->addStamina(5 + ((1 + 4 * LogMod) * (1 + 3 * PosMod)), true);
    }
}

int Character::getViewDistance()
{
    // Value = 3 + LogMod(PER)
    // MIN   = 3.0
    // MAX   = 7.2
    return 3 + static_cast<int>(this->getAbilityLog(Ability::Perception, 0.0, 1.0));
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

    unsigned int consumedStamina;
    // Check if the actor has enough stamina to execute the action.
    if (!this->hasStaminaFor(consumedStamina, ActionType::Move))
    {
        error = "You are too tired to move.\n";
        Logger::log(
            LogLevel::Debug,
            "[%s] Has %s stamina and needs %s.",
            this->getName(),
            ToString(this->stamina),
            ToString(consumedStamina));
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
    return ((this->getCarryingWeight() + item->getTotalWeight()) < this->getMaxCarryingWeight());
}

unsigned int Character::getCarryingWeight()
{
    unsigned int carrying = 0;
    for (auto iterator : inventory)
    {
        carrying += iterator->getTotalWeight();
    }
    for (auto iterator : equipment)
    {
        carrying += iterator->getTotalWeight();
    }
    return carrying;
}

unsigned int Character::getMaxCarryingWeight()
{
    // Value = 50 + (AbilMod(STR) * 10)
    // MIN   =  50
    // MAX   = 300
    return 50 + (this->getAbilityModifier(Ability::Strength) * 10);
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

bool Character::setThirst(int value)
{
    int result = static_cast<int>(this->thirst) + value;
    if (result < 0)
    {
        return false;
    }
    else if (result > 100)
    {
        this->thirst = 100;
    }
    else
    {
        this->thirst = static_cast<unsigned int>(result);
    }
    return true;
}

bool Character::setHunger(int value)
{
    int result = static_cast<int>(this->hunger) + value;
    if (result < 0)
    {
        return false;
    }
    else if (result > 100)
    {
        this->hunger = 100;
    }
    else
    {
        this->hunger = static_cast<unsigned int>(result);
    }
    return true;
}

unsigned int Character::getThirst()
{
    return this->thirst;
}

unsigned int Character::getHunger()
{
    return this->hunger;
}

std::string Character::getThirstDesc()
{
    std::string output;
    int percent = static_cast<int>((100.0 * static_cast<double>(this->thirst)) / (100.0));
    // Determine the correct description.
    if (percent >= 90) output = "are not thirsty.\n";
    else if (percent >= 60) output = "are quite thirsty.\n";
    else if (percent >= 30) output = "are thirsty.\n";
    else output = "are dying of thirst.\n";
    return output;
}

std::string Character::getHungerDesc()
{
    std::string output;
    int percent = static_cast<int>((100.0 * static_cast<double>(this->hunger)) / (100.0));
    // Determine the correct description.
    if (percent >= 90) output = "are not hungry.\n";
    else if (percent >= 60) output = "are quite hungry.\n";
    else if (percent >= 30) output = "are hungry.\n";
    else output = "are dying of hunger.\n";
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
    // + AGILITY MODIFIER
    result += this->getAbilityModifier(Ability::Agility);
    return result;
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

unsigned int Character::getCooldown(CombatAction combatAction)
{
    double BASE = 5.0;
    // The strength modifier.
    // MIN = 0.00
    // MAX = 1.40
    double AGI = this->getAbilityLog(Ability::Agility, 0.0, 1.0);
    // The agility modifier.
    // MIN = 0.00
    // MAX = 1.40
    double STR = this->getAbilityLog(Ability::Strength, 0.0, 1.0);
    // The weight modifier.
    // MIN =  0.80
    // MAX =  2.51
    double WGT = 0.8;
    double WGTmod = this->weight;
    if (WGTmod > 0)
    {
        if (WGTmod > 320)
        {
            WGTmod = 320;
        }
        WGT = log10(WGTmod);
    }
    // The carried weight.
    // MIN =  0.00
    // MAX =  2.48
    double CAR = 0.0;
    double CARmod = this->getCarryingWeight();
    if (CARmod > 0)
    {
        if (CARmod > 300)
        {
            CARmod = 300;
        }
        CAR = log10(CARmod);
    }
    if (combatAction == CombatAction::BasicAttack)
    {
        double RHD = 0;
        double LHD = 0;
        if (this->canAttackWith(EquipmentSlot::RightHand))
        {
            // Right Hand Weapon
            // MIN =  0.00
            // MAX =  1.60
            Item * weapon = this->findEquipmentSlotItem(EquipmentSlot::RightHand);
            double RHDmod = weapon->getTotalWeight();
            if (RHDmod > 0)
            {
                if (RHDmod > 40)
                {
                    RHDmod = 40;
                }
                RHD = log10(RHDmod);
            }
        }
        if (this->canAttackWith(EquipmentSlot::LeftHand))
        {
            // Left Hand Weapon
            // MIN =  0.00
            // MAX =  1.60
            Item * weapon = this->findEquipmentSlotItem(EquipmentSlot::LeftHand);
            double LHDmod = weapon->getTotalWeight();
            if (LHDmod > 0)
            {
                if (LHDmod > 40)
                {
                    LHDmod = 40;
                }
                LHD = log10(LHDmod);
            }
        }
        BASE += -STR - AGI + WGT + CAR + max(RHD, LHD);
    }
    else if (combatAction == CombatAction::Flee)
    {
        BASE += -STR - AGI + WGT + CAR;
    }
    return static_cast<unsigned int>(BASE);
}

bool Character::hasStaminaFor(
    unsigned int & consumed,
    const ActionType & actionType,
    const CombatAction & combatAction,
    const EquipmentSlot & slot)
{
    // The strength modifier.
    // MIN = 0.0
    // MAX = 2.8
    double STR = this->getAbilityLog(Ability::Strength, 0.0, 2.0);
    // The weight modifier.
    // MIN = 0.80
    // MAX = 1.25
    double WGT = 0.8;
    double WGTmod = this->weight;
    if (WGTmod > 0)
    {
        if (WGTmod > 320)
        {
            WGTmod = 320;
        }
        WGT = (log10(WGTmod) / 2);
    }
    // The carried weight.
    // MIN = 0.00
    // MAX = 1.24
    double CAR = 0.0;
    double CARmod = this->getCarryingWeight();
    if (CARmod > 0)
    {
        if (CARmod > 300)
        {
            CARmod = 300;
        }
        CAR = (log10(CARmod) / 2);
    }
    if ((actionType == ActionType::Move) || (actionType == ActionType::Building)
        || (actionType == ActionType::Crafting))
    {
        double BASE = 3.0;
        double RSLT = BASE - STR + WGT + CAR;
        Logger::log(
            LogLevel::Warning,
            "Required Stamina : %s - %s + %s + %s = %s",
            ToString(BASE),
            ToString(STR),
            ToString(WGT),
            ToString(CAR),
            ToString(RSLT));
        if (RSLT > 0)
        {
            consumed = static_cast<unsigned int>(RSLT);
            if (this->stamina >= consumed)
            {
                return true;
            }
        }
        else
        {
            Logger::log(LogLevel::Warning, "Evaluated cosumed stamina is below zero(%s).", ToString(RSLT));
        }
    }
    else if (actionType == ActionType::Combat)
    {
        if (combatAction == CombatAction::BasicAttack)
        {
            if (this->canAttackWith(slot))
            {
                double WPN = 0;
                Item * weapon = this->findEquipmentSlotItem(slot);
                double WPNmod = weapon->getTotalWeight();
                if (WPNmod > 0)
                {
                    if (WPNmod > 40)
                    {
                        WPNmod = 40;
                    }
                    WPN = log10(WPNmod); // min:0.0; max:1.60
                }
                double BASE = 5.0;
                double RSLT = BASE - STR + WGT + CAR + WPN;
                Logger::log(
                    LogLevel::Warning,
                    "Required Stamina : %s - %s + %s + %s = %s",
                    ToString(BASE),
                    ToString(STR),
                    ToString(WGT),
                    ToString(CAR),
                    ToString(RSLT));
                if (RSLT > 0)
                {
                    consumed = static_cast<unsigned int>(RSLT);
                    if (this->stamina >= consumed)
                    {
                        return true;
                    }
                }
                else
                {
                    Logger::log(LogLevel::Warning, "Evaluated cosumed stamina is below zero(%s).", ToString(RSLT));
                }
            }
        }
        else if (combatAction == CombatAction::Flee)
        {
            double BASE = 3.0;
            double RSLT = BASE - STR + WGT + CAR;
            Logger::log(
                LogLevel::Warning,
                "Required Stamina : %s - %s + %s + %s = %s",
                ToString(BASE),
                ToString(STR),
                ToString(WGT),
                ToString(CAR),
                ToString(RSLT));
            if (RSLT > 0)
            {
                consumed = static_cast<unsigned int>(RSLT);
                if (this->stamina >= consumed)
                {
                    return true;
                }
            }
            else
            {
                Logger::log(LogLevel::Warning, "Evaluated cosumed stamina is below zero(%s).", ToString(RSLT));
            }
        }
    }
    return false;
}

bool Character::setHealth(const unsigned int & value, const bool & force)
{
    unsigned int maximum = this->getMaxHealth();
    if ((value > maximum) && (force))
    {
        if (force)
        {
            this->health = maximum;
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
            this->health = maximum;
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
            this->health = 0;
        }
        else
        {
            return false;
        }
    }
    this->health = static_cast<unsigned int>(result);
    return true;
}

unsigned int Character::getHealth()
{
    return this->health;
}

bool Character::setStamina(const unsigned int & value, const bool & force)
{
    unsigned int maximum = this->getMaxStamina();
    if (value > maximum)
    {
        if (force)
        {
            this->stamina = maximum;
        }
        else
        {
            return false;
        }
    }
    this->stamina = value;
    return true;
}

bool Character::addStamina(const unsigned int & value, const bool & force)
{
    unsigned int maximum = this->getMaxStamina();
    unsigned int result = this->stamina + value;
    if (result > maximum)
    {
        if (force)
        {
            this->stamina = maximum;
        }
        else
        {
            return false;
        }
    }
    this->stamina = result;
    return true;
}

bool Character::remStamina(const unsigned int & value, const bool & force)
{
    int result = static_cast<int>(this->stamina) - static_cast<int>(value);
    if (result < 0)
    {
        if (force)
        {
            this->stamina = 0;
        }
        else
        {
            return false;
        }
    }
    this->stamina = static_cast<unsigned int>(result);
    return true;
}

unsigned int Character::getStamina()
{
    return this->stamina;
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

void Character::createCorpse()
{
    // Retrieve the model of the corpse.
    Logger::log(LogLevel::Debug, "Retrieve the model of the corpse.");
    Model * corpseModel = &race->corpse;
    // Create a new corpse.
    Logger::log(LogLevel::Debug, "Create a new corpse.");
    Item * corpse = corpseModel->createItem(this->name, race->material, ItemQuality::Normal);
    // Set the weight of the new corpse.
    corpse->customWeight = this->weight;
    // Add the corpse to the room.
    Logger::log(LogLevel::Debug, "Add the corpse to the room.");
    room->addItem(corpse);
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
    .beginClass<Character>("Character") //
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
