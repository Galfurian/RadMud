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

#include "structureUtils.hpp"
#include "characterUtilities.hpp"
#include "formatter.hpp"
#include "lightItem.hpp"
#include "armorItem.hpp"
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
    L(luaL_newstate()),
    actionQueue(),
    actionQueueMutex(),
    inputProcessor(std::make_shared<ProcessInput>()),
    outbuffer(),
    effectManager(),
    skillManager(this),
    combatHandler(this)
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
    sheet.addColumn("Attribute", align::left);
    sheet.addColumn("Value", align::left);
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
            ToString(this->getAbility(Ability::Strength, false)) + " [" +
            ToString(effectManager.getAbilityMod(Ability::Strength)) + "][" +
            ToString(effectManager.getAbilityMod(Ability::Strength)) + "]"});
    sheet.addRow(
        {
            "Agility",
            ToString(this->getAbility(Ability::Agility, false)) + " [" +
            ToString(effectManager.getAbilityMod(Ability::Agility)) + "][" +
            ToString(effectManager.getAbilityMod(Ability::Agility)) + "]"});
    sheet.addRow(
        {
            "Perception",
            ToString(this->getAbility(Ability::Perception, false)) + " [" +
            ToString(effectManager.getAbilityMod(Ability::Perception)) + "][" +
            ToString(effectManager.getAbilityMod(Ability::Perception)) + "]"});
    sheet.addRow(
        {
            "Constitution",
            ToString(this->getAbility(Ability::Constitution, false)) + " [" +
            ToString(effectManager.getAbilityMod(Ability::Constitution)) +
            "][" +
            ToString(effectManager.getAbilityMod(Ability::Constitution)) +
            "]"});
    sheet.addRow(
        {
            "Intelligence",
            ToString(this->getAbility(Ability::Intelligence, false)) + " [" +
            ToString(effectManager.getAbilityMod(Ability::Intelligence)) +
            "][" +
            ToString(effectManager.getAbilityMod(Ability::Intelligence)) +
            "]"});
    sheet.addRow({"## Skill", "## Points"});
    for (const auto & skillData : skillManager.skills)
    {
        sheet.addRow({skillData->skill->name, ToString(skillData->skillLevel)});
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
    for (const auto & it : effectManager.getActiveEffects())
    {
        sheet.addRow({it.name, ToString(it.tremain)});
    }
}

void Character::initialize()
{
    // Nothing to do.
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
    if (this->getAction() != ActionType::Wait)
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
        overall += effectManager.getAbilityMod(ability);
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
    auto overall = static_cast<int>(BASE);
    overall += effectManager.getStatusMod(StatusModifier::Health);
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
    auto overall = static_cast<int>(BASE);
    overall += effectManager.getStatusMod(StatusModifier::Stamina);
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
    return 4 + static_cast<int>(this->getAbilityModifier(Ability::Perception));
}

void Character::pushAction(const std::shared_ptr<GeneralAction> & _action)
{
    std::lock_guard<std::mutex> lock(actionQueueMutex);
    actionQueue.push_front(_action);
}

void Character::popAction()
{
    std::lock_guard<std::mutex> lock(actionQueueMutex);
    if (!actionQueue.empty())
    {
        if (!actionQueue.front()->isLastAction())
        {
            actionQueue.pop_front();
        }
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

void Character::performAction()
{
    auto & action = this->getAction();
    if (action->isLastAction())
    {
        return;
    }
    if (action->checkElapsed())
    {
        auto status = action->perform();
        if ((status == ActionStatus::Finished) ||
            (status == ActionStatus::Error))
        {
            // Remove the from action.
            this->popAction();
        }
    }
}

void Character::resetActionQueue()
{
    std::lock_guard<std::mutex> lock(actionQueueMutex);
    // Clear the action queue.
    actionQueue.clear();
    // Add a general action and set it as the last action of the action queue.
    actionQueue.emplace_back(std::make_shared<GeneralAction>(this, true));
}

Item * Character::findNearbyItem(std::string const & key, int & number)
{
    auto item = ItemUtils::FindItemIn(inventory, key, number);
    if (item != nullptr) return item;
    item = ItemUtils::FindItemIn(equipment, key, number);
    if (item != nullptr) return item;
    if (room != nullptr) return room->findItem(key, number);
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

void Character::addInventoryItem(Item *& item)
{
    // Add the item to the inventory.
    inventory.emplace_back_item(item);
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
    equipment.emplace_back_item(item);
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
    auto sigHunger = static_cast<int>(hunger) + value;
    if (sigHunger < 0) hunger = 0;
    else if (sigHunger > 100) hunger = 100;
    else hunger = static_cast<unsigned int>(sigHunger);
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
    auto constMod(this->getAbilityModifier(Ability::Constitution));
    auto regainMod(posture.getRegainModifier());
    auto effectMod(
        static_cast<unsigned int>(
            effectManager.getStatusMod(StatusModifier::HealthRegeneration)));
    this->addHealth((constMod * regainMod) + effectMod, true);
}

void Character::updateStamina()
{
    auto constMod(this->getAbilityModifier(Ability::Constitution));
    auto regainMod(posture.getRegainModifier());
    auto effectMod(
        static_cast<unsigned int>(
            effectManager.getStatusMod(StatusModifier::HealthRegeneration)));
    this->addHealth(((2 * constMod) * regainMod) + effectMod, true);
}

void Character::updateHunger()
{
    hunger = (hunger > 0) ? hunger - 1 : 0;
}

void Character::updateThirst()
{
    thirst = (thirst > 0) ? thirst - 1 : 0;
}

void Character::updateExpiredEffects()
{
    effectManager.effectUpdate();
}

void Character::updateActivatedEffects()
{
    effectManager.effectActivate();
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
        output += Align(bodyPart->getDescription(true),
                              align::left, 15);
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
        output += bodyPart->getName(false);
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
    return StructUtils::los(room->coord,
                            target->room->coord,
                            room->area,
                            range,
                            race->height);
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
    outbuffer += msg;
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
