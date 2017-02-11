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

#include "roomUtilityFunctions.hpp"
#include "characterUtilities.hpp"
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
    skills(),
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
    // Initialize the action queue.
    this->resetActionQueue();
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
                effects.getAbilityModifier(Ability::Strength)) +
            "][" +
            ToString(
                effects.getAbilityModifier(Ability::Strength)) +
            "]"});
    sheet.addRow(
        {
            "Agility",
            ToString(this->getAbility(Ability::Agility, false)) +
            " [" +
            ToString(
                effects.getAbilityModifier(Ability::Agility)) +
            "][" +
            ToString(
                effects.getAbilityModifier(Ability::Agility)) +
            "]"});
    sheet.addRow(
        {
            "Perception",
            ToString(this->getAbility(Ability::Perception, false)) +
            " [" +
            ToString(effects.getAbilityModifier(
                Ability::Perception)) +
            "][" +
            ToString(effects.getAbilityModifier(
                Ability::Perception)) +
            "]"});
    sheet.addRow(
        {
            "Constitution",
            ToString(this->getAbility(Ability::Constitution, false)) +
            " [" +
            ToString(effects.getAbilityModifier(
                Ability::Constitution)) +
            "][" +
            ToString(effects.getAbilityModifier(
                Ability::Constitution)) +
            "]"});
    sheet.addRow(
        {
            "Intelligence",
            ToString(this->getAbility(Ability::Intelligence, false)) +
            " [" +
            ToString(effects.getAbilityModifier(
                Ability::Intelligence)) +
            "][" +
            ToString(effects.getAbilityModifier(
                Ability::Intelligence)) +
            "]"});
    sheet.addRow({"## Skill", "## Points"});
    for (auto it : skills)
    {
        auto skill = Mud::instance().findSkill(it.first);
        if (skill)
        {
            sheet.addRow({skill->name, ToString(it.second)});
        }
    }
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

void Character::initialize()
{
    Skill::updateSkillEffects(this);
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
        overall += effects.getAbilityModifier(ability);
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
    overall += effects.getStatusModifier(StatusModifier::Health);
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
    overall += effects.getStatusModifier(StatusModifier::Stamina);
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

void Character::pushAction(const std::shared_ptr<GeneralAction> & _action)
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
    if (actionQueue.size() > 1)
    {
        actionQueue.pop_front();
    }
}

std::shared_ptr<GeneralAction> & Character::getAction()
{
    std::lock_guard<std::mutex> lock(actionQueueMutex);
    return actionQueue.front();
}

std::shared_ptr<GeneralAction> const & Character::getAction() const
{
    std::lock_guard<std::mutex> lock(actionQueueMutex);
    return actionQueue.front();
}

void Character::resetActionQueue()
{
    std::lock_guard<std::mutex> lock(actionQueueMutex);
    actionQueue.clear();
    actionQueue.emplace_back(std::make_shared<GeneralAction>(this));
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
    // Set the modifier due to effects.
    auto effModifier = static_cast<unsigned int>(
        effects.getStatusModifier(StatusModifier::HealthRegeneration));
    // Add the health.
    this->addHealth((1 + 3 * logModifier) *
                    (1 + 2 * posModifier) +
                    (1 + 2 * effModifier), true);
}

void Character::updateStamina()
{
    auto logModifier = this->getAbilityLog(Ability::Constitution);
    // Set the modifier due to the posture.
    unsigned int posModifier = 0;
    if (posture == CharacterPosture::Sit) posModifier = 3;
    else if (posture == CharacterPosture::Rest) posModifier = 5;
    else if (posture == CharacterPosture::Rest) posModifier = 10;
    // Set the modifier due to effects.
    auto effModifier = static_cast<unsigned int>(
        effects.getStatusModifier(StatusModifier::StaminaRegeneration));
    // Add the stamina.
    this->addHealth((1 + 4 * logModifier) *
                    (1 + 3 * posModifier) +
                    (1 + 2 * effModifier), true);
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
    this->resetActionQueue();
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
        CharacterVector characterContainer;
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
        CharacterVector exceptions;
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
    auto RoomCheckFunction = [&](Room * from, Room * to)
    {
        // Preapre the options.
        MovementOptions options;
        options.character = this;
        // Prepare the error string.
        std::string error;
        return CheckConnection(options, from, to, error);
    };
    // TODO: Fix with new AStar algorithm.
    luabridge::LuaRef luaRef(L, luabridge::LuaRef::newTable(L));
    // Find the path from the actor to the target.
    AStar<Room *> aStar(RoomCheckFunction,
                        RoomGetDistance,
                        RoomAreEqual,
                        RoomGetNeighbours);
    std::vector<Room *> path;
    if (this->room != nullptr)
    {
        if (aStar.findPath(this->room, destination, path))
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
