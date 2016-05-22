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

#include "constants.hpp"
#include "mud.hpp"

Action::Action(Character * _character) :
    type(ActionType::Wait), actor(_character), target(), itemTarget(), destination(), direction(Direction::None), production(), schematics(), craftMaterial(), usedTools(), usedIngredients(), actionCooldown(), opponents()
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
        LogError("No destination has been set.");
        correct = false;
    }
    if (_direction == Direction::None)
    {
        LogError("No direction has been set.");
        correct = false;
    }
    if (_cooldown == 0)
    {
        LogError("No cooldown has been set.");
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
        LogError("No profession has been set.");
        correct = false;
    }
    if (_usedTools.empty())
    {
        LogError("No used tools have been set.");
        correct = false;
    }
    if (_usedIngredients.empty())
    {
        LogError("No used ingredients have been set.");
        correct = false;
    }
    if (_craftMaterial == nullptr)
    {
        LogError("No crafting material has been set.");
        correct = false;
    }
    if (_cooldown == 0)
    {
        LogError("No cooldown has been set.");
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
        LogError("No schematics have been set.");
        correct = false;
    }
    if (_itemTarget == nullptr)
    {
        LogError("No item is set for the building.");
        correct = false;
    }
    if (_usedTools.empty())
    {
        LogError("No used tools have been set.");
        correct = false;
    }
    if (_cooldown == 0)
    {
        LogError("No cooldown has been set.");
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

bool Action::setCombat(Character * opponent)
{
    if (opponent == nullptr)
    {
        LogError("No opponent is set.");
        return false;
    }
    // Set the initial aggro as the difference between the fighters levels.
    unsigned int initialAggro = ::abs(opponent->level - actor->level);

    // Add the opponent
    if (!opponents.addOpponent(opponent, initialAggro))
    {
        actor->sendMsg("You are already fighting against " + opponent->getName());
        return false;
    }

    // Set the type of action to combat.
    type = ActionType::Combat;

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
    Item * rightHand = actor->findEquipmentSlotItem(EquipmentSlot::RightHand);
    if (rightHand != nullptr)
    {
        if (rightHand->model->type == ModelType::Weapon)
        {
            equippedWeight += rightHand->getWeight() * 2.5 * (base / 100);
        }
        else if (rightHand->model->type == ModelType::Shield)
        {
            equippedWeight += rightHand->getWeight() * 3.0 * (base / 100);
        }
    }
    Item * leftHand = actor->findEquipmentSlotItem(EquipmentSlot::RightHand);
    if (leftHand != nullptr)
    {
        if (leftHand->model->type == ModelType::Weapon)
        {
            equippedWeight += leftHand->getWeight() * 2.5 * (base / 100);
        }
        else if (leftHand->model->type == ModelType::Shield)
        {
            equippedWeight += leftHand->getWeight() * 3.0 * (base / 100);
        }
    }
    return static_cast<unsigned int>(base - agility + weight + carriedWeight + equippedWeight);
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
        LogWarning(actor->getName() + ":mine = Vein is a null pointer.");
        // Notify the character.
        actor->sendMsg("You have failed your action.\n");
        return;
    }
    // Retrieve the product.
    Model * product = Mud::getInstance().findModel(itemTarget->model->getNodeFunc().provides);
    // Check if the product exists.
    if (product == nullptr)
    {
        // Log a warning.
        LogWarning(actor->getName() + ":mine = Product is a null pointer.");
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
        LogWarning(actor->getName() + ":mine = Material is a null pointer.");
        // Notify the character.
        actor->sendMsg("You have failed your action.\n");
        return;
    }
    // Create the product.
    Mud::getInstance().getDbms().beginTransaction();
    Item * newItem = product->createItem(actor->getName(), material, ItemQuality::Normal);
    // Check if the creation is not null.
    if (newItem == nullptr)
    {
        // Log a warning.
        LogWarning(actor->getName() + ":mine = The new item is a null pointer.");
        // Rollback the database.
        Mud::getInstance().getDbms().rollbackTransection();
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        // Delete the item.
        delete (newItem);
        return;
    }
    Mud::getInstance().getDbms().endTransaction();
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
        LogWarning(actor->getName() + ":chop = Tree is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Retrieve the product.
    Model * product = Mud::getInstance().findModel(itemTarget->model->getNodeFunc().provides);
    // Check if the product exists.
    if (product == nullptr)
    {
        // Log a warning.
        LogWarning(actor->getName() + ":chop = Product is a null pointer.");
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
        LogWarning(actor->getName() + ":chop = Material is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Create the product.
    Mud::getInstance().getDbms().beginTransaction();
    Item * newItem = product->createItem(actor->getName(), material, ItemQuality::Normal);
    // Check if the creation is not null.
    if (newItem == nullptr)
    {
        // Log a warning.
        LogWarning(actor->getName() + ":chop = The new item is a null pointer.");
        // Rollback the database.
        Mud::getInstance().getDbms().rollbackTransection();
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        // Delete the item.
        delete (newItem);
        return;
    }
    Mud::getInstance().getDbms().endTransaction();

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
        LogWarning(actor->getName() + ":craft = Production is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Check the number of ingredients.
    if (usedIngredients.empty())
    {
        // Log a warning.
        LogWarning(actor->getName() + ":craft = No ingredients.");
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
            LogWarning(actor->getName() + ":craft = One ingredient is a null pointer.");
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
            LogWarning(actor->getName() + ":craft = One tool is a null pointer.");
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            return;
        }
    }

    // Check the crafring material.
    if (craftMaterial == nullptr)
    {
        // Log a warning.
        LogWarning(actor->getName() + ":craft = Crafting material is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    ItemList toDestroy;
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

    Mud::getInstance().getDbms().beginTransaction();
    ItemList createdItems;
    for (unsigned int it = 0; it < production->outcome.second; ++it)
    {
        Model * outcomeMode = production->outcome.first;
        // Create the item.
        Item * newItem = outcomeMode->createItem(actor->getName(), craftMaterial, ItemQuality::Normal);
        if (newItem == nullptr)
        {
            // Log a warning.
            LogWarning(actor->getName() + ":craft = New item is a null pointer.");
            // Rollback the database.
            Mud::getInstance().getDbms().rollbackTransection();
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
    Mud::getInstance().getDbms().endTransaction();

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
        LogWarning(actor->getName() + ":craft = Building is a null pointer.");
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
            LogWarning(actor->getName() + ":craft = One ingredient is a null pointer.");
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
            LogWarning(actor->getName() + ":craft = One tool is a null pointer.");
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            // Reset the action values.
            this->reset();
            return;
        }
    }

    ItemList toDestroy;
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
        LogWarning(actor->getName() + ":craft = The building model is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        // Reset the action values.
        this->reset();
        return;
    }
    Mud::getInstance().getDbms().beginTransaction();
    actor->remInventoryItem(itemTarget);
    actor->room->addBuilding(itemTarget);
    itemTarget->updateOnDB();
    Mud::getInstance().getDbms().endTransaction();

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
    if (!opponents.hasOpponents())
    {
        actor->sendMsg("You stop fighting.");
        this->reset();
    }
    // Get the top aggressor.
    const Aggression aggression = opponents.getTopAggro();
    // Retrieve the opponent.
    Character * opponent = aggression.aggressor;
    if (opponent == nullptr)
    {
        actor->sendMsg("You stop fighting.");
        this->reset();
    }
    else
    {
        if (opponent->room->vnum != actor->room->vnum)
        {
            actor->sendMsg("You stop fighting.");
            this->reset();
        }
        else
        {
            actor->sendMsg("You attack '" + opponent->getName() + "', combat is not implemented.");
            // Set the action cooldown.
            actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(this->getNextAttack());
        }
    }
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