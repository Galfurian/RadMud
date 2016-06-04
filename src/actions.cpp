/// @file   actions.cpp
/// @brief  Implements all the actions that a player can do and which takes more than one turn to conclude.
/// @author Enrico Fraccaroli
/// @date   Feb 28 2015
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
#include "actions.hpp"

// Other Include.
#include "mud.hpp"
#include "logger.hpp"
#include "constants.hpp"

Action::Action(Character * _character) :
        type(ActionType::Wait),
        actor(_character),
        target(),
        itemTarget(),
        destination(),
        direction(Direction::None),
        production(),
        schematics(),
        craftMaterial(),
        usedTools(),
        usedIngredients(),
        actionCooldown(),
        opponents(_character),
        nextCombatAction()
{
    // Nothing to do.
}

Action::~Action()
{
    // Nothing to do.
}

ActionType Action::getType()
{
    return this->type;
}

OpponentsList * Action::getOpponentsList()
{
    return &opponents;
}

std::string Action::getDescription()
{
    std::string msg;
    switch (this->type)
    {
        case ActionType::NoAction:
            case ActionType::Wait:
            break;
        case ActionType::Move:
            msg = "moving";
            break;
        case ActionType::Crafting:
            if (this->production != nullptr)
            {
                if (this->production->profession != nullptr)
                {
                    msg = this->production->profession->action;
                }
            }
            break;
        case ActionType::Building:
            msg = "building";
            break;
        case ActionType::Combat:
            msg = "fighting";
            break;
    }
    return msg;
}

std::string Action::stop()
{
    std::string msg;
    switch (this->type)
    {
        case ActionType::NoAction:
            msg = "No action set.";
            break;
        case ActionType::Wait:
            msg = "Excuse me sir, but you are doing nothing.";
            break;
        case ActionType::Move:
            msg = "You stop moving.";
            break;
        case ActionType::Crafting:
            if (this->production != nullptr)
            {
                if (this->production->profession != nullptr)
                {
                    msg = this->production->profession->interruptMessage + ".";
                }
            }
            break;
        case ActionType::Building:
            msg = "You stop building.";
            break;
        case ActionType::Combat:
            msg = "You stop fighting.";
            break;
    }
    this->reset();
    return msg;
}

void Action::perform()
{
    switch (this->type)
    {
        case ActionType::Move:
            this->performMove();
            break;
        case ActionType::Crafting:
            this->performCraft();
            break;
        case ActionType::Building:
            this->performBuild();
            break;
        case ActionType::Combat:
            this->performComb();
            break;
        case ActionType::NoAction:
            break;
        case ActionType::Wait:
            break;
    }
}

bool Action::setMove(Room * _destination, Direction _direction, unsigned int _cooldown)
{
    bool correct = true;
    if (_destination == nullptr)
    {
        Logger::log(LogLevel::Error, "No destination has been set.");
        correct = false;
    }
    if (_direction == Direction::None)
    {
        Logger::log(LogLevel::Error, "No direction has been set.");
        correct = false;
    }
    if (_cooldown == 0)
    {
        Logger::log(LogLevel::Error, "No cooldown has been set.");
        correct = false;
    }
    if (correct)
    {
        type = ActionType::Move;
        destination = _destination;
        direction = _direction;
        actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(_cooldown);
    }
    return correct;
}

bool Action::setCraft(
    Production * _production,
    std::vector<Item *> & _usedTools,
    std::vector<Item *> & _usedIngredients,
    Material * _craftMaterial,
    unsigned int _cooldown)
{
    bool correct = true;
    if (_production == nullptr)
    {
        Logger::log(LogLevel::Error, "No profession has been set.");
        correct = false;
    }
    if (_usedTools.empty())
    {
        Logger::log(LogLevel::Error, "No used tools have been set.");
        correct = false;
    }
    if (_usedIngredients.empty())
    {
        Logger::log(LogLevel::Error, "No used ingredients have been set.");
        correct = false;
    }
    if (_craftMaterial == nullptr)
    {
        Logger::log(LogLevel::Error, "No crafting material has been set.");
        correct = false;
    }
    if (_cooldown == 0)
    {
        Logger::log(LogLevel::Error, "No cooldown has been set.");
        correct = false;
    }
    if (correct)
    {
        type = ActionType::Crafting;
        production = _production;
        usedTools = _usedTools;
        usedIngredients = _usedIngredients;
        craftMaterial = _craftMaterial;
        actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(_cooldown);
    }
    return correct;
}

