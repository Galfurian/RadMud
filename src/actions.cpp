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

std::string Action::getDescription()
{
    if (this->type == ActionType::Move)
    {
        return "moving";
    }
    else if (this->type == ActionType::Crafting)
    {
        if (this->production != nullptr)
        {
            if (this->production->profession != nullptr)
            {
                return this->production->profession->action;
            }
        }
    }
    else if (this->type == ActionType::Building)
    {
        return "moving";
    }
    else if (this->type == ActionType::Combat)
    {
        return "fighting";
    }
    return "";
}

std::string Action::stop()
{
    std::string msg;
    if (this->type == ActionType::Wait)
    {
        msg = "Excuse me sir, but you are doing nothing.";
    }
    else if (this->type == ActionType::Move)
    {
        msg = "You stop moving.";
    }
    else if (this->type == ActionType::Crafting)
    {
        if (this->production != nullptr)
        {
            if (this->production->profession != nullptr)
            {
                msg = this->production->profession->interruptMessage + ".";
            }
        }
    }
    else if (this->type == ActionType::Building)
    {
        msg = "You stop building.";
    }
    else if (this->type == ActionType::Combat)
    {
        msg = "You stop fighting.";
    }
    else
    {
        msg = "You stop whatever you were doing.";
    }
    this->reset();
    return msg;
}