bool Action::setBuild(
    Building * _schematics,
    Item * _itemTarget,
    std::vector<Item *> & _usedTools,
    std::vector<Item *> & _usedIngredients,
    unsigned int _cooldown)
{
    bool correct = true;
    if (_schematics == nullptr)
    {
        Logger::log(LogLevel::Error, "No schematics have been set.");
        correct = false;
    }
    if (_itemTarget == nullptr)
    {
        Logger::log(LogLevel::Error, "No item is set for the building.");
        correct = false;
    }
    if (_usedTools.empty())
    {
        Logger::log(LogLevel::Error, "No used tools have been set.");
        correct = false;
    }
    if (_cooldown == 0)
    {
        Logger::log(LogLevel::Error, "No cooldown has been set.");
        correct = false;
    }
    if (correct)
    {
        type = ActionType::Building;
        schematics = _schematics;
        itemTarget = _itemTarget;
        usedTools = _usedTools;
        usedIngredients = _usedIngredients;
        actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(_cooldown);
    }
    return correct;
}

bool Action::setCombat(Character * opponent, const CombatAction & nextAction)
{
    // Check if the opponent is provided.
    if (opponent == nullptr)
    {
        Logger::log(LogLevel::Error, "No opponent is set.");
        return false;
    }

    // Add the opponent
    if (!opponents.addOpponent(opponent, opponents.getInitialAggro(opponent)))
    {
        Logger::log(LogLevel::Error, "Already fighting against '" + opponent->getName() + "'.");
        return false;
    }

    // Set the type of action to combat.
    type = ActionType::Combat;
    nextCombatAction = nextAction;
    // Set the action cooldown.
    actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(this->getNextAttack());
    return true;
}

unsigned int Action::getNextAttack()
{
    unsigned int base = 6.0;
    // The agility modifier.
    double agility = (actor->agility + actor->effects.getAgiMod()) * 3.5 * (base / 100);
    // The weight modifier.
    double weight = actor->weight * (base / 100);
    // The carried weight.
    double carriedWeight = (actor->getCarryingWeight() / 2) * (base / 100);

    double equippedWeight = 0;
    Item * rh = actor->findEquipmentSlotItem(EquipmentSlot::RightHand);
    if (rh != nullptr)
    {
        if (rh->model->type == ModelType::Weapon)
        {
            equippedWeight += rh->getWeight() * 2.5 * (base / 100);
        }
        else if (rh->model->type == ModelType::Shield)
        {
            equippedWeight += rh->getWeight() * 3.0 * (base / 100);
        }
    }
    Item * lh = actor->findEquipmentSlotItem(EquipmentSlot::LeftHand);
    if (lh != nullptr)
    {
        if (lh->model->type == ModelType::Weapon)
        {
            equippedWeight += lh->getWeight() * 2.5 * (base / 100);
        }
        else if (lh->model->type == ModelType::Shield)
        {
            equippedWeight += lh->getWeight() * 3.0 * (base / 100);
        }
    }
    unsigned int nextAttack = static_cast<unsigned int>(base - agility + weight + carriedWeight + equippedWeight);
    Logger::log(LogLevel::Debug, actor->getNameCapital() + " will attack in " + ToString(nextAttack));
    return nextAttack;
}

void Action::performAttack(Character * opponent)
{
    // Evaluate only once the armor class of the opponent.
    unsigned int AC = opponent->getArmorClass();
    // Evaluate only once the strenght modifier.
    unsigned int STR = GetAbilityModifier((actor->strength + actor->effects.getStrMod()));
    // Retrieve the items.
    Item * rh = actor->findEquipmentSlotItem(EquipmentSlot::RightHand);
    Item * lh = actor->findEquipmentSlotItem(EquipmentSlot::LeftHand);
    // If there are no weapons equiped.
    if ((rh == nullptr) && (lh == nullptr))
    {
        // Roll the attack.
        unsigned int ATK = RandInteger(1, 20);
        // Log the rolled value.
        Logger::log(LogLevel::Debug, "%s rolls a %s against %s.", actor->getNameCapital(), ToString(ATK), ToString(AC));
        // Check if its a hit.
        if (ATK < AC)
        {
            actor->sendMsg("You miss %s with your fist.\n", opponent->getName());
            opponent->sendMsg("%s misses you with %s fist.\n\n",
                actor->getNameCapital(), actor->getPossessivePronoun());
            return;
        }
        // Roll the damage.
        unsigned int DMG = RandInteger(1, 3) + STR;
        // Log the rolled value.
        Logger::log(LogLevel::Debug, "%s hit %s for %s.", actor->getNameCapital(), opponent->getName(), ToString(DMG));
        actor->sendMsg("You hit %s with your fist for %s.\n", opponent->getName(), ToString(DMG));
        opponent->sendMsg("%s hits you with %s fist for %s.\n\n",
            actor->getNameCapital(),
            actor->getPossessivePronoun(),
            ToString(DMG));
        return;
    }

    // Check with which hand the character can attack.
    bool rhCanAttack = actor->canAttackWith(EquipmentSlot::RightHand);
    bool lhCanAttack = actor->canAttackWith(EquipmentSlot::LeftHand);
    // If there is a weapon in the right hand.
    if (rhCanAttack)
    {
        // Roll the attack.
        unsigned int ATK = RandInteger(1, 20);
        // Evaluate any kind of hit penality.
        if (rhCanAttack && lhCanAttack)
        {
            // Apply any penality.
            if (ATK > 4)
            {
                ATK -= 4;
            }
            else
            {
                ATK = 0;
            }
        }
        // Log the rolled value.
        Logger::log(LogLevel::Debug, "%s rolls a %s against %s.", actor->getNameCapital(), ToString(ATK), ToString(AC));
        // Check if its a hit.
        if (ATK < AC)
        {
            actor->sendMsg("You miss %s with %s.\n", opponent->getName(), rh->getName());
            opponent->sendMsg("%s misses you with %s %s.\n\n",
                actor->getNameCapital(), actor->getPossessivePronoun(),
                rh->getName());
            return;
        }
        // Roll the damage.
        unsigned int DMG = 1;
        DMG += RandInteger(rh->model->getWeaponFunc().minDamage, rh->model->getWeaponFunc().maxDamage);
        if (HasFlag(rh->model->flags, ModelFlag::TwoHand))
        {
            DMG += static_cast<unsigned int>(STR * 1.5);
        }
        else
        {
            DMG += STR;
        }
        // Log the damage.
        Logger::log(LogLevel::Debug, "%s hit %s for %s with %s.",
            actor->getNameCapital(), opponent->getName(),
            ToString(DMG), rh->getName());
        // Send the messages to the characters.
        actor->sendMsg("You hit %s with your %s for %s.\n", opponent->getName(), rh->getName(), ToString(DMG));
        opponent->sendMsg("%s hits you with %s %s for %s.\n\n",
            actor->getNameCapital(), actor->getPossessivePronoun(),
            rh->getName(), ToString(DMG));
    }

    // If there is a weapon in the right hand.
    if (lhCanAttack)
    {
        // Roll the attack.
        unsigned int ATK = RandInteger(1, 20);
        // Evaluate any kind of hit penality.
        if (rhCanAttack && lhCanAttack)
        {
            // Apply any penality.
            if (ATK > 4)
            {
                ATK -= 4;
            }
            else
            {
                ATK = 0;
            }
        }
        // Log the rolled value.
        Logger::log(LogLevel::Debug, "%s rolls a %s against %s.", actor->getNameCapital(), ToString(ATK), ToString(AC));
        // Check if its a hit.
        if (ATK < AC)
        {
            actor->sendMsg("You miss %s with %s.\n", opponent->getName(), lh->getName());
            opponent->sendMsg("%s misses you with %s %s.\n\n",
                actor->getNameCapital(), actor->getPossessivePronoun(),
                lh->getName());
            return;
        }
        // Roll the damage.
        unsigned int DMG = 1;
        DMG += RandInteger(lh->model->getWeaponFunc().minDamage, lh->model->getWeaponFunc().maxDamage);
        DMG += static_cast<unsigned int>(STR * 0.5);
        // Log the damage.
        Logger::log(LogLevel::Debug, "%s hit %s for %s with %s.",
            actor->getNameCapital(), opponent->getName(),
            ToString(DMG), lh->getName());
        // Send the messages to the characters.
        actor->sendMsg("You hit %s with your %s for %s.\n", opponent->getName(), lh->getName(), ToString(DMG));
        opponent->sendMsg("%s hits you with %s %s for %s.\n\n",
            actor->getNameCapital(), actor->getPossessivePronoun(),
            lh->getName(), ToString(DMG));
    }

    if (!rhCanAttack && !lhCanAttack)
    {
        actor->sendMsg("You do not have a valid weapon equipped.");
    }
}