void Action::perform()
{
    if (this->type == ActionType::Move)
    {
        this->performMove();
    }
    else if (this->type == ActionType::Crafting)
    {
        this->performCraft();
    }
    else if (this->type == ActionType::Building)
    {
        this->performBuild();
    }
    else if (this->type == ActionType::Combat)
    {
        this->performComb();
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
    ItemVector & _usedTools,
    ItemVector & _usedIngredients,
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
    ItemVector & _usedTools,
    ItemVector & _usedIngredients,
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

bool Action::setInCombat()
{
    if (this->type != ActionType::Wait)
    {
        return false;
    }
    else
    {
        this->type = ActionType::Combat;
        return true;
    }
}

bool Action::setNextCombatAction(const CombatAction & nextAction)
{
    if (!actor->opponents.hasOpponents())
    {
        Logger::log(LogLevel::Error, "The list of opponents is empty.");
        return false;
    }
    if (type != ActionType::Combat)
    {
        Logger::log(LogLevel::Error, "The character is not fighting.");
        return false;
    }
    // Set the next combat action.
    nextCombatAction = nextAction;
    // Set the action cooldown.
    actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(actor->getCooldown(nextCombatAction));
    return true;
}

CombatAction Action::getNextCombatAction() const
{
    return this->nextCombatAction;
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
    Model * product = Mud::instance().findModel(static_cast<int>(itemTarget->model->getNodeFunc().provides));
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
        actor->sendMsg("%s is too heavy, it will be left on the ground.\n", newItem->getName());
        actor->room->addItem(newItem);
    }
    else
    {
        actor->addInventoryItem(newItem);
    }
    // Update the condition of the involved objects.
    if (itemTarget->editCondition(-10))
    {
        actor->sendMsg("With your last blow, %s crumbles into pieces.\n", itemTarget->getName());
        // Set the list of exceptions.
        CharacterVector exceptions;
        exceptions.push_back(actor);
        // Send the message inside the room.
        actor->room->sendToAll("%s crumbles into pieces.\n", exceptions, itemTarget->getNameCapital());
        // Destroy the vein.
        itemTarget->destroy();
    }
    // Check if the tool is destroyed.
    Item * tool = dynamic_cast<Item *>(usedTools.front());
    if (tool->editCondition(-1))
    {
        actor->sendMsg("%s falls into pieces.\n", tool->getName());
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
    Model * product = Mud::instance().findModel(static_cast<int>(itemTarget->model->getNodeFunc().provides));
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
        actor->sendMsg("%s is too heavy, it will be left on the ground.\n", newItem->getName());
        actor->room->addItem(newItem);
    }
    else
    {
        actor->addInventoryItem(newItem);
    }
    // Update the condition of the involved objects.
    if (itemTarget->editCondition(-10))
    {
        actor->sendMsg("\nWith your last blow, %s falls down.\n", itemTarget->getName());
        // Set the list of exceptions.
        CharacterVector exceptions;
        exceptions.push_back(actor);
        // Send the message inside the room.
        actor->room->sendToAll("%s falls down.", exceptions, itemTarget->getNameCapital());
        // Destroy the vein.
        itemTarget->destroy();
    }
    // Check if the tool is destroyed.
    Item * tool = dynamic_cast<Item *>(usedTools.front());
    if (tool->editCondition(-1))
    {
        actor->sendMsg("%s falls into pieces.\n", tool->getName());
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
            actor->sendMsg("%s falls into pieces.\n", it->getName());
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
    actor->sendMsg("%s %s.\n\n", production->profession->finishMessage,
        Formatter::yellow() + createdItems.back()->getName() + Formatter::reset());

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
    actor->sendMsg("You have finished building %s.\n\n",
        Formatter::yellow() + schematics->buildingModel->getName() + Formatter::reset());

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
    // The sequence is the following:
    //  1. Check the list of opponents.
    //  2. Get the target. It could eighter be the top aggro or an allied.
    //  3. Check if the target is at range (depending on the action).
    //  4. If not return to 2.
    //  5. Otherwise perform the action.

    // Check the list of opponents.
    actor->opponents.checkList();
    if (nextCombatAction == CombatAction::BasicAttack)
    {
        // Perform the combat action.
        this->performCombatAction(nextCombatAction);
    }
    else if (nextCombatAction == CombatAction::Flee)
    {
        // Perform the combat action.
        this->performCombatAction(nextCombatAction);
    }
    else
    {
        actor->sendMsg("You stop fighting.\n\n");
        this->reset();
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

void Action::performCombatAction(const CombatAction & move)
{
    if (move == CombatAction::BasicAttack)
    {
        ItemVector activeWeapons = actor->getActiveWeapons();
        if (activeWeapons.empty())
        {
            actor->sendMsg("You do not have a valid weapon equipped.\n");
        }
        else
        {
            for (auto iterator : activeWeapons)
            {
                WeaponFunc wFun = iterator->model->getWeaponFunc();
                // Get the top aggro enemy at range.
                Character * enemy = actor->getNextOpponentAtRange(wFun.range);
                if (enemy == nullptr)
                {
                    continue;
                }
                // Roll the attack.
                unsigned int ATK = TRandInteger<unsigned int>(1, 20);
                // Evaluate the armor class of the enemy.
                unsigned int AC = enemy->getArmorClass();
                // Log the rolled value.
                Logger::log(LogLevel::Debug, "%s rolls a %s against %s.", actor->getNameCapital(), ToString(ATK),
                    ToString(AC));
                // Check if its a hit.
                if (ATK < AC)
                {
                    actor->sendMsg("You miss %s with %s.\n", enemy->getName(), iterator->getName());
                    enemy->sendMsg("%s misses you with %s.\n\n", actor->getNameCapital(), iterator->getName());
                }
                else
                {
                    // Roll the damage.
                    unsigned int DMG = 1;
                    DMG += TRandInteger<unsigned int>(wFun.minDamage, wFun.maxDamage);
                    // Log the damage.
                    Logger::log(LogLevel::Debug, "%s hit %s for %s with %s.", actor->getNameCapital(), enemy->getName(),
                        ToString(DMG), iterator->getName());
                    // Send the messages to the characters.
                    actor->sendMsg("You hit %s with %s for %s.\n", enemy->getName(), iterator->getName(),
                        ToString(DMG));
                    enemy->sendMsg("%s hits you with %s for %s.\n\n", actor->getNameCapital(), iterator->getName(),
                        ToString(DMG));
                }
            }
        }
    }
    else if (move == CombatAction::Flee)
    {
        // Get the character chance of fleeing (D20).
        unsigned int fleeChance = TRandInteger<unsigned int>(0, 20) + GetAbilityModifier(actor->getAgility());
        // Base the escape level on how many enemies are surrounding the character.
        if (fleeChance < static_cast<unsigned int>(actor->opponents.getSize()))
        {
            actor->sendMsg("You were not able to escape from your attackers.\n");
        }
        else
        {
            std::vector<Direction> directions = actor->room->getAvailableDirections();
            if (!directions.empty())
            {
                Direction randomDirection = directions.at(TRandInteger<size_t>(0, directions.size() - 1));
                Exit * selected = actor->room->findExit(randomDirection);
                if (selected == nullptr)
                {
                    Logger::log(LogLevel::Error, "Selected null exit during action Flee.");
                    actor->sendMsg("You were not able to escape from your attackers.\n");
                }
                else
                {
                    actor->moveTo(selected->destination, actor->getNameCapital() + " flees from the battlefield.\n\n",
                        actor->getNameCapital() + " arives fleeing.\n\n", "You flee from the battlefield.\n");
                    this->setNextCombatAction(CombatAction::NoAction);
                    return;
                }
            }
        }
    }
    // By default set the next combat action to basic attack.
    this->setNextCombatAction(CombatAction::BasicAttack);
}