Character * Action::getNextOpponent()
{
    if (opponents.hasOpponents())
    {
        // Get the top aggressor.
        const Aggression aggression = opponents.getTopAggro();
        // Retrieve the opponent.
        Character * opponent = aggression.aggressor;
        if (opponent != nullptr)
        {
            if (opponent->room->vnum == actor->room->vnum)
            {
                return opponent;
            }
        }
    }
    return nullptr;
}

bool Action::checkElapsed()
{
    TimeClock currentTime = std::chrono::system_clock::now();
    return (std::chrono::duration_cast<std::chrono::seconds>(actionCooldown - currentTime).count() <= 0);
}

void Action::performMove()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }
    // Define departure message.
    std::string msgDepart = actor->getNameCapital() + " goes " + GetDirectionName(direction) + ".\n";
    // Define arrival message.
    std::string msgArrive = actor->getNameCapital() + " arives from " + GetDirectionName(InverDirection(direction))
        + ".\n";
    // Move character.
    actor->moveTo(destination, msgDepart, msgArrive);
    // Return the character in waiting status.
    this->reset();
}

void Action::performMine()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }

    // Check if the presence of the tool.
    if (usedTools.empty())
    {
        // Notify the character.
        actor->sendMsg("You don't have any tool.\n");
        return;
    }
    else if (usedTools.front() == nullptr)
    {
        // Notify the character.
        actor->sendMsg("You don't have the right tool.\n");
        return;
    }
    // Check if the vein is still there.
    if (itemTarget == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":mine = Vein is a null pointer.");
        // Notify the character.
        actor->sendMsg("You have failed your action.\n");
        return;
    }
    // Retrieve the product.
    Model * product = Mud::instance().findModel(itemTarget->model->getNodeFunc().provides);
    // Check if the product exists.
    if (product == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":mine = Product is a null pointer.");
        // Notify the character.
        actor->sendMsg("You have failed your action.\n");
        return;
    }
    // Retrieve the material of the resources node.
    Material * material = itemTarget->composition;
    // Check the existance of the material.
    if (material == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":mine = Material is a null pointer.");
        // Notify the character.
        actor->sendMsg("You have failed your action.\n");
        return;
    }
    // Create the product.
    SQLiteDbms::instance().beginTransaction();
    Item * newItem = product->createItem(actor->getName(), material, ItemQuality::Normal);
    // Check if the creation is not null.
    if (newItem == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":mine = The new item is a null pointer.");
        // Rollback the database.
        SQLiteDbms::instance().rollbackTransection();
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        // Delete the item.
        delete (newItem);
        return;
    }
    SQLiteDbms::instance().endTransaction();
    // Check if the character can carry the item.
    if (!actor->canCarry(newItem))
    {
        actor->sendMsg(newItem->getName() + "is too heavy, it will be left on the ground.\n");
        actor->room->addItem(newItem);
    }
    else
    {
        actor->addInventoryItem(newItem);
    }
    // Update the condition of the involved objects.
    if (itemTarget->editCondition(-10))
    {
        actor->sendMsg("With your last blow, " + itemTarget->getName() + " crumbles into pieces..\n");
        actor->room->sendToAll("The " + itemTarget->getName() + " crumbles into pieces.", actor);
        // Destroy the vein.
        itemTarget->destroy();
    }
    // Check if the tool is destroyed.
    Item * tool = dynamic_cast<Item *>(usedTools.front());
    if (tool->editCondition(-1))
    {
        actor->sendMsg(tool->getName() + " falls into pieces.");
        tool->destroy();
    }

    // Advise character that he has ended the action.
    actor->sendMsg("You have successfully extracted some minerals.\n\n");

    // Return the character in waiting status.
    this->reset();
}

void Action::performChop()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }

    // Check if the presence of the tool.
    if (usedTools.empty())
    {
        // Notify the character.
        actor->sendMsg("\nYou don't have any tool.\n");
        return;
    }
    else if (usedTools.front() == nullptr)
    {
        // Notify the character.
        actor->sendMsg("\nYou don't have the right tool.\n");
        return;
    }
    // Check if the vein is still there.
    if (itemTarget == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":chop = Tree is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Retrieve the product.
    Model * product = Mud::instance().findModel(itemTarget->model->getNodeFunc().provides);
    // Check if the product exists.
    if (product == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":chop = Product is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Retrieve the material of the resources node.
    Material * material = itemTarget->composition;
    // Check the existance of the material.
    if (material == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":chop = Material is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Create the product.
    SQLiteDbms::instance().beginTransaction();
    Item * newItem = product->createItem(actor->getName(), material, ItemQuality::Normal);
    // Check if the creation is not null.
    if (newItem == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":chop = The new item is a null pointer.");
        // Rollback the database.
        SQLiteDbms::instance().rollbackTransection();
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        // Delete the item.
        delete (newItem);
        return;
    }
    SQLiteDbms::instance().endTransaction();

    // Check if the character can carry the item.
    if (!actor->canCarry(newItem))
    {
        actor->sendMsg(newItem->getName() + "is too heavy, it will be left on the ground.\n");
        actor->room->addItem(newItem);
    }
    else
    {
        actor->addInventoryItem(newItem);
    }
    // Update the condition of the involved objects.
    if (itemTarget->editCondition(-10))
    {
        actor->sendMsg("\nWith your last blow, " + itemTarget->getName() + " falls down..\n");
        actor->room->sendToAll("The " + itemTarget->getName() + " falls down.", actor);
        // Destroy the vein.
        itemTarget->destroy();
    }
    // Check if the tool is destroyed.
    Item * tool = dynamic_cast<Item *>(usedTools.front());
    if (tool->editCondition(-1))
    {
        actor->sendMsg(tool->getName() + " falls into pieces.");
        tool->destroy();
    }

    // Advise character that he has ended the action.
    actor->sendMsg("\nYou have successfully cut some branches.\n\n");

    // Return the character in waiting status.
    this->reset();
}

void Action::performCraft()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }

    // Check if the production is not a null pointer.
    if (production == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":craft = Production is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Check the number of ingredients.
    if (usedIngredients.empty())
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":craft = No ingredients.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Check the ingredients and the used tools.
    for (auto iterator : usedIngredients)
    {
        if (iterator == nullptr)
        {
            // Log a warning.
            Logger::log(LogLevel::Warning, actor->getName() + ":craft = One ingredient is a null pointer.");
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            return;
        }
    }
    for (auto iterator : usedTools)
    {
        if (iterator == nullptr)
        {
            // Log a warning.
            Logger::log(LogLevel::Warning, actor->getName() + ":craft = One tool is a null pointer.");
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            return;
        }
    }

    // Check the crafring material.
    if (craftMaterial == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":craft = Crafting material is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    ItemVector toDestroy;
    for (auto it : usedTools)
    {
        // Update the condition of the involved objects.
        if (it->editCondition(-1))
        {
            actor->sendMsg(it->getName() + " falls into pieces.");
            toDestroy.push_back(it);
        }
    }
    for (auto it : usedIngredients)
    {
        toDestroy.push_back(it);
    }
    for (auto it : toDestroy)
    {
        it->destroy();
    }

    SQLiteDbms::instance().beginTransaction();
    ItemVector createdItems;
    for (unsigned int it = 0; it < production->outcome.second; ++it)
    {
        Model * outcomeMode = production->outcome.first;
        // Create the item.
        Item * newItem = outcomeMode->createItem(actor->getName(), craftMaterial, ItemQuality::Normal);
        if (newItem == nullptr)
        {
            // Log a warning.
            Logger::log(LogLevel::Warning, actor->getName() + ":craft = New item is a null pointer.");
            // Rollback the database.
            SQLiteDbms::instance().rollbackTransection();
            // Delete all the items created so far.
            for (auto createdItem : createdItems)
            {
                delete (createdItem);
            }
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            return;
        }
        createdItems.push_back(newItem);
    }
    SQLiteDbms::instance().endTransaction();

    // Add the created items into the character inventory.
    bool dropped = false;
    for (auto createdItem : createdItems)
    {
        if (!actor->canCarry(createdItem))
        {
            actor->room->addItem(createdItem);
            dropped = true;
        }
        else
        {
            actor->addInventoryItem(createdItem);
        }
    }

    // Send conclusion message.
    std::string msg;
    msg += "\n" + production->profession->finishMessage;
    msg += " " + Telnet::yellow() + createdItems.back()->getName() + Telnet::reset() + ".\n\n";
    actor->sendMsg(msg);

    if (dropped)
    {
        actor->sendMsg("Since you can't carry them, some of the items have been placed on the ground.\n\n");
    }

    // Return the character in waiting status.
    this->reset();
}

void Action::performBuild()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }

    // Check the schematics.
    if (schematics == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":craft = Building is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        // Reset the action values.
        this->reset();
        return;
    }

    // Check the ingredients and used tools.
    for (auto iterator : usedIngredients)
    {
        if (iterator == nullptr)
        {
            // Log a warning.
            Logger::log(LogLevel::Warning, actor->getName() + ":craft = One ingredient is a null pointer.");
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            // Reset the action values.
            this->reset();
            return;
        }
    }
    for (auto iterator : usedTools)
    {
        if (iterator == nullptr)
        {
            // Log a warning.
            Logger::log(LogLevel::Warning, actor->getName() + ":craft = One tool is a null pointer.");
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            // Reset the action values.
            this->reset();
            return;
        }
    }

    ItemVector toDestroy;
    for (auto iterator : usedTools)
    {
        // Update the condition of the involved objects.
        if (iterator->editCondition(-1))
        {
            actor->sendMsg(iterator->getName() + " falls into pieces.");
            toDestroy.push_back(iterator);
        }
    }
    for (auto it : usedIngredients)
    {
        toDestroy.push_back(it);
    }
    for (auto it : toDestroy)
    {
        it->destroy();
    }

    if (schematics->buildingModel == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":craft = The building model is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        // Reset the action values.
        this->reset();
        return;
    }
    SQLiteDbms::instance().beginTransaction();
    actor->remInventoryItem(itemTarget);
    actor->room->addBuilding(itemTarget);
    itemTarget->updateOnDB();
    SQLiteDbms::instance().endTransaction();

    // Send conclusion message.
    std::string msg;
    msg += "You have finished building";
    msg += " " + Telnet::yellow() + schematics->buildingModel->getName() + Telnet::reset() + ".\n\n";
    actor->sendMsg(msg);

    // Return the character in waiting status.
    this->reset();
}

void Action::performComb()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }
    // Check if the character has at least some opponents.
    if (nextCombatAction == CombatAction::BasicAttack)
    {
        // Retrieve the opponent.
        Character * opponent = this->getNextOpponent();
        if (opponent != nullptr)
        {
            // Perform the attack.
            this->performAttack(opponent);
            // Set the action cooldown.
            actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(this->getNextAttack());
            return;
        }
    }
    actor->sendMsg("You stop fighting.\n\n");
    this->reset();
}

void Action::reset()
{
    type = ActionType::Wait;
    target = nullptr;
    itemTarget = nullptr;
    destination = nullptr;
    direction = Direction::None;
    production = nullptr;
    schematics = nullptr;
    craftMaterial = nullptr;
    usedTools.clear();
    usedIngredients.clear();
}
